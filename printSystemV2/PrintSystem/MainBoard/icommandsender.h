#ifndef ICOMMANDSENDER_H
#define ICOMMANDSENDER_H

enum class NetChannel {
    Auto, // 自动选择 (如优先 TCP，无 TCP 则选 UDP)
    TCP,
    UDP
};

class ICommandSender
{
public:
    virtual ~ICommandSender() = default;
    // 发送底层命令：命令字、数据负载、长度
    virtual bool SendCommand(NetChannel channel = NetChannel::Auto) = 0;
    virtual bool SendData(NetChannel channel = NetChannel::Auto) = 0;
};

#endif // ICOMMANDSENDER_H
