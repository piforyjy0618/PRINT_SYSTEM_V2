#include "tcp_async_client.h"
#include "spdlog/spdlog.h"

TcpASyncClient::TcpASyncClient(const int localPort) : m_localPort(localPort),
                                                      m_ioContext(new io_context),
                                                      m_socket(new tcp::socket(*m_ioContext))
{
}

TcpASyncClient::~TcpASyncClient()
{
    this->CloseConnect();
}

bool TcpASyncClient::OpenConnect(const std::string &host, const int port)
{
    try
    {
        if (m_socket->is_open())
        {
            spdlog::warn("Socket already connected");
            return true;
        }
        tcp::resolver resolver(*m_ioContext);
        auto results = resolver.resolve(host, std::to_string(port));
        if (m_localPort != 0)
        {
            boost::system::error_code ec;
            m_socket->close(ec);
            m_socket->open(tcp::v4());
            m_socket->bind(tcp::endpoint(tcp::v4(), m_localPort));
        }
        boost::asio::connect(*m_socket, results);
        spdlog::info("Async TCP connected: {}:{}", host, port);
        // 连接成功时，初始化最后收到数据的时间
        m_lastReceiveTimeMs.store(GetCurrentTimeMs());

        m_workGuard = std::make_unique<io_worker_guard>(m_ioContext->get_executor());
        // 启动异步接收
        StartReceive();
        // 启动 io_context 线程
        m_ioThread = std::thread([this]()
                                 { m_ioContext->run(); });

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
    try
    {
        if (m_socket && m_socket->is_open())
        {
            boost::system::error_code ec;
            m_socket->shutdown(tcp::socket::shutdown_both, ec);
            m_socket->close(ec);
        }
        if (m_workGuard)
        {
            m_workGuard.reset();
        }
        if (m_ioContext)
        {
            m_ioContext->stop();
        }
        if (m_ioThread.joinable())
        {
            m_ioThread.join();
        }
        spdlog::info("Async TCP closed");
    }
    catch (...)
    {
        spdlog::error("CloseConnect exception");
    }
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

    auto data = std::make_shared<std::string>(message);

    // 使用 post 确保 async_send 在 IO 线程中调用
    // 无论在 UI 线程还是定时器线程调用 Send，真正的 Socket 操作都会排队执行
    boost::asio::post(*m_ioContext, [this, data]()
                      { m_socket->async_send(
                            boost::asio::buffer(*data),
                            std::bind(&TcpASyncClient::HandleSend, this,
                                      std::placeholders::_1,
                                      std::placeholders::_2,
                                      data)); });

    return true;
}

void TcpASyncClient::StartReceive()
{
    m_socket->async_receive(
        boost::asio::buffer(m_recvBuffer),
        std::bind(&TcpASyncClient::HandleReceive, this,
                  std::placeholders::_1,
                  std::placeholders::_2));
}

void TcpASyncClient::HandleReceive(const boost::system::error_code &error, std::size_t size)
{
    if (!error)
    {
        // 只要收到任何数据（包括心跳包），就刷新时间戳
        m_lastReceiveTimeMs.store(GetCurrentTimeMs());

        std::string data(m_recvBuffer.data(), size);
        //spdlog::info("Async TCP received: {}", data);

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

void TcpASyncClient::HandleSend(const boost::system::error_code &ec, size_t bytes, std::shared_ptr<std::string> data)
{
    (void)bytes;
    (void)data;
    if (ec)
    {
        spdlog::error("Send error: {}", ec.message());
    }
}
