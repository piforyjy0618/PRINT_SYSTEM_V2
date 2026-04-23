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
    // 多线程相关组件
    mutable std::mutex m_mutex;        // 保护 m_mainBoards 及其他共享状态的互斥锁
    std::thread m_networkThread;       // 后台网络监听线程
    std::atomic<bool> m_isRunning;     // 控制线程退出的标志位

    /** 回调相关 */
    SystemEvent m_eventCB = nullptr;
    void *m_pUserData = nullptr;

    // 后台网络线程的执行函数
    void NetworkWorkerThread();
public:
    PrintSystem();
    virtual ~PrintSystem();

    // 初始化系统，传入配置文件路径
    bool Initialize(const char *configPath = nullptr) override;
    // 添加主板到系统
    IMainBoard *AddMainBoard(const char *ip) override;
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
