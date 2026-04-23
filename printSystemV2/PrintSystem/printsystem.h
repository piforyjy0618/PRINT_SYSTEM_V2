#ifndef PRINTSYSTEM_H
#define PRINTSYSTEM_H

#include "PrintSystemSDK.h"
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

class PrintSystem : IPrintSystem
{
    std::vector<std::unique_ptr<IMainboard>> m_mainboards;
    // 多线程相关组件
    mutable std::mutex m_mutex;        // 保护 m_mainboards 及其他共享状态的互斥锁
    std::thread m_networkThread;       // 后台网络监听线程
    std::atomic<bool> m_isRunning;     // 控制线程退出的标志位

    // 后台网络线程的执行函数
    void NetworkWorkerThread();
public:
    PrintSystem();
    ~PrintSystem();

    IMainboard* AddMainboard(const char* ip) override;
    bool RemoveMainboard(int index) override;
    int GetMainboardCount() const override;
    IMainboard* GetMainboard(int index) const override;

    bool Initialize(const char* configPath) override;
    void Release() override;
};

#endif // PRINTSYSTEM_H
