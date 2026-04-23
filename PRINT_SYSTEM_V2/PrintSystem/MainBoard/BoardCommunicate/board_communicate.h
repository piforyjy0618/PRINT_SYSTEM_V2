#pragma once
#include "PrintSystemSDK.h"
#include <boost/signals2.hpp>

class BoardCommunicate
{
public:
    // 创建通信对象
    static std::shared_ptr<BoardCommunicate> Create(IMainBoard::NetChannel channel, int localPort = 0);

    // 打开连接
    virtual bool OpenConnect(const std::string &host, const int port) = 0;
    // 关闭连接
    virtual void CloseConnect() = 0;
    // 连接状态
    virtual bool IsConnected() const = 0;
    // 发送数据
    virtual bool Send(const std::string &message) = 0;

    // 数据接收信号
    boost::signals2::signal<void(std::string)> DataReceived;

    virtual ~BoardCommunicate() = default;

protected:
    // 获取当前系统的毫秒级时间戳
    int64_t GetCurrentTimeMs() const;
};
