#include "main_board.h"
#include "HeadBoard/head_board.h"

MainBoard::MainBoard(const char *ip)
{
}

MainBoard::~MainBoard()
{
}

bool MainBoard::SendCommand(NetChannel channel)
{
    return true;
}

bool MainBoard::SendData(NetChannel channel)
{
    return false;
}

bool MainBoard::ConnectTCP(int port)
{
    std::lock_guard<std::mutex> lock(m_mbMutex);
    if (m_isTcpConnected)
        return true;
    // 伪代码：发起 TCP 连接
    // m_tcpSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    // connect(...);
    m_tcpPort = port;
    m_isTcpConnected = true;
    //    std::cout << "[Mainboard " << m_ip << "] TCP Connected on port " << port << "\n";
    return true;
}

void MainBoard::DisconnectTCP()
{
    std::lock_guard<std::mutex> lock(m_mbMutex);
    m_isTcpConnected = false;
    // close(m_tcpSocketFd);
    //    std::cout << "[Mainboard " << m_ip << "] TCP Disconnected.\n";
}

bool MainBoard::IsTCPConnected() const
{
    return m_isTcpConnected;
}

bool MainBoard::ConnectUDP(int port)
{
    std::lock_guard<std::mutex> lock(m_mbMutex);
    if (m_isUdpConnected)
        return true;
    // 伪代码：绑定 UDP 端口
    m_udpPort = port;
    m_isUdpConnected = true;
    //std::cout << "[Mainboard " << m_ip << "] UDP Ready on port " << port << "\n";
    return true;
}

void MainBoard::DisconnectUDP()
{
    std::lock_guard<std::mutex> lock(m_mbMutex);
    m_isUdpConnected = false;
}

bool MainBoard::IsUDPConnected() const { return m_isUdpConnected; }

const char *MainBoard::GetIPAddress() const
{
    return nullptr;
}

const char *MainBoard::GetMainBoardSerial() const
{
    return nullptr;
}

// --- 设备层级实现 ---
size_t MainBoard::GetHeadboardCount() const
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

// --- 内部通信实现 ---
bool MainBoard::SendHardwareCommand(uint16_t cmdId, const uint8_t *payload, int length, NetChannel channel)
{
    std::lock_guard<std::mutex> lock(m_mbMutex);

    bool useTCP = false;
    if (channel == NetChannel::TCP && m_isTcpConnected)
        useTCP = true;
    else if (channel == NetChannel::UDP && m_isUdpConnected)
        useTCP = false;
    else if (channel == NetChannel::Auto)
    {
        // 自动模式：TCP优先，否则UDP
        if (m_isTcpConnected)
            useTCP = true;
        else if (m_isUdpConnected)
            useTCP = false;
        else
            return false; // 都没有连接
    }
    else
    {
        return false; // 请求的通道未连接
    }

    // std::cout << "[Network Thread] Sending via " << (useTCP ? "TCP" : "UDP")
    //           << " to " << m_ip << " | CMD: 0x" << std::hex << cmdId << std::endl;

    // if (useTCP) send(m_tcpSocketFd, ...); else sendto(m_udpSocketFd, ...);
    return true;
}
