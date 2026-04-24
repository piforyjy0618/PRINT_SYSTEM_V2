#include "print_system.h"
#include "MainBoard/main_board.h"
#include "ConfigManager/config_manager.h"
#include "spdlog/spdlog.h"

PrintSystem::PrintSystem()
{
}

PrintSystem::~PrintSystem()
{

}

bool PrintSystem::Initialize(const char *configPath)
{
    if (configPath == nullptr)
    {
        spdlog::error("Config path is null");
        return false;
    }

    // 1. 加载配置
    PrintSystemConfig cfg;
    if (!ConfigManager::LoadConfig(configPath, cfg))
    {
        return false; // 如果 XML 错误，直接初始化失败
    }

    // 自动根据 XML 创建并连接所有主板
    for (const auto &mbCfg : cfg.mainBoards)
    {
        IMainBoard *board = AddMainBoard(mbCfg.channel, mbCfg.ip.c_str(), mbCfg.port);
        if (board)
        {
            board->OpenConnect();

            // 自动挂载头板
            for (const auto &hbCfg : mbCfg.headBoards)
            {
                board->AddHeadboard(hbCfg.serial.c_str());
            }
        }
    }
    return true;
}

bool PrintSystem::SaveConfig(const char *configPath)
{
    PrintSystemConfig cfg;
    // 从主板中提取配置信息
    for (const auto &mb : m_mainBoards)
    {
        auto mbPtr = dynamic_cast<MainBoard *>(mb.get());
        if (!mbPtr)
        {
            continue;
        }
        MainBoardConfig mbCfg = mbPtr->GetCurrentConfig();
        cfg.mainBoards.push_back(mbCfg);
    }
    // 保存配置到文件
    return ConfigManager::SaveConfig(configPath, cfg);
}

IMainBoard *PrintSystem::AddMainBoard(NetChannel netChannel, const char *ip, int port)
{
    m_mainBoards.push_back(std::make_unique<MainBoard>(netChannel, ip, port));
    return m_mainBoards.back().get();
}

bool PrintSystem::RemoveMainBoard(int index)
{
    if (index >= 0 && index < m_mainBoards.size())
    {
        m_mainBoards.erase(m_mainBoards.begin() + index);
        return true;
    }
    return false;
}

int PrintSystem::GetMainBoardCount() const
{
    return (int)m_mainBoards.size();
}

IMainBoard *PrintSystem::GetMainBoard(int index) const
{
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
