#include "tcp_async_client.h"
#include "spdlog/spdlog.h"

TcpASyncClient::TcpASyncClient(std::shared_ptr<io_context> ioContext,
                               const int localPort) : m_localPort(localPort),
                                                      m_ioContext(ioContext),
                                                      m_socket(std::make_unique<tcp::socket>(*m_ioContext))
{
}

TcpASyncClient::~TcpASyncClient()
{
}

bool TcpASyncClient::OpenConnect(const std::string &host, const int port)
{
    try
    {
        m_socket = std::make_unique<tcp::socket>(*m_ioContext);
        tcp::resolver resolver(*m_ioContext);
        auto results = resolver.resolve(host, std::to_string(port));
        if (m_localPort != 0)
        {
            boost::system::error_code ec;
            m_socket->close(ec);
            m_socket->open(tcp::v4());
            m_socket->bind(tcp::endpoint(tcp::v4(), m_localPort));
        }
        // 使用异步连接
        auto self = shared_from_this();
        boost::asio::async_connect(*m_socket, results,
                                   [this, self, host, port](const boost::system::error_code &ec, const tcp::endpoint & /*endpoint*/)
                                   {
                                       if (!ec)
                                       {
                                           spdlog::info("Async TCP connected: {}:{}", host, port);
                                           // 连接成功时，初始化最后收到数据的时间
                                           m_lastReceiveTimeMs.store(GetCurrentTimeMs());
                                           StartReceive();
                                       }
                                       else
                                       {
                                           spdlog::error("Async TCP connect failed: {}", ec.message());
                                           CloseConnect();
                                       }
                                   });
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Async TCP connect failed: {}", e.what());
        return false;
    }
}

void TcpASyncClient::CloseConnect()
{
    auto self = shared_from_this();
    boost::asio::post(*m_ioContext,
                      [this, self]()
                      {
                          if (m_socket && m_socket->is_open())
                          {
                              boost::system::error_code ec;
                              m_socket->shutdown(tcp::socket::shutdown_both, ec);
                              m_socket->close(ec);
                          }
                      });
}

bool TcpASyncClient::IsConnected() const
{
    // 如果本地 socket 已经处于关闭状态，直接返回 false
    if (!m_socket || !m_socket->is_open())
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

bool TcpASyncClient::Send(const std::string &message)
{
    if (!m_socket || !m_socket->is_open())
    {
        spdlog::error("Send failed: socket not open");
        return false;
    }

    auto self = shared_from_this();
    auto data = std::make_shared<std::vector<uint8_t>>(message.begin(), message.end());

    boost::asio::post(*m_ioContext,
                      [this, self, data]()
                      {
                          m_sendQueue.push(*data);
                          if (!m_isSending)
                          {
                              DoAsyncWrite(); // 启动发送
                          }
                      });
    return true;
}

void TcpASyncClient::DoAsyncWrite()
{
    auto self = shared_from_this();
    m_isSending = true;
    // 取出队首数据进行发送
    boost::asio::async_write(*m_socket,
                             boost::asio::buffer(m_sendQueue.front()),
                             [this, self](const boost::system::error_code &ec, size_t bytes)
                             {
                                 HandleSend(ec, bytes);
                             });
}

void TcpASyncClient::StartReceive()
{
    auto self = shared_from_this();
    m_socket->async_receive(
        boost::asio::buffer(m_recvBuffer),
        [this, self](const boost::system::error_code &ec, std::size_t size)
        {
            HandleReceive(ec, size);
        });
}

void TcpASyncClient::HandleReceive(const boost::system::error_code &error, std::size_t size)
{
    if (!error)
    {
        // 只要收到任何数据（包括心跳包），就刷新时间戳
        m_lastReceiveTimeMs.store(GetCurrentTimeMs());

        std::string data(m_recvBuffer.data(), size);
        // spdlog::info("Async TCP received: {}", data);

        // 发射信号（和Qt信号槽一样用法）
        DataReceived(data);

        // 继续接收
        StartReceive();
    }
    else
    {
        spdlog::error("Receive error: {}", error.message());
        CloseConnect();
    }
}

void TcpASyncClient::HandleSend(const boost::system::error_code &ec, size_t bytes)
{
    (void)bytes;
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
