#pragma once
#include "../board_communicate.h"
#include <boost/asio.hpp>
#include <thread>

using namespace boost::asio;
using ip::udp;
typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_worker_guard;

class UdpASyncClient : public BoardCommunicate
{
public:
    UdpASyncClient(const int localPort = 0);
    virtual ~UdpASyncClient();

    // 打开连接
    bool OpenConnect(const std::string &host, const int port) override;
    // 关闭连接
    void CloseConnect() override;
    // 连接状态
    bool IsConnected() const override;
    // 发送数据
    bool Send(const std::string &message) override;

private:
    // 开始异步接收数据
    void StartReceive();
    // 处理接收到的数据
    void HandleReceive(const boost::system::error_code &error, std::size_t size);

    std::unique_ptr<io_context> m_ioContext;
    std::thread m_ioThread;
    std::unique_ptr<udp::socket> m_socket;
    std::unique_ptr<udp::endpoint> m_serverEndpoint;
    std::array<char, 1024> m_recvBuffer;
    std::unique_ptr<io_worker_guard> m_workGuard;

    std::atomic<int64_t> m_lastReceiveTimeMs{0}; // 记录最后一次收到心跳/数据的时间
    const int64_t HEARTBEAT_TIMEOUT_MS = 1500;   // 设置心跳超时阈值
};
