#include <string>
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

        void database() {
            if (!std::filesystem::exists(PluginData + "/blacklist.db")) {
                logger.info("数据库 blacklist.db 已创建");
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
                gui = 4,
                ip = 5,
                xuid = 6
            } op;
            CommandSelector<Player> target;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    SQLiteDatabase db(PluginData + "/blacklist.db");
                    auto res = target.results(ori);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    switch (op) {
                        case add:
                            switch (op) {
                                case xuid:
                                    if (!Command::checkHasTargets(res, outp)) return;
                                    for (auto i: res) {
                                        std::string xuid = i->getXuid();
                                        std::string BlackCause = cause;
                                        if (cause == "") BlackCause = lang.tr(get(i), "blacklist.cause");
                                        if (!db.exists(xuid)) {
                                            db.setTable(xuid);
                                            db.set("Cause", BlackCause);
                                            db.set("Time", std::to_string(time));
                                        }
                                        i->kick(BlackCause);
                                    }
                                    break;
                                case ip:
                                    break;
                                default:
                                    outp.error("Blacklist: Add Error");
                                    break;
                            }
                            break;
                        case remove:
                            break;
                        case list:
                            break;
                        case gui:
                            break;
                        default:
                            outp.error("Blacklist: Error");
                            break;
                    }
                    db.close();
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("blacklist", "§e§lLOICollection -> §b服务器黑名单", CommandPermissionLevel::GameMasters, {(CommandFlagValue)0},{(CommandFlagValue)0x200});
                    registry->addEnum<BLACKLISTOP>("BLACKLISTGUI", {{"gui", BLACKLISTOP::gui}});
                    registry->addEnum<BLACKLISTOP>("BLACKLISTADD", {{"add", BLACKLISTOP::add}});
                    registry->addEnum<BLACKLISTOP>("BLACKLISTREMOVE", {{"remove", BLACKLISTOP::remove}});
                    registry->addEnum<BLACKLISTOP>("BLACKLISTLIST", {{"list", BLACKLISTOP::list}});
                    registry->addEnum<BLACKLISTOP>("BLACKLISTYPE", {{"xuid", BLACKLISTOP::xuid},{"ip", BLACKLISTOP::ip}});
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "optional", "BLACKLISTGUI"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "optional", "BLACKLISTLIST"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "optional", "BLACKLISTADD"), makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "type", "BLACKLISTYPE"), makeMandatory(&BlacklistCommand::target, "player"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "optional", "BLACKLISTADD"), makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "type", "BLACKLISTYPE"), makeMandatory(&BlacklistCommand::target, "player"), makeMandatory(&BlacklistCommand::cause, "cause"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "optional", "BLACKLISTADD"), makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "type", "BLACKLISTYPE"), makeMandatory(&BlacklistCommand::target, "player"), makeMandatory(&BlacklistCommand::cause, "cause"), makeMandatory(&BlacklistCommand::time, "time"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "optional", "BLACKLISTREMOVE"), makeMandatory(&BlacklistCommand::PlayerString, "string"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                BlacklistCommand::setup(e.mCommandRegistry);
                return true;
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        listen();
        database();
        logger.info("插件 <Blacklist> 已加载");
    }
}