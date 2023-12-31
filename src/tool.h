#ifndef LOICOLLECTION_TOOL_H
#define LOICOLLECTION_TOOL_H

#include <string>
#include <vector>
#include <Nlohmann/json.hpp>
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
    bool isJsonArrayFind(const nlohmann::ordered_json& json, const std::string& find);
    string replaceString(string str, const string& from, const string& to);
    string vec3ToString(Vec3 vec3);

    namespace llmoney {
        int get(Player* player);
        bool add(Player* player, int amount);
        bool reduce(Player* player, int amount);
        bool set(Player* player, int amount);
    }
}

#endif