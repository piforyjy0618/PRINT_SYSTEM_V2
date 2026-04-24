#include "udp_async_client.h"
#include "spdlog/spdlog.h"

UdpASyncClient::UdpASyncClient(std::shared_ptr<io_context> ioContext,
                               const int localPort) : m_ioContext(ioContext),
                                                      m_socket(new udp::socket(*m_ioContext, udp::endpoint(udp::v4(), localPort)))
{
}

UdpASyncClient::~UdpASyncClient()
{
}

bool UdpASyncClient::OpenConnect(const std::string &host, const int port)
{
    try
    {
        udp::resolver resolver(*m_ioContext);
        auto endpoints = resolver.resolve(udp::v4(), host, std::to_string(port));
        m_serverEndpoint = std::unique_ptr<udp::endpoint>(new udp::endpoint(*endpoints.begin()));
        // 初始化最后收到数据的时间
        m_lastReceiveTimeMs.store(GetCurrentTimeMs());
        this->StartReceive();

        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("UDP OpenConnect failed: {}", e.what());
        return false;
    }
}

void UdpASyncClient::CloseConnect()
{
    // 停止 socket
    if (m_socket && m_socket->is_open())
    {
        boost::system::error_code ec;
        m_socket->close(ec);
    }
    spdlog::info("UDP Client Closed.");
}

bool UdpASyncClient::IsConnected() const
{
    // 如果本地 socket 已经处于关闭状态，直接返回 false
    if (!m_socket)
    {
        return false;
    }
    // 检查距离上一次收到数据过去了多久
    int64_t now = GetCurrentTimeMs();
    int64_t lastTime = m_lastReceiveTimeMs.load();
    // 如果差值大于设定的阈值，说明心跳超时
    if ((now - lastTime) > HEARTBEAT_TIMEOUT_MS)
    {
        return false;
    }
    return true;
}

bool UdpASyncClient::Send(const std::string &message)
{
    if (!m_socket || !m_socket->is_open())
    {
        return false;
    }

    auto self = shared_from_this();
    auto data = std::make_shared<std::vector<uint8_t>>(message.begin(), message.end());

    boost::asio::post(*m_ioContext,
                      [this, self, data]()
                      {
                          m_sendQueue.push(*data); // 将数据入队

                          // 如果当前没有发送任务正在进行，则启动发送
                          if (!m_isSending)
                          {
                              m_isSending = true; // 标记状态为正在发送
                              DoAsyncWrite();     // 触发实际的异步发送
                          }
                      });
    return true;
}

void UdpASyncClient::DoAsyncWrite()
{
    auto self = shared_from_this();
    m_isSending = true;
    // 取出队首数据进行发送
    m_socket->async_send_to(
        boost::asio::buffer(m_sendQueue.front()),
        *m_serverEndpoint,
        [this, self](const boost::system::error_code &ec, std::size_t bytesSent)
        {
            // 发送完成后，触发回调处理
            HandleSend(ec, bytesSent);
        });
}

void UdpASyncClient::StartReceive()
{
    auto self = shared_from_this();
    m_socket->async_receive_from(boost::asio::buffer(m_recvBuffer),
                                 m_senderEndpoint,
                                 [this, self](const boost::system::error_code& error, std::size_t size)
                                         {
                                             this->HandleReceive(error, size);
                                         });
}

void UdpASyncClient::HandleReceive(const boost::system::error_code &error, std::size_t size)
{
    if (!error)
        {
            // 刷新时间戳
            m_lastReceiveTimeMs.store(GetCurrentTimeMs());

            std::string recvString(m_recvBuffer.data(), size);
            DataReceived(recvString);

            // 继续接收数据
            this->StartReceive();
        }
        else
        {
            if (error != boost::asio::error::operation_aborted) {
                spdlog::error("UDP Receive error: {}", error.message());
            }
        }
}

void UdpASyncClient::HandleSend(const boost::system::error_code &ec, std::size_t bytes_sent)
{
    (void)bytes_sent;
    if (!ec)
    {
        m_sendQueue.pop(); // 发送成功，移除队首
        if (!m_sendQueue.empty())
        {
            DoAsyncWrite(); // 队列不为空，继续发送下一个
        }
        else
        {
            m_isSending = false; // 队列清空，标记为空闲
        }
    }
    else
    {
        spdlog::error("Send error: {}", ec.message());
        CloseConnect();
    }
}
