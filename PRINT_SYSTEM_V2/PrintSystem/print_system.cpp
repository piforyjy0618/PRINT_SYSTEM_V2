#include "print_system.h"
#include "MainBoard/main_board.h"

PrintSystem::PrintSystem()
{
}

PrintSystem::~PrintSystem()
{

}

bool PrintSystem::Initialize(const char *configPath)
{
    return true;
}

IMainBoard *PrintSystem::AddMainBoard(NetChannel netChannel, const char *ip)
{
    m_mainBoards.push_back(std::make_unique<MainBoard>(netChannel, ip));
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
