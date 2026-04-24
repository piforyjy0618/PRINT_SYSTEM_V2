#include "config_manager.h"
#include "spdlog/spdlog.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

bool ConfigManager::LoadConfig(const std::string &filePath, PrintSystemConfig &config)
{
    try
    {
        boost::property_tree::ptree pt;
        // 核心解析：读取 XML 到 ptree
        boost::property_tree::read_xml(filePath, pt);

        // 2. 遍历读取主板配置列表
        // 寻找 PrintSystem 节点下的 MainBoards 节点
        auto mainBoardsNode = pt.get_child_optional("PrintSystem.MainBoards");
        if (mainBoardsNode)
        {
            for (const auto &node : *mainBoardsNode)
            {
                // 确保只处理名为 "Board" 的节点
                if (node.first == "Board")
                {
                    MainBoardConfig mbCfg;
                    // 读取 XML 节点的属性 (<xmlattr>)
                    mbCfg.ip = node.second.get<std::string>("<xmlattr>.ip", "127.0.0.1");
                    mbCfg.port = node.second.get<int>("<xmlattr>.port", 0);

                    std::string channelStr = node.second.get<std::string>("<xmlattr>.channel", "TCP");
                    mbCfg.channel = (channelStr == "UDP") ? NetChannel::UDP : NetChannel::TCP;

                    // 读取当前主板下的头板
                    auto headBoardsNode = node.second.get_child_optional("HeadBoards");
                    if (headBoardsNode)
                    {
                        for (const auto &hbNode : *headBoardsNode)
                        {
                            if (hbNode.first == "HeadBoard")
                            {
                                HeadBoardConfig hbCfg;
                                // 读取头板的序列号属性
                                hbCfg.serial = hbNode.second.get<std::string>("<xmlattr>.serial", "Unknown_Serial");
                                mbCfg.headBoards.push_back(hbCfg);
                            }
                        }
                    }

                    config.mainBoards.push_back(mbCfg);
                }
            }
        }

        spdlog::info("Config loaded successfully. Found {} MainBoards.", config.mainBoards.size());
        return true;
    }
    catch (const boost::property_tree::xml_parser_error &e)
    {
        spdlog::error("Failed to parse XML config: {}", e.what());
        return false;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Config loading error: {}", e.what());
        return false;
    }
}

bool ConfigManager::SaveConfig(const std::string &filePath, const PrintSystemConfig &config)
{
    try
    {
        boost::property_tree::ptree pt;

        // 1. 写入系统基础配置
        // pt.put("PrintSystem.System.<xmlattr>.sdkVersion", config.sdkVersion);

        // 2. 写入主板列表
        for (const auto &mbCfg : config.mainBoards)
        {
            boost::property_tree::ptree boardNode;

            // 设置属性
            boardNode.put("<xmlattr>.ip", mbCfg.ip);
            boardNode.put("<xmlattr>.port", mbCfg.port);

            std::string channelStr = (mbCfg.channel == NetChannel::UDP) ? "UDP" : "TCP";
            boardNode.put("<xmlattr>.channel", channelStr);

            // 写入头板列表
            if (!mbCfg.headBoards.empty())
            {
                boost::property_tree::ptree headBoardsNode; // 创建 <HeadBoards> 父节点

                for (const auto &hbCfg : mbCfg.headBoards)
                {
                    boost::property_tree::ptree hbNode; // 创建 <HeadBoard> 节点
                    hbNode.put("<xmlattr>.serial", hbCfg.serial);

                    // 将单个头板加入到 <HeadBoards> 中
                    headBoardsNode.add_child("HeadBoard", hbNode);
                }
                // 将 <HeadBoards> 加入到当前主板 <Board> 中
                boardNode.add_child("HeadBoards", headBoardsNode);
            }

            // 使用 add_child 添加多个同名节点 "Board"
            pt.add_child("PrintSystem.MainBoards.Board", boardNode);
        }

        // 3. 写入文件
        boost::property_tree::xml_writer_settings<std::string> settings(' ', 4);
        boost::property_tree::write_xml(filePath, pt, std::locale(), settings);

        spdlog::info("Config saved successfully to: {}", filePath);
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to save XML config: {}", e.what());
        return false;
    }
}
