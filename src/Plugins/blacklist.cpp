#include <string>
#include <llapi/LoggerAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/mc/Types.hpp>
#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/mc/Player.hpp>
#include "../config.h"
#include "../Storage/SQLiteDatabase.h"
#include "blacklist.h"
extern Logger logger;

namespace blacklist {
    namespace {
        void database() {
            if (!std::filesystem::exists(PluginDataPath.append("blacklist.db"))) {
                logger.info("数据库 blacklist.db 已创建");
                SQLiteDatabase db(PluginData + "/blacklist.db");
                db.close();
            }
        }

        class BlacklistCommand : public Command {
            std::string cause;
            std::string PlayerString;
            int time;
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
                    switch (op) {
                        case add:
                            break;
                        case remove:
                            break;
                        case list:
                            break;
                        case gui:
                            break;
                        default:
                            break;
                    }
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
        logger.info("插件 <Blacklist> 已加载");
    }
}