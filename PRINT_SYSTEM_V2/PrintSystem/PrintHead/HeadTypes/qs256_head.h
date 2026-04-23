#ifndef QS256_HEAD_H
#define QS256_HEAD_H

#include "PrintSystemSDK.h"
#include "PrintHead/print_head.h"
#include "MainBoard/i_command_sender.h"

class QS256_head : public PrintHead, public IQS256Extension
{
public:
    QS256_head(const char* config, ICommandSender* sender);

    bool SetVoltageCurve(int curveId) override;
    void EnableInternalHeating(bool enable) override;

    void* QueryExtension(const char* extensionId) override;
};

#endif // QS256_HEAD_H
