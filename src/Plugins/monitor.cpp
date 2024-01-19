#include <string>
#include <vector>
#include <llapi/EventAPI.h>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/ServerPlayer.hpp>
#include <Nlohmann/json.hpp>
#include "../Storage/JsonManager.h"
#include "../include/LLMoney.h"
#include "../tools/tool.h"
#include "../API.h"
#include "include/monitor.h"

namespace monitor {
    namespace {
        bool OnLLMoneyCallback(LLMoneyEvent type, xuid_t from, xuid_t to, money_t value) {
            JsonManager config("./plugins/LOICollection/config.json");
            nlohmann::ordered_json data = config.get("Monitor");
            config.clear();
            Player* PlayerFrom = tool::toXuidPlayer(from);
            Player* PlayerTo = tool::toXuidPlayer(to);
            switch (type) {
                case LLMoneyEvent::Add: {
                    if (PlayerTo != nullptr) {
                        int llmoney = tool::llmoney::get(PlayerTo);
                        std::string MonitorString = data["llmoney"];
                        MonitorString = tool::replaceString(MonitorString, "${OriMoney}", std::to_string(llmoney));
                        MonitorString = tool::replaceString(MonitorString, "${SetMoney}", std::to_string(value));
                        MonitorString = tool::replaceString(MonitorString, "${GetMoney}", std::to_string(llmoney + value));
                        PlayerTo->sendTextPacket(MonitorString);
                    }
                    break;
                }
                case LLMoneyEvent::Reduce: {
                    if (PlayerTo != nullptr) {
                        int llmoney = tool::llmoney::get(PlayerTo);
                        std::string MonitorString = data["llmoney"];
                        MonitorString = tool::replaceString(MonitorString, "${OriMoney}", std::to_string(llmoney));
                        MonitorString = tool::replaceString(MonitorString, "${SetMoney}", std::to_string(value));
                        MonitorString = tool::replaceString(MonitorString, "${GetMoney}", std::to_string(llmoney - value));
                        PlayerTo->sendTextPacket(MonitorString);
                    }
                    break;
                }
                default:
                    if (PlayerTo != nullptr) {
                        int llmoney = tool::llmoney::get(PlayerTo);
                        std::string MonitorString = data["llmoney"];
                        MonitorString = tool::replaceString(MonitorString, "${OriMoney}", std::to_string(llmoney));
                        MonitorString = tool::replaceString(MonitorString, "${SetMoney}", std::to_string(value));
                        MonitorString = tool::replaceString(MonitorString, "${GetMoney}", std::to_string(llmoney + value));
                        PlayerTo->sendTextPacket(MonitorString);
                    }
                    break;
            }
            data.clear();
            return true;
        }

        void listen() {
            LLMoneyCallback callback = OnLLMoneyCallback;
            LLMoneyListenBeforeEvent(callback);
            Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& e) {
                if (!tool::isBlacklist(e.mPlayer)) {
                    JsonManager config("./plugins/LOICollection/config.json");
                    nlohmann::ordered_json data = config.get("Monitor");
                    config.clear();
                    std::string MonitorString = data["join"];
                    MonitorString = std::string(LOICollectionAPI::translateString(MonitorString, e.mPlayer, false));
                    tool::BroadcastText(MonitorString);
                }
                return true;
            });
            Event::PlayerLeftEvent::subscribe([](const Event::PlayerLeftEvent& e) {
                JsonManager config("./plugins/LOICollection/config.json");
                nlohmann::ordered_json data = config.get("Monitor");
                config.clear();
                std::string MonitorString = data["left"];
                MonitorString = std::string(LOICollectionAPI::translateString(MonitorString, e.mPlayer, false));
                tool::BroadcastText(MonitorString);
                return true;
            });
            Event::PlayerCmdEvent::subscribe([](const Event::PlayerCmdEvent& e) {
                JsonManager config("./plugins/LOICollection/config.json");
                nlohmann::ordered_json data = config.get("Monitor");
                config.clear();
                std::vector<std::string> commandArray = tool::split(e.mCommand, ' ');
                if (tool::isJsonArrayFind(data["command"], commandArray[0])) {
                    e.mPlayer->sendTextPacket(data["tips"]);
                    return false;
                }
                return true;
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        listen();
    }
}