#ifndef MAINBOARD_H
#define MAINBOARD_H

#include "PrintSystemSDK.h"
#include "icommandsender.h"
#include <memory>
#include <vector>
#include <mutex>

class MainBoard : public IMainboard, public ICommandSender
{
    std::string m_ip;
    std::string m_serial;
    std::vector<std::unique_ptr<IHeadboard>> m_headboards;

    // 网络状态
    int m_tcpPort = 0;
    bool m_isTcpConnected = false;
    // int m_tcpSocketFd = -1;

    int m_udpPort = 0;
    bool m_isUdpConnected = false;
    // int m_udpSocketFd = -1;

    mutable std::mutex m_mbMutex;

public:
    MainBoard();

    bool ConnectTCP(int port) override;
    void DisconnectTCP() override;
    bool IsTCPConnected() const override;
    bool SendCommand(NetChannel channel = NetChannel::Auto) override;
};

#endif // MAINBOARD_H
