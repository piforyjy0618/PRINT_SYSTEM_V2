#include "main_board.h"
#include "HeadBoard/head_board.h"
#include "BoardCommunicate/board_communicate.h"
#include "spdlog/spdlog.h"

MainBoard::MainBoard(const char *ip) : m_ip(ip)
{
}

MainBoard::~MainBoard()
{
}

bool MainBoard::OpenConnect(NetChannel channel, int port)
{
    std::string channelStr;
    switch (channel)
    {
        case NetChannel::UDP:
            channelStr = "UDP";
            break;
        case NetChannel::TCP:
            channelStr = "TCP";
            break;
        default:
            spdlog::warn("Unsupported channel type.");
            return false;
    }
    spdlog::info("{0:s} Connect to {1:s}. IP: {2:s}, port: {3:d}", __FUNCTION__, channelStr, m_ip, port);
    // 关闭之前的连接
    if (m_boardCommunicate)
    {
        m_boardCommunicate->CloseConnect();
        m_boardCommunicate = nullptr;
    }
    // 创建并绑定接收函数
    m_boardCommunicate = BoardCommunicate::Create(channel);
    m_boardCommunicate->DataReceived.connect(std::bind(&MainBoard::DataReceivedFromBoard, this, std::placeholders::_1));
    if(!m_boardCommunicate->OpenConnect(m_ip, port))
    {
        spdlog::warn("{0:s}: Failed to Open Connect.", __FUNCTION__);
        return false;
    }
    return true;
}

void MainBoard::CloseConnect()
{
    if (m_boardCommunicate)
    {
        m_boardCommunicate->CloseConnect();
        m_boardCommunicate = nullptr;
    }
    spdlog::info("{0:s}", __FUNCTION__);
}

bool MainBoard::IsConnected() const
{
    return m_boardCommunicate->IsConnected();
}

bool MainBoard::SendCommand()
{
    return true;
}

bool MainBoard::SendData()
{
    return false;
}

void MainBoard::SetMainBoardEventCallback(SystemEvent cb, void *pUserData)
{
    m_eventCB = cb;
    m_pUserData = pUserData;
}

const char *MainBoard::GetIPAddress() const
{
    return nullptr;
}

const char *MainBoard::GetMainBoardSerial() const
{
    return nullptr;
}

// --- 设备层级实现 ---
int MainBoard::GetHeadboardCount() const
{
    std::lock_guard<std::mutex> lock(m_mbMutex);
    return m_headBoards.size();
}

IHeadboard *MainBoard::GetHeadboard(int index) const
{
    std::lock_guard<std::mutex> lock(m_mbMutex);
    if (index >= 0 && index < static_cast<int>(m_headBoards.size()))
    {
        return m_headBoards[index].get();
    }
    return nullptr;
}

// 只要修改主板内部参数都要加锁
IHeadboard *MainBoard::AddHeadboard(const char *serial)
{
    std::lock_guard<std::mutex> lock(m_mbMutex);
    m_headBoards.push_back(std::make_unique<HeadBoard>(serial, this));
    return m_headBoards.back().get();
}

void MainBoard::DataReceivedFromBoard(std::string recvData)
{
}

// --- 内部通信实现 ---
bool MainBoard::SendHardwareCommand(uint16_t cmdId, const uint8_t *payload, int length, NetChannel channel)
{
    std::lock_guard<std::mutex> lock(m_mbMutex);
    return false;
}
