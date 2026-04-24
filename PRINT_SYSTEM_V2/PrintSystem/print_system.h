#ifndef PRINTSYSTEM_H
#define PRINTSYSTEM_H

#include "PrintSystemSDK.h"
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

class PrintSystem : public IPrintSystem
{
    std::vector<std::unique_ptr<IMainBoard>> m_mainBoards;

    /** 回调相关 */
    SystemEvent m_eventCB = nullptr;
    void *m_pUserData = nullptr;

public:
    PrintSystem();
    virtual ~PrintSystem();

    // 初始化系统，传入配置文件路径
    bool Initialize(const char *configPath = nullptr) override;
    // 保存当前配置
    bool SaveConfig(const char *configPath) override;
    // 添加主板到系统
    IMainBoard *AddMainBoard(NetChannel netChannel, const char *ip, int port) override;
    // 移除主板
    bool RemoveMainBoard(int index) override;
    // 获取主板数量
    int GetMainBoardCount() const override;
    // 获取主板指针
    IMainBoard *GetMainBoard(int index) const override;
    // 系统释放
    void Release() override;
    // 系统事件回调接口
    void SetSystemEventCallback(SystemEvent cb, void *pUserData = nullptr) override;
};

#endif // PRINTSYSTEM_H
