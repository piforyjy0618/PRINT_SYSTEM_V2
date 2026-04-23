#ifndef MAINBOARD_H
#define MAINBOARD_H

#include "PrintSystemSDK.h"
#include "i_command_sender.h"
#include <memory>
#include <vector>
#include <mutex>

class MainBoard : public IMainBoard, public ICommandSender
{
    std::string m_ip;
    std::string m_serial;
    std::vector<std::unique_ptr<IHeadboard>> m_headBoards;

    // 网络状态
    int m_tcpPort = 0;
    bool m_isTcpConnected = false;
    // int m_tcpSocketFd = -1;

    int m_udpPort = 0;
    bool m_isUdpConnected = false;
    // int m_udpSocketFd = -1;

    mutable std::mutex m_mbMutex;

    bool SendHardwareCommand(uint16_t cmdId, const uint8_t *payload, int length, NetChannel channel);
public:
    MainBoard(const char *ip);
    virtual ~MainBoard();

    /** TCP 连接 */
    // 连接TCP端口，返回是否成功
    bool ConnectTCP(int port) override;
    // 断开TCP连接
    void DisconnectTCP() override;
    // 判断TCP是否已连接
    bool IsTCPConnected() const override;

    /** UDP 连接*/
    // 连接UDP端口，返回是否成功
    bool ConnectUDP(int port) override;
    // 断开UDP连接
    void DisconnectUDP() override;
    // 判断UDP是否已连接
    bool IsUDPConnected() const override;

    // 获取主板IP地址
    const char *GetIPAddress() const override;
    // 获取主板序列号
    const char *GetMainBoardSerial() const override;
    // 获取头板数量
    size_t GetHeadboardCount() const override;
    // 获取头板指针
    IHeadboard *GetHeadboard(int index) const override;
    // 添加头板
    IHeadboard *AddHeadboard(const char *serial) override;

    bool SendCommand(NetChannel channel = NetChannel::Auto) override;
    bool SendData(NetChannel channel = NetChannel::Auto) override;
};

#endif // MAINBOARD_H
