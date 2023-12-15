#include <string>
#include <vector>
#include <ctime>
#include <llapi/LoggerAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/mc/Types.hpp>
#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/mc/Player.hpp>
#include "i18nLang.h"
#include "../Storage/SQLiteDatabase.h"
#include "blacklist.h"
extern Logger logger;
const std::string PluginData = "./plugins/LOICollection/data";

namespace blacklist {
    namespace {
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

        std::vector<std::string> split(const std::string& s, char delimiter) {
            std::vector<std::string> tokens;
            std::stringstream ss(s);
            std::string token;
            while (std::getline(ss, token, delimiter)) {
                tokens.push_back(token);
            }
            return tokens;
        }

        void database() {
            if (!std::filesystem::exists(PluginData + "/blacklist.db")) {
                logger.info("<Blacklist>: 数据库已创建");
                SQLiteDatabase db(PluginData + "/blacklist.db");
                db.close();
            }
        }

        class BlacklistCommand : public Command {
            std::string cause = "";
            std::string PlayerString = "";
            int time = 0;
            enum BLACKLISTOP : int {
                add = 1,
                remove = 2,
                list = 3,
                gui = 4
            } op;
            enum BLACKLISTYPE : int {
                xuid = 1,
                ip = 2
            } BlacklisType;
            CommandSelector<Player> target;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    SQLiteDatabase db(PluginData + "/blacklist.db");
                    i18nLang lang("./plugins/LOICollection/language.json");
                    auto res = target.results(ori);
                    switch (op) {
                        case BLACKLISTOP::add:
                            switch (BlacklisType) {
                                case BLACKLISTYPE::xuid:
                                    for (auto i : res) {
                                        std::string xuid = i->getXuid();
                                        std::string BlackCause = cause;
                                        if (cause.empty()) BlackCause = lang.tr(get(i), "blacklist.cause");
                                        if (!db.existsTable("Xuid" + xuid)) {
                                            db.setTable("Xuid" + xuid);
                                            db.createTable();
                                            db.set("Cause", BlackCause);
                                            db.set("Time", std::to_string(time));
                                        }
                                        i->kick(BlackCause);
                                    }
                                    outp.success("Blacklist: Adding players succeeded.");
                                    break;
                                case BLACKLISTYPE::ip:
                                    for (auto i : res) {
                                        std::string mIp = split(i->getIP(), ':')[0];
                                        std::replace(mIp.begin(), mIp.end(), '.', '\0');
                                        std::string BlackCause = cause;
                                        if (cause.empty()) BlackCause = lang.tr(get(i), "blacklist.cause");
                                        if (!db.existsTable("Ip" + mIp)) {
                                            db.setTable("Ip" + mIp);
                                            db.createTable();
                                            db.set("Cause", BlackCause);
                                            db.set("Time", std::to_string(time));
                                        }
                                        i->kick(BlackCause);
                                    }
                                    outp.success("Blacklist: Adding players succeeded.");
                                    break;
                                default:
                                    outp.error("Blacklist: Failed to add player.");
                                    break;
                            }
                            break;
                        case BLACKLISTOP::remove:
                            break;
                        case BLACKLISTOP::list:
                            break;
                        case BLACKLISTOP::gui:
                            break;
                        default:
                            logger.error("<Blacklist>: 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Blacklist: Instruction error.");
                            break;
                    }
                    lang.close();
                    db.close();
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("blacklist", "§e§lLOICollection -> §b服务器黑名单", CommandPermissionLevel::GameMasters, {(CommandFlagValue)0},{(CommandFlagValue)0x200});
                    registry->addEnum<BLACKLISTOP>("gui", {{"gui", BLACKLISTOP::gui}});
                    registry->addEnum<BLACKLISTOP>("add", {{"add", BLACKLISTOP::add}});
                    registry->addEnum<BLACKLISTOP>("remove", {{"remove", BLACKLISTOP::remove}});
                    registry->addEnum<BLACKLISTOP>("list", {{"list", BLACKLISTOP::list}});
                    registry->addEnum<BLACKLISTYPE>("type", {{"xuid", BLACKLISTYPE::xuid},{"ip", BLACKLISTYPE::ip}});
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "gui"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "list"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "add"), makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::BlacklisType, "type", "type"), makeMandatory(&BlacklistCommand::target, "player"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "add"), makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::BlacklisType, "type", "type"), makeMandatory(&BlacklistCommand::target, "player"), makeMandatory(&BlacklistCommand::cause, "cause"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "add"), makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::BlacklisType, "type", "type"), makeMandatory(&BlacklistCommand::target, "player"), makeMandatory(&BlacklistCommand::cause, "cause"), makeMandatory(&BlacklistCommand::time, "time"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "remove"), makeMandatory(&BlacklistCommand::PlayerString, "string"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                BlacklistCommand::setup(e.mCommandRegistry);
                return true;
            });
            Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& e) {
                SQLiteDatabase db(PluginData + "/blacklist.db");
                std::string xuid = e.mPlayer->getXuid();
                std::string ip = split(e.mPlayer->getIP(), ':')[0];
                std::replace(ip.begin(), ip.end(), '.', '\0');
                if (db.existsTable("Xuid" + xuid)) {
                    db.setTable("Xuid" + xuid);
                    e.mPlayer->kick(db.get("Cause"));
                    db.close();
                    return false;
                } else if(db.existsTable("Ip" + ip)) {
                    db.setTable("Ip" + ip);
                    e.mPlayer->kick(db.get("Cause"));
                    db.close();
                    return false;
                } else {
                    db.close();
                    return true;
                }
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        listen();
        database();
        logger.info("<Blacklist>: 插件已加载");
    }
}