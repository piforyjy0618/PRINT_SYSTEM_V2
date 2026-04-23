#include "printhead.h"

PrintHead::PrintHead(const char* config, ICommandSender* sender)
    : m_config(config ? config : ""), m_sender(sender)
{

}
