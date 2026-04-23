#ifndef HEADBOARD_H
#define HEADBOARD_H

#include "PrintSystemSDK.h"
#include "MainBoard/i_command_sender.h"
#include <vector>
#include <memory>

class HeadBoard : public IHeadboard
{
    std::string m_serial;
    std::vector<std::unique_ptr<IPrinthead>> m_heads; // 安全管理生命周期
    ICommandSender *m_sender;

public:
    HeadBoard(const std::string &serial, ICommandSender *sender);
    virtual ~HeadBoard();

    int GetPrintheadCount() const override;
    const char *GetHeadBoardSerial() const override;
    IPrinthead *GetPrinthead(int index) const override;
    IPrinthead *AddPrinthead(PrintheadModel model, const char *configParams) override;
    bool RemovePrinthead(int index) override;
    bool RemovePrintheadByPtr(IPrinthead *head) override;
};

#endif // HEADBOARD_H
