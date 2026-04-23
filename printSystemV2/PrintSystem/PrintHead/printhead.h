#ifndef PRINTHEAD_H
#define PRINTHEAD_H

#include "PrintSystemSDK.h"
#include "MainBoard/icommandsender.h"
#include <string>

class PrintHead : public IPrinthead
{
    std::string m_serial;

public:
    PrintHead(const char* config, ICommandSender* sender);

protected:
    std::string m_config;
    ICommandSender* m_sender; // 核心：基类统一持有通信接口
};

#endif // PRINTHEAD_H
