#ifndef MAINBOARD_H
#define MAINBOARD_H

#include "PrintSystemSDK.h"
#include "i_command_sender.h"
#include <memory>
#include <vector>
#include <mutex>

class BoardCommunicate;

class MainBoard : public IMainBoard, public ICommandSender
{
    std::string m_serial;
    std::vector<std::unique_ptr<IHeadboard>> m_headBoards;
    mutable std::mutex m_mbMutex;

    /** 网络连接相关 */
    // 接收主板数据回调函数
    void DataReceivedFromBoard(std::string recvData);
    std::string m_ip;                                     // 主板IP地址
    std::shared_ptr<BoardCommunicate> m_boardCommunicate; // 通信类指针

    /** 回调相关 */
    SystemEvent m_eventCB = nullptr;
    void *m_pUserData = nullptr;

    bool SendHardwareCommand(uint16_t cmdId, const uint8_t *payload, int length, NetChannel channel);
    
public:
    MainBoard(const char *ip);
    virtual ~MainBoard();

    /**------连接------ */
    // 打开主板连接
    bool OpenConnect(NetChannel channel, int port) override;
    // 断开主板连接
    void CloseConnect() override;
    // 获取连接状态
    bool IsConnected() const override;
    // 获取主板IP地址
    const char *GetIPAddress() const override;

    // 获取主板序列号
    const char *GetMainBoardSerial() const override;
    // 获取头板数量
    int GetHeadboardCount() const override;
    // 获取头板指针
    IHeadboard *GetHeadboard(int index) const override;
    // 添加头板
    IHeadboard *AddHeadboard(const char *serial) override;

    bool SendCommand() override;
    bool SendData() override;

    // 主板事件回调接口
    void SetMainBoardEventCallback(SystemEvent cb, void *pUserData = nullptr) override;
};

#endif // MAINBOARD_H
