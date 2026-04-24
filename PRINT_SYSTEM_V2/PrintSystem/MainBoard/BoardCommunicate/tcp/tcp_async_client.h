#pragma once
#include "../board_communicate.h"
#include <boost/asio.hpp>
#include <thread>
#include <queue>

using namespace boost::asio;
using ip::tcp;


class TcpASyncClient : public BoardCommunicate, public std::enable_shared_from_this<TcpASyncClient>
{
public:
    TcpASyncClient(std::shared_ptr<io_context> ioContext, const int localPort = 0);
    virtual ~TcpASyncClient();

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
    // 异步发送数据
    void DoAsyncWrite();
    // 处理接收到的数据
    void HandleReceive(const boost::system::error_code &error, std::size_t size);
    // 异步发送完成回调
    void HandleSend(const boost::system::error_code &ec, size_t bytes);

    int m_localPort;
    std::shared_ptr<io_context> m_ioContext;
    std::unique_ptr<tcp::socket> m_socket;
    std::unique_ptr<tcp::endpoint> m_serverEndpoint;
    std::array<char, 1024> m_recvBuffer;
    std::queue<std::vector<uint8_t>> m_sendQueue;
    bool m_isSending = false;

    std::atomic<int64_t> m_lastReceiveTimeMs{0}; // 记录最后一次收到心跳/数据的时间
    const int64_t HEARTBEAT_TIMEOUT_MS = 1500;   // 设置心跳超时阈值
};
