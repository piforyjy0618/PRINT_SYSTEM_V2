#include "udp_async_client.h"
#include "spdlog/spdlog.h"

UdpASyncClient::UdpASyncClient(const int localPort) : m_ioContext(new io_context),
                                                      m_socket(new udp::socket(*m_ioContext, udp::endpoint(udp::v4(), localPort)))
{
}

UdpASyncClient::~UdpASyncClient()
{
    // 确保对象销毁时，线程能安全退出
    this->CloseConnect();
}

bool UdpASyncClient::OpenConnect(const std::string &host, const int port)
{
    try
    {
        udp::resolver resolver(*m_ioContext);
        auto endpoints = resolver.resolve(udp::v4(), host, std::to_string(port));
        m_serverEndpoint = std::unique_ptr<udp::endpoint>(new udp::endpoint(*endpoints.begin()));
        // 防止 io_context.run() 在没有异步操作时立即返回
        m_workGuard = std::make_unique<io_worker_guard>(m_ioContext->get_executor());
        // 如果 io_context 之前停止过，重启它
        if (m_ioContext->stopped())
        {
            m_ioContext->restart();
        }
        // 初始化最后收到数据的时间
        m_lastReceiveTimeMs.store(GetCurrentTimeMs());
        this->StartReceive();
        m_ioThread = std::thread([this]()
                                 { 
            m_ioContext->run(); 
            spdlog::info("UDP IO Thread exited."); });
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
    // 1. 停止 socket
    if (m_socket && m_socket->is_open())
    {
        boost::system::error_code ec;
        m_socket->close(ec);
    }
    // 2. 释放 Work Guard（允许 run() 结束）
    if (m_workGuard)
    {
        m_workGuard.reset();
    }
    // 3. 停止并回收
    if (m_ioContext)
    {
        m_ioContext->stop();
    }
    if (m_ioThread.joinable())
    {
        m_ioThread.join();
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

    auto data = std::make_shared<std::string>(message);

    // --- 核心改动 2: 使用 post 投递任务 ---
    // 确保 async_send_to 始终在 m_ioThread 中被调用，彻底避免 Socket 竞争
    boost::asio::post(*m_ioContext, [this, data]()
                      { m_socket->async_send_to(
                            boost::asio::buffer(*data),
                            *m_serverEndpoint,
                            [data](const boost::system::error_code &ec, std::size_t /*bytesSent*/)
                            {
                                if (ec)
                                {
                                    spdlog::error("UDP send error: {}", ec.message());
                                }
                            }); });

    return true;
}

void UdpASyncClient::StartReceive()
{
    m_socket->async_receive_from(boost::asio::buffer(m_recvBuffer),
                                 *m_serverEndpoint,
                                 std::bind(&UdpASyncClient::HandleReceive,
                                           this,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
}

void UdpASyncClient::HandleReceive(const boost::system::error_code &error, std::size_t size)
{
    if (!error)
    {
        // 只要收到任何数据（包括心跳包），就刷新时间戳
        m_lastReceiveTimeMs.store(GetCurrentTimeMs());
        // 处理接收到的数据
        std::string recvString(m_recvBuffer.data(), size);
        // spdlog::info("UdpASyncClient receive: {}", recvString);
        //  发送数据接收信号
        DataReceived(recvString);
        // 继续接收数据
        this->StartReceive();
    }
}
