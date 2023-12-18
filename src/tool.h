#ifndef LOICOLLECTION_TOOL_H
#define LOICOLLECTION_TOOL_H

#include <ctime>
#include <string>
#include <vector>
#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/mc/Player.hpp>
#include "Storage/SQLiteDatabase.h"
const std::string PluginData = "./plugins/LOICollection/data";

namespace tool {
    std::string get(ServerPlayer* player);
    std::string get(Player* player);
    std::vector<std::string> split(const std::string& s, char delimiter);
    std::string timeCalculate(int hours);
    bool isReach(const std::string& timeString);
    bool isMute(Player* player);
    bool isMute(ServerPlayer* player);
}

#endif