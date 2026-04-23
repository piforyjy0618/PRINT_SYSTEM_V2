#include "head_board.h"
#include "PrintHead/HeadTypes/qs256_head.h"
#include <memory>

HeadBoard::HeadBoard(const std::string &serial, ICommandSender *sender)
    : m_serial(serial), m_sender(sender)
{
}

HeadBoard::~HeadBoard()
{
}

// const 只读不写
IPrinthead *HeadBoard::GetPrinthead(int index) const
{
    if (index >= 0 && index < static_cast<int>(m_heads.size()))
        return m_heads[index].get();
    return nullptr;
}

int HeadBoard::GetPrintheadCount() const
{
    return 0;
}

const char *HeadBoard::GetHeadBoardSerial() const
{
    return m_serial.c_str();
}

IPrinthead *HeadBoard::AddPrinthead(PrintheadModel model, const char *configParams)
{
    std::unique_ptr<IPrinthead> newHead;

    switch (model)
    {
    case PrintheadModel::QS256:
        newHead = std::make_unique<QS256_head>(configParams, m_sender);
        break;
    // case PrintheadModel::EpsonI3200:
    //     newHead = std::make_unique<EpsonPrintheadImpl>(configParams);
    //     break;
    default:
        return nullptr; // 型号不支持
    }

    IPrinthead *rawPtr = newHead.get();
    m_heads.push_back(std::move(newHead)); // 转移所有权给 vector 管理
    return rawPtr;
}

// 【核心新增】：动态移除实现（按索引）
bool HeadBoard::RemovePrinthead(int index)
{
    if (index >= 0 && index < static_cast<int>(m_heads.size()))
    {
        // 从 vector 中 erase，unique_ptr 会自动调用喷头析构函数，释放内存
        m_heads.erase(m_heads.begin() + index);
        return true;
    }
    return false;
}

// 【核心新增】：动态移除实现（按指针）
bool HeadBoard::RemovePrintheadByPtr(IPrinthead *head)
{
    for (auto it = m_heads.begin(); it != m_heads.end(); ++it)
    {
        if (it->get() == head)
        {
            m_heads.erase(it);
            return true;
        }
    }
    return false;
}
