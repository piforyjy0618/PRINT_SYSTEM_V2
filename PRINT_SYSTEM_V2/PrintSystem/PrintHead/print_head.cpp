#include "print_head.h"

PrintHead::PrintHead(const char* config, ICommandSender* sender)
    : m_config(config ? config : ""), m_sender(sender)
{

}

const char *PrintHead::GetPrintheadSerial() const
{
    return nullptr;
}

PrintheadModel PrintHead::GetModel() const
{
    return PrintheadModel();
}

float PrintHead::GetTemperature() const
{
    return 0.0f;
}

bool PrintHead::Fire()
{
    return false;
}
