#ifndef PRINTSYSTEMSDK_H
#define PRINTSYSTEMSDK_H

#include "SdkExport.h"

enum class SystemEventLevel
{
    Info,
    Warning,
    Error
};

typedef void (*SystemEvent)(SystemEventLevel level, const char *source, const char *msg, void *pUserData);

enum class PrintheadModel
{
    Unknown = 0,
    QS256 = 1,
    Polaris = 2,
    StartFire = 3,
    Samba = 4
};

// 扩展接口的唯一标识符
constexpr const char *IID_QS256 = "Ext_QS256_v1";
constexpr const char *IID_Polaris = "Ext_Polaris_v1";

class IQS256Extension
{
public:
    virtual ~IQS256Extension() = default;
    virtual bool SetVoltageCurve(int curveId) = 0;
    virtual void EnableInternalHeating(bool enable) = 0;
};

class IPrinthead
{
public:
    virtual ~IPrinthead() = default;

    // 共性功能
    virtual const char *GetPrintheadSerial() const = 0;
    virtual PrintheadModel GetModel() const = 0;
    virtual float GetTemperature() const = 0;
    virtual bool Fire() = 0; // 闪喷/打印动作

    // 核心扩展机制：获取特性接口
    virtual void *QueryExtension(const char *extensionId) = 0;
};

class IHeadboard
{
public:
    virtual ~IHeadboard() = default;
    virtual const char *GetHeadBoardSerial() const = 0;

    // --- 获取当前喷头信息 ---
    virtual int GetPrintheadCount() const = 0;
    virtual IPrinthead *GetPrinthead(int index) const = 0;

    // --- 动态管理喷头接口 ---
    // 动态添加喷头：传入型号和相关配置（如排线端口、IP等），返回创建好的接口指针
    virtual IPrinthead *AddPrinthead(PrintheadModel model, const char *configParams) = 0;

    // 动态移除喷头：按索引移除，SDK 内部负责安全释放内存
    virtual bool RemovePrinthead(int index) = 0;

    // 也可以提供重载，按指针移除
    virtual bool RemovePrintheadByPtr(IPrinthead *head) = 0;
};

enum class NetChannel
{
    TCP,
    UDP,
};

class IMainBoard
{
public:
    virtual ~IMainBoard() = default;

    /**------连接------ */
    // 打开主板连接
    virtual bool OpenConnect() = 0;
    // 断开主板连接
    virtual void CloseConnect() = 0;
    // 获取连接状态
    virtual bool IsConnected() const = 0;
    // 获取主板IP地址
    virtual const char *GetIPAddress() const = 0;
    // 获取主板端口号
    virtual int GetPort() const = 0;
    // 获取主板通信方式
    virtual NetChannel GetNetChannel() const = 0;

    // 获取主板序列号
    virtual const char *GetMainBoardSerial() const = 0;
    // 获取头板数量
    virtual int GetHeadboardCount() const = 0;
    // 获取头板指针
    virtual IHeadboard *GetHeadboard(int index) const = 0;
    // 添加头板
    virtual IHeadboard *AddHeadboard(const char *serial) = 0;
    // 主板事件回调接口
    virtual void SetMainBoardEventCallback(SystemEvent cb, void *pUserData = nullptr) = 0;
};

class IPrintSystem
{
public:
    virtual ~IPrintSystem() = default;

    // 初始化系统，传入配置文件路径
    virtual bool Initialize(const char *configPath = nullptr) = 0;
    // 保存当前配置
    virtual bool SaveConfig(const char *configPath) = 0;
    // 添加主板到系统
    virtual IMainBoard *AddMainBoard(NetChannel netChannel, const char *ip, int port) = 0;
    // 移除主板
    virtual bool RemoveMainBoard(int index) = 0;
    // 获取主板数量
    virtual int GetMainBoardCount() const = 0;
    // 获取主板指针
    virtual IMainBoard *GetMainBoard(int index) const = 0;
    // 系统释放
    virtual void Release() = 0;
    // 系统事件回调接口
    virtual void SetSystemEventCallback(SystemEvent cb, void *pUserData = nullptr) = 0;
};

// 导出系统创建函数
extern "C"
{
    SDK_API IPrintSystem *CreatePrintSystem();
}

#endif // PRINTSYSTEMSDK_H
