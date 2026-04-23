#ifndef PRINTSYSTEMSDK_H
#define PRINTSYSTEMSDK_H

#pragma once
#include "SdkExport.h"

enum class PrintheadModel {
    Unknown = 0,
    QS256 = 1,
    Polaris = 2,
    StartFire = 3,
    Samba = 4
};

// 扩展接口的唯一标识符
constexpr const char* IID_QS256  = "Ext_QS256_v1";
constexpr const char* IID_Polaris = "Ext_Polaris_v1";

class IQS256Extension {
public:
    virtual ~IQS256Extension() = default;
    virtual bool SetVoltageCurve(int curveId) = 0;
    virtual void EnableInternalHeating(bool enable) = 0;
};

class IPrinthead {
public:
    virtual ~IPrinthead() = default;

    // 共性功能
    virtual const char* GetPrintheadSerial() const = 0;
    virtual PrintheadModel GetModel() const = 0;
    virtual float GetTemperature() const = 0;
    virtual bool Fire() = 0; // 闪喷/打印动作

    // 核心扩展机制：获取特性接口
    virtual void* QueryExtension(const char* extensionId) = 0;
};

class IHeadboard {
public:
    virtual ~IHeadboard() = default;
    virtual const char* GetHeadBoardSerial() const = 0;

    // --- 获取当前喷头信息 ---
    virtual int GetPrintheadCount() const = 0;
    virtual IPrinthead* GetPrinthead(int index) const = 0;

    // --- 动态管理喷头接口 ---
    // 动态添加喷头：传入型号和相关配置（如排线端口、IP等），返回创建好的接口指针
    virtual IPrinthead* AddPrinthead(PrintheadModel model, const char* configParams) = 0;

    // 动态移除喷头：按索引移除，SDK 内部负责安全释放内存
    virtual bool RemovePrinthead(int index) = 0;

    // 也可以提供重载，按指针移除
    virtual bool RemovePrintheadByPtr(IPrinthead* head) = 0;
};

class IMainboard {
public:
    virtual ~IMainboard() = default;

    virtual bool ConnectTCP(int port) = 0;
    virtual void DisconnectTCP() = 0;
    virtual bool IsTCPConnected() const = 0;

    virtual bool ConnectUDP(int port) = 0;
    virtual void DisconnectUDP() = 0;
    virtual bool IsUDPConnected() const = 0;

    virtual const char* GetIPAddress() const = 0;
    virtual const char* GetMainBoardSerial() const = 0;
    virtual int GetHeadboardCount() const = 0;
    virtual IHeadboard* GetHeadboard(int index) const = 0;
    virtual IHeadboard* AddHeadboard(const char* serial) = 0;
};

class IPrintSystem {
public:
    virtual ~IPrintSystem() = default;
    virtual bool Initialize(const char* configPath) = 0;

    // 仅添加主板到系统，不立即发起网络连接
    virtual IMainboard* AddMainboard(const char* ip) = 0;
    // 允许动态移除主板
    virtual bool RemoveMainboard(int index) = 0;

    virtual int GetMainboardCount() const = 0;
    virtual IMainboard* GetMainboard(int index) const = 0;

    // 注册系统级回调函数（用于接收底层主动上报的错误、状态等）
    // virtual void SetEventCallback(SystemEventCallback cb) = 0;

    // 自身释放的方法
    virtual void Release() = 0;
};

// 导出系统创建函数
extern "C" {
    SDK_API IPrintSystem* CreatePrintSystem();
}

#endif // PRINTSYSTEMSDK_H
