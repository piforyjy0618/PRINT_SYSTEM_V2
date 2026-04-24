#pragma once
#include "PrintSystemSDK.h"
#include <string>
#include <vector>

struct HeadBoardConfig
{
    std::string serial; // 头板序列号
};

struct MainBoardConfig
{
    std::string ip;
    int port;
    NetChannel channel;
    std::vector<HeadBoardConfig> headBoards;
};


struct PrintSystemConfig
{
    std::vector<MainBoardConfig> mainBoards;
};
