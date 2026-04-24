#ifndef ICOMMANDSENDER_H
#define ICOMMANDSENDER_H

#include <string>

class ICommandSender
{
public:
    virtual ~ICommandSender() = default;
    // 发送底层命令：命令字、数据负载、长度
    virtual bool SendCommand(const std::string &cmd, const std::string &data) = 0;
};

#endif // ICOMMANDSENDER_H
