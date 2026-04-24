#pragma once
#include <string>
#include "system_config.h"

class ConfigManager
{
public:
    ConfigManager() = default;
    ~ConfigManager() = default;

    // 解析 XML 文件
    static bool LoadConfig(const std::string &filePath, PrintSystemConfig &config);
    // 将当前配置保存到 XML 文件里
    static bool SaveConfig(const std::string &filePath, const PrintSystemConfig &config);
};
