#include <ctime>
#include <string>
#include <vector>
#include <filesystem>
#include <llapi/mc/Container.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Scoreboard.hpp>
#include <Nlohmann/json.hpp>
#include "../Storage/SQLiteDatabase.h"
#include "../Storage/JsonManager.h"
#include "../include/LLMoney.h"
#include "tool.h"

namespace tool {
    std::string get(Player* player) {
        if (std::filesystem::exists(PluginData + "/language.db") && player != nullptr) {
            SQLiteDatabase db(PluginData + "/language.db");
            if (db.existsTable(player->getXuid())) {
                std::string playerLang = db.get(player->getXuid());
                db.close();
                return playerLang;
            } else {
                return "zh_CN";
            }
        } else {
            return "zh_CN";
        }
    }

    std::string timeCalculate(int hours) {
        if (hours > 0) {
            std::time_t currentTime = std::time(nullptr);
            std::tm* timeInfo = std::localtime(&currentTime);
            timeInfo->tm_hour += hours;
            std::time_t laterTime = std::mktime(timeInfo);
            char formattedTime[15];
            std::strftime(formattedTime, sizeof(formattedTime), "%Y%m%d%H%M%S", std::localtime(&laterTime));
            std::string formattedTimeString(formattedTime);
            return formattedTimeString;
        } else {
            return "0";
        }
    }

    std::string replaceString(std::string str, const std::string& from, const std::string& to) {
        for (std::string::size_type pos(0); pos != std::string::npos; pos += to.length()) {
            if ((pos = str.find(from, pos)) != std::string::npos) {
                str.replace(pos, from.length(), to);
            } else break;
        }
        return str;
    }

    std::string vec3ToString(Vec3 vec3) {
        return std::to_string(vec3[0]) + "," + std::to_string(vec3[1]) + "," + std::to_string(vec3[2]);
    }

    bool isBlacklist(Player* player) {
        if (std::filesystem::exists(PluginData + "/blacklist.db")) {
            SQLiteDatabase db(PluginData + "/blacklist.db");
            std::string xuid = player->getXuid();
            std::string ip = tool::split(player->getIP(), ':')[0];
            std::replace(ip.begin(), ip.end(), '.', 'A');
            if (db.existsTable("XUID" + xuid)) {
                db.close();
                return true;
            } else if(db.existsTable("IP" + ip)) {
                db.close();
                return true;
            }
            db.close();
            return false;
        } else {
            return false;
        }
    }

    bool isReach(const std::string& timeString) {
        if (timeString != "0") {
            std::time_t currentTime = std::time(nullptr);
            char formattedTime[15];
            std::strftime(formattedTime, sizeof(formattedTime), "%Y%m%d%H%M%S", std::localtime(&currentTime));
            std::string formattedTimeString(formattedTime);
            int64_t formattedTimeInt = std::stoll(formattedTimeString);
            int64_t timeInt = std::stoll(timeString);
            if (formattedTimeInt > timeInt) return true;
            else return false;
        } else {
            return false;
        }
    }

    bool isMute(Player* player) {
        if (std::filesystem::exists(PluginData + "/mute.db")) {
            SQLiteDatabase db(PluginData + "/mute.db");
            bool is_mute = db.existsTable("XUID" + player->getXuid());
            db.close();
            return is_mute;
        } else {
            return false;
        }
    }

    bool isJsonArrayFind(const nlohmann::ordered_json& json, const std::string& find) {
        auto it = std::find(json.begin(), json.end(), find);
        return it != json.end();
    }

    bool isItemPlayerInventory(Player* player, ItemStack* item) {
        Container& itemInventory = player->getInventory();
        for (int i = 0; i < itemInventory.getSize(); i++) {
            auto& itemObject = itemInventory.getItem(i);
            if (!itemObject.isNull()) {
                if (item->getTypeName() == itemObject.getTypeName()) {
                    if (item->getCount() <= itemObject.getCount()) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    int toInt(const std::string& intString, int defaultValue) {
        int result = defaultValue;
        try {
            result = std::stoi(intString);
        } catch (const std::exception& e) {
            result = defaultValue;
        }
        return result;
    }

    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(s);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    std::vector<std::string> getAllPlayerName() {
        std::vector<Player*> players = Global<Level>->getAllPlayers();
        std::vector<std::string> names;
        for (auto& player : players) {
            names.push_back(player->getName());
        }
        return names;
    }

    Player* toXuidPlayer(const std::string& xuid) {
        return Global<Level>->getPlayer(xuid);
    }

    Player* toNamePlayer(const std::string& name) {
        std::vector<Player*> players = Global<Level>->getAllPlayers();
        for (auto player : players) {
            if (player->getName() == name) {
                return player;
            }
        }
        return nullptr;
    }

    nlohmann::ordered_json getJson(const std::string& path) {
        JsonManager data(path);
        return data.toJson();
    }

    void updateChat(Player* player) {
        if (std::filesystem::exists(PluginData + "/chat.db") && player != nullptr) {
            std::string xuid = player->getXuid();
            SQLiteDatabase db(PluginData + "/chat.db");
            if (db.existsTable("XUID" + xuid + "TITLE")) {
                db.setTable("XUID" + xuid + "TITLE");
                for (auto& i : db.listTable("XUID" + xuid + "TITLE")) {
                    std::string timeString = std::to_string(tool::toInt(db.get(i), 0));
                    if (tool::isReach(timeString)) {
                        db.remove(i);
                    }
                }
            }
            if (db.existsTable("XUID" + xuid)) {
                db.setTable("XUID" + xuid);
                std::string title = db.get("title");
                db.setTable("XUID" + xuid + "TITLE");
                if (!db.exists(title)) {
                    db.setTable("XUID" + xuid);
                    db.update("title", "None");
                }
            }
            db.close();
        }
    }

    void clearItem(Player* player, ItemStack* item) {
        int itemCount = item->getCount();
        Container& itemInventory = player->getInventory();
        for (int i = 0; i < itemInventory.getSize(); i++) {
            auto& itemObject = itemInventory.getItem(i);
            if (!itemObject.isNull()) {
                if (item->getTypeName() == itemObject.getTypeName()) {
                    if (itemCount <= itemObject.getCount()) {
                        ItemStack newItem = itemObject.clone();
                        newItem.remove(itemCount);
                        itemInventory.setItem(i, newItem);
                        break;
                    } else {
                        itemCount -= itemObject.getCount();
                        ItemStack newItem = itemObject.clone();
                        newItem.remove(itemObject.getCount());
                        itemInventory.setItem(i, newItem);
                    }
                }
            }
        }
        player->refreshInventory();
    }

    namespace llmoney {
        int get(Player* player) {
            return LLMoneyGet(player->getXuid());
        }

        bool add(Player* player, int amount) {
            if (amount > 0) {
                return LLMoneyAdd(player->getXuid(), amount);
            }
            return false;
        }

        bool reduce(Player* player, int amount) {
            if (amount > 0) {
                return LLMoneyReduce(player->getXuid(), amount);
            }
            return false;
        }

        bool set(Player* player, int amount) {
            if (amount > 0) {
                return LLMoneySet(player->getXuid(), amount);
            }
            return false;
        }
    }
}