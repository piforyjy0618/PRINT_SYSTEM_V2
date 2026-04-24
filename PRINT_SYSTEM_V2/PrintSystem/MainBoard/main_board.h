#ifndef MAINBOARD_H
#define MAINBOARD_H

#include "PrintSystemSDK.h"
#include "i_command_sender.h"
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <mutex>

class BoardCommunicate;

typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_worker_guard;

class MainBoard : public IMainBoard, public ICommandSender
{
    std::string m_serial;
    std::vector<std::unique_ptr<IHeadboard>> m_headBoards;
    mutable std::mutex m_mbMutex;

    /** 网络连接相关 */
    // 接收主板数据回调函数
    void DataReceivedFromBoard(std::string recvData);
    NetChannel m_netChannel;                              // 通信方式
    std::shared_ptr<boost::asio::io_context> m_ioContext; // 上下文
    std::unique_ptr<io_worker_guard> m_workGuard;         // 守卫
    std::thread m_commThread;                             // 通信线程
    std::string m_ip;                                     // 主板IP地址
    std::shared_ptr<BoardCommunicate> m_boardCommunicate; // 通信类指针

    /** 回调相关 */
    SystemEvent m_eventCB = nullptr;
    void *m_pUserData = nullptr;
    
public:
    MainBoard(NetChannel netChannel, const char *ip);
    virtual ~MainBoard();

    /**------连接------ */
    // 打开主板连接
    bool OpenConnect(int port) override;
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

    bool SendCommand(const std::string &cmd, const std::string &data) override;

    // 主板事件回调接口
    void SetMainBoardEventCallback(SystemEvent cb, void *pUserData = nullptr) override;
};

#endif // MAINBOARD_H
