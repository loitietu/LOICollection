#include <ctime>
#include <string>
#include <vector>
#include <filesystem>
#include <llapi/GlobalServiceAPI.h>
#include <llapi/mc/Container.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Scoreboard.hpp>
#include <Nlohmann/json.hpp>
#include "Storage/SQLiteDatabase.h"
#include "include/LLMoney.h"
#include "tool.h"

namespace tool {
    std::string get(ServerPlayer* player) {
        SQLiteDatabase db(PluginData + "/language.db");
        std::string playerLang = db.get(player->getXuid());
        db.close();
        return playerLang;
    }

    std::string get(Player* player) {
        SQLiteDatabase db(PluginData + "/language.db");
        std::string playerLang = db.get(player->getXuid());
        db.close();
        return playerLang;
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

    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(s);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    std::string timeCalculate(int hours) {
        std::time_t currentTime = std::time(nullptr);
        std::tm* timeInfo = std::localtime(&currentTime);
        timeInfo->tm_hour += 1;
        std::time_t laterTime = std::mktime(timeInfo);
        char formattedTime[15];
        std::strftime(formattedTime, sizeof(formattedTime), "%Y%m%d%H%H%S", std::localtime(&laterTime));
        std::string formattedTimeString(formattedTime);
        return formattedTimeString;
    }

    bool isReach(const std::string& timeString) {
        std::time_t currentTime = std::time(nullptr);
        char formattedTime[15];
        std::strftime(formattedTime, sizeof(formattedTime), "%Y%m%d%H%H%S", std::localtime(&currentTime));
        std::string formattedTimeString(formattedTime);
        int64_t formattedTimeInt = std::stoll(formattedTimeString);
        int64_t timeInt = std::stoll(timeString);
        if (formattedTimeInt > timeInt) return true;
        else return false;
    }

    bool isMute(Player* player) {
        SQLiteDatabase db(PluginData + "/mute.db");
        bool is_mute = db.existsTable("XUID" + player->getXuid());
        db.close();
        return is_mute;
    }

    bool isMute(ServerPlayer* player) {
        SQLiteDatabase db(PluginData + "/mute.db");
        bool is_mute = db.existsTable("XUID" + player->getXuid());
        db.close();
        return is_mute;
    }

    void BroadcastText(const std::string& text) {
        Level::broadcastText(text, TextType::SYSTEM);
    }

    void createScoreboardObject(const std::string& id) {
        Level::runcmdEx("scoreboard objectives add " + id + " dummy");
    }

    Player* toServerPlayer(ServerPlayer* player) {
        return Global<Level>->getPlayer(player->getXuid());
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
    
    bool isJsonArrayFind(const nlohmann::ordered_json& json, const std::string& find) {
        auto it = std::find(json.begin(), json.end(), find);
        return it != json.end();
    }

    bool isItemPlayerInventory(Player* player, ItemStack* item) {
        Container& itemInventory = player->getInventory();
        for (int i = 0; i < itemInventory.getSize(); i++) {
            ItemStack itemObject = itemInventory.getItem(i);
            if (item->getTypeName() == itemObject.getTypeName()) {
                if (item->getCount() <= itemObject.getCount()) {
                    return true;
                }
            }
        }
        return false;
    }

    std::string replaceString(std::string str, const std::string& from, const std::string& to) {
        for (std::string::size_type pos(0); pos != std::string::npos; pos += to.length()) {
            if ((pos = str.find(from, pos)) != std::string::npos) str.replace(pos, from.length(), to);
            else break;
        }
        return str;
    }

    std::string vec3ToString(Vec3 vec3) {
        return std::to_string(vec3[0]) + "," + std::to_string(vec3[1]) + "," + std::to_string(vec3[2]);
    }

    namespace llmoney {
        int get(Player* player) {
            return LLMoneyGet(player->getXuid());
        }

        bool add(Player* player, int amount) {
            return LLMoneyAdd(player->getXuid(), amount);
        }

        bool reduce(Player* player, int amount) {
            return LLMoneyReduce(player->getXuid(), amount);
        }

        bool set(Player* player, int amount) {
            return LLMoneySet(player->getXuid(), amount);
        }
    }
}