#ifndef PRINTHEAD_H
#define PRINTHEAD_H

#include "PrintSystemSDK.h"
#include "MainBoard/i_command_sender.h"
#include <string>

class PrintHead : public IPrinthead
{
    std::string m_serial;

public:
    PrintHead(const char* config, ICommandSender* sender);

    // 共性功能
    const char *GetPrintheadSerial() const override;
    PrintheadModel GetModel() const override;
    float GetTemperature() const override;
    bool Fire() override; // 闪喷/打印动作

    // 核心扩展机制：获取特性接口
    void *QueryExtension(const char *extensionId) = 0;

protected:
    std::string m_config;
    ICommandSender* m_sender; // 核心：基类统一持有通信接口
};

#endif // PRINTHEAD_H
