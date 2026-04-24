#include "main_board.h"
#include "HeadBoard/head_board.h"
#include "BoardCommunicate/board_communicate.h"
#include "spdlog/spdlog.h"

MainBoard::MainBoard(NetChannel netChannel, const char *ip, int port) : m_netChannel(netChannel),
                                                              m_ip(ip),
                                                              m_port(port)
{
    m_ioContext = std::make_shared<boost::asio::io_context>();
    // 使用 work_guard 保证 io_context 在没有任务时也不会退出 run()
    m_workGuard = std::make_unique<io_worker_guard>(m_ioContext->get_executor());

    // 在主板初始化时就开启通信线程
    m_commThread = std::thread([this]()
                               {
                                    spdlog::info("MainBoard I/O thread started.");
                                    m_ioContext->run(); });
}

MainBoard::~MainBoard()
{
    if (m_boardCommunicate)
    {
        m_boardCommunicate->CloseConnect();
        m_boardCommunicate = nullptr;
    }
    m_workGuard.reset();
    if (m_commThread.joinable())
    {
        m_commThread.join();
    }
}

bool MainBoard::OpenConnect()
{
    spdlog::info("{0:s} Connect to Board. IP: {2:s}, port: {3:d}", __FUNCTION__, m_ip, m_port);
    // 关闭之前的连接
    if (m_boardCommunicate)
    {
        m_boardCommunicate->CloseConnect();
        m_boardCommunicate = nullptr;
    }
    // 初始化客户端，传入主板的 io_context
    m_boardCommunicate = BoardCommunicate::Create(m_netChannel, m_ioContext);
    // 绑定接收函数
    m_boardCommunicate->DataReceived.connect(std::bind(&MainBoard::DataReceivedFromBoard, this, std::placeholders::_1));

    if(!m_boardCommunicate->OpenConnect(m_ip, m_port))
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
    if (!m_boardCommunicate)
    {
        return false;
    }
    return m_boardCommunicate->IsConnected();
}

MainBoardConfig MainBoard::GetCurrentConfig() const
{
    MainBoardConfig cfg;
    cfg.ip = m_ip;
    cfg.port = m_port;
    cfg.channel = m_netChannel;
    for (const auto &hb : m_headBoards)
    {
        auto hbPtr = dynamic_cast<HeadBoard *>(hb.get());
        if (!hbPtr)
        {
            continue;
        }
        HeadBoardConfig hbCfg = hbPtr->GetCurrentConfig();
        cfg.headBoards.push_back(hbCfg);
    }
    return cfg;
}

bool MainBoard::SendCommand(const std::string &cmd, const std::string &data)
{
    return true;
}

void MainBoard::SetMainBoardEventCallback(SystemEvent cb, void *pUserData)
{
    m_eventCB = cb;
    m_pUserData = pUserData;
}

const char *MainBoard::GetMainBoardSerial() const
{
    return nullptr;
}

int MainBoard::GetHeadboardCount() const
{
    std::lock_guard<std::mutex> lock(m_mbMutex);
    return (int)m_headBoards.size();
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

