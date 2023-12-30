#ifndef LOICOLLECTION_TOOL_H
#define LOICOLLECTION_TOOL_H

#include <ctime>
#include <string>
#include <vector>
#include <llapi/GlobalServiceAPI.h>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Scoreboard.hpp>
#include "Storage/SQLiteDatabase.h"
#include "include/LLMoney.h"
const std::string PluginData = "./plugins/LOICollection/data";

namespace tool {
    std::string get(ServerPlayer* player);
    std::string get(Player* player);
    std::vector<std::string> split(const std::string& s, char delimiter);
    std::string timeCalculate(int hours);
    bool isReach(const std::string& timeString);
    bool isMute(Player* player);
    bool isMute(ServerPlayer* player);
    Player* toServerPlayer(ServerPlayer* player);

    namespace llmoney {
        int get(Player* player);
        bool add(Player* player, int amount);
        bool reduce(Player* player, int amount);
        bool set(Player* player, int amount);
    }
}

#endif