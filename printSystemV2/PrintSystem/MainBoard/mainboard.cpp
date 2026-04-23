#include "mainboard.h"
#include "HeadBoard/headboard.h"

MainBoard::MainBoard()
{

}

bool MainBoard::SendCommand(NetChannel channel)
{
    return true;
}

bool MainBoard::ConnectTCP(int port)
{
    std::lock_guard<std::mutex> lock(m_mbMutex);
    if (m_isTcpConnected) return true;
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

bool ConnectUDP(int port) override {
    std::lock_guard<std::mutex> lock(m_mbMutex);
    if (m_isUdpConnected) return true;
    // 伪代码：绑定 UDP 端口
    m_udpPort = port;
    m_isUdpConnected = true;
    std::cout << "[Mainboard " << m_ip << "] UDP Ready on port " << port << "\n";
    return true;
}

void DisconnectUDP() override {
    std::lock_guard<std::mutex> lock(m_mbMutex);
    m_isUdpConnected = false;
}

bool IsUDPConnected() const override { return m_isUdpConnected; }

// --- 设备层级实现 ---
int GetHeadboardCount() const override {
    std::lock_guard<std::mutex> lock(m_mbMutex);
    return static_cast<int>(m_headboards.size());
}

IHeadboard* GetHeadboard(int index) const override {
    std::lock_guard<std::mutex> lock(m_mbMutex);
    if (index >= 0 && index < static_cast<int>(m_headboards.size())) {
        return m_headboards[index].get();
    }
    return nullptr;
}

//只要修改主板内部参数都要加锁
IHeadboard* AddHeadboard(const char* serial) override {
    std::lock_guard<std::mutex> lock(m_mbMutex);
    m_headboards.push_back(std::make_unique<HeadboardImpl>(serial, this));
    return m_headboards.back().get();
}

// --- 内部通信实现 ---
bool SendHardwareCommand(uint16_t cmdId, const uint8_t* payload, int length, NetChannel channel) override {
    std::lock_guard<std::mutex> lock(m_mbMutex);

    bool useTCP = false;
    if (channel == NetChannel::TCP && m_isTcpConnected) useTCP = true;
    else if (channel == NetChannel::UDP && m_isUdpConnected) useTCP = false;
    else if (channel == NetChannel::Auto) {
        // 自动模式：TCP优先，否则UDP
        if (m_isTcpConnected) useTCP = true;
        else if (m_isUdpConnected) useTCP = false;
        else return false; // 都没有连接
    } else {
        return false; // 请求的通道未连接
    }

    std::cout << "[Network Thread] Sending via " << (useTCP ? "TCP" : "UDP")
              << " to " << m_ip << " | CMD: 0x" << std::hex << cmdId << std::endl;

    // if (useTCP) send(m_tcpSocketFd, ...); else sendto(m_udpSocketFd, ...);
    return true;
}
};



