#ifndef ICOMMANDSENDER_H
#define ICOMMANDSENDER_H

class ICommandSender
{
public:
    virtual ~ICommandSender() = default;
    // 发送底层命令：命令字、数据负载、长度
    virtual bool SendCommand() = 0;
    virtual bool SendData() = 0;
};

#endif // ICOMMANDSENDER_H
