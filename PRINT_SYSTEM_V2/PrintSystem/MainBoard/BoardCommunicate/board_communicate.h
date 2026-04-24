#pragma once
#include "PrintSystemSDK.h"
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

using boost::asio::io_context;

#ifdef TEST_ENV
// 在测试环境下，导出为DLL接口
#include "SdkExport.h"
#pragma warning(disable : 4251)
class SDK_API BoardCommunicate
#else
class BoardCommunicate
#endif
{
public:
    // 创建通信对象
    static std::shared_ptr<BoardCommunicate> Create(NetChannel channel, std::shared_ptr<io_context> ioContext, int localPort = 0);

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
