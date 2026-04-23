#include "qs256_head.h"
#include <cstring>

QS256_head::QS256_head(const char* config, ICommandSender* sender)
    : PrintHead(config, sender)
{

}

bool QS256_head::SetVoltageCurve(int curveId)
{
    return false;
}

void QS256_head::EnableInternalHeating(bool enable)
{
}

void* QS256_head::QueryExtension(const char* extensionId){
    // 如果客户请求的是QS256接口，将自身指针转换为对应类型返回
    if (std::strcmp(extensionId, IID_QS256) == 0) {
        return static_cast<IQS256Extension*>(this);
    }
    return nullptr; // 不支持的接口请求返回空
}
