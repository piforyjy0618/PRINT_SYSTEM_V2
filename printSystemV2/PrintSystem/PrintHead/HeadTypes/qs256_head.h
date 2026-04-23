#ifndef QS256_HEAD_H
#define QS256_HEAD_H

#include "PrintSystemSDK.h"
#include "PrintHead/printhead.h"
#include "MainBoard/icommandsender.h"

class QS256_head : public PrintHead, public IQS256Extension
{
public:
    QS256_head(const char* config, ICommandSender* sender) ;

    void* QueryExtension(const char* extensionId) override;
};

#endif // QS256_HEAD_H
