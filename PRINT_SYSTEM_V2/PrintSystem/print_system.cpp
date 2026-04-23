#include "print_system.h"
#include "MainBoard/main_board.h"

PrintSystem::PrintSystem() : m_isRunning(false)
{
}

PrintSystem::~PrintSystem()
{
    // 析构时必须安全地停止后台线程
    m_isRunning = false;
    if (m_networkThread.joinable())
    {
        m_networkThread.join();
    }
}

bool PrintSystem::Initialize(const char *configPath)
{
    // 1. 初始化通信层 (如打开 USB)
    // m_usbHandle = OpenUSBDevice(...);

    // 2. 扫描并创建头板，将 this（此时身份是 ICommandSender*）传给头板
    m_mainBoards.push_back(std::make_unique<MainBoard>("PCC-001"));

    if (m_isRunning)
        return true;

    // 启动后台网络监听线程
    m_isRunning = true;
    m_networkThread = std::thread(&PrintSystem::NetworkWorkerThread, this);
    return true;
}

void PrintSystem::NetworkWorkerThread()
{
    while (m_isRunning)
    {
        // 1. 使用 select / epoll / IOCP 等监听所有 Socket 的数据到来
        // 2. 解析接收到的底层 TCP/UDP 数据包
        // 3. 如果收到数据包：

        /*
        std::lock_guard<std::mutex> lock(m_mutex);
        // 处理接收到的数据，例如更新某块主板的状态，或触发回调通知 UI
        */

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 避免CPU空转 (模拟)
    }
}

IMainBoard *PrintSystem::AddMainBoard(const char *ip)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_mainBoards.push_back(std::make_unique<MainBoard>(ip));
    return m_mainBoards.back().get();
}

bool PrintSystem::RemoveMainBoard(int index)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index >= 0 && index < m_mainBoards.size())
    {
        m_mainBoards.erase(m_mainBoards.begin() + index);
        return true;
    }
    return false;
}

int PrintSystem::GetMainBoardCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return (int)m_mainBoards.size();
}

IMainBoard *PrintSystem::GetMainBoard(int index) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index >= 0 && index < m_mainBoards.size())
    {
        return m_mainBoards[index].get();
    }
    return nullptr;
}

void PrintSystem::Release()
{
    delete this; // 自我销毁
}

void PrintSystem::SetSystemEventCallback(SystemEvent cb, void *pUserData)
{
    m_eventCB = cb;
    m_pUserData = pUserData;
}

extern "C" SDK_API IPrintSystem* CreatePrintSystem() 
{
    return new (std::nothrow) PrintSystem();
}
