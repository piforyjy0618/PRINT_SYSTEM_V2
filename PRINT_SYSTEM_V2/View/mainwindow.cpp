#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//#include "IPrintSystem.h"
//#include <iostream>

//int main() {
//    IMainBoard* system = CreatePrintSystem();

//    if (system->Initialize("config.xml")) {
//        // 假设获取了第 0 号头板
//        IHeadboard* hb = system->GetHeadboard(0);

//        std::cout << "[1] Initial printhead count: " << hb->GetPrintheadCount() << "\n";

//        // =========================================
//        // 动态添加喷头
//        // =========================================
//        std::cout << "[2] Adding Ricoh Gen5 printheads...\n";
//        // 传入型号，以及必要的硬件标识（比如端口A，端口B）
//        IPrinthead* head1 = hb->AddPrinthead(PrintheadModel::RicohGen5, "Port=A;VoltageBase=24V");
//        IPrinthead* head2 = hb->AddPrinthead(PrintheadModel::RicohGen5, "Port=B;VoltageBase=24V");

//        std::cout << "    Current printhead count: " << hb->GetPrintheadCount() << "\n";

//        // 测试调用特性功能
//        if (head1 && head1->GetModel() == PrintheadModel::RicohGen5) {
//            auto ricohExt = static_cast<IRicohGen5Extension*>(head1->QueryExtension(IID_RicohGen5));
//            if (ricohExt) {
//                std::cout << "    Setting voltage curve for Head 1...\n";
//                ricohExt->SetVoltageCurve(3);
//            }
//        }

//        // =========================================
//        // 动态移除喷头
//        // =========================================
//        std::cout << "[3] Removing the first printhead...\n";
//        // 注意：调用 Remove 之后，head1 指针就失效（悬空）了，不能再使用！
//        hb->RemovePrinthead(0);

//        // 也可以通过指针移除
//        // hb->RemovePrintheadByPtr(head2);

//        std::cout << "    Final printhead count: " << hb->GetPrintheadCount() << "\n";
//    }

//    system->Release();
//    return 0;
//}
