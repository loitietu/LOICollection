#ifndef LOICOLLECTION_TOOL_H
#define LOICOLLECTION_TOOL_H

#include <string>
#include <vector>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/mc/Player.hpp>
#include <Nlohmann/json.hpp>
const std::string PluginData = "./plugins/LOICollection/data";

namespace tool {
    std::string get(Player* player);
    bool isBlacklist(Player* player);
    int toInt(const std::string& intString, int defaultValue);
    void updateChat(Player* player);
    std::vector<std::string> split(const std::string& s, char delimiter);
    std::string timeCalculate(int hours);
    bool isReach(const std::string& timeString);
    bool isMute(Player* player);
    Player* toXuidPlayer(const std::string& xuid);
    Player* toNamePlayer(const std::string& name);
    nlohmann::ordered_json getJson(const std::string& path);
    bool isJsonArrayFind(const nlohmann::ordered_json& json, const std::string& find);
    bool isItemPlayerInventory(Player* player, ItemStack* item);
    std::string replaceString(std::string str, const std::string& from, const std::string& to);
    std::string vec3ToString(Vec3 vec3);

    namespace llmoney {
        int get(Player* player);
        bool add(Player* player, int amount);
        bool reduce(Player* player, int amount);
        bool set(Player* player, int amount);
    }
}

#endif