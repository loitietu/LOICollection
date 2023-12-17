#include <llapi/LoggerAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/EventAPI.h>
#include "../tool.h"
#include "include/i18nLang.h"
#include "include/mute.h"
extern Logger logger;

namespace mute {
    namespace {
        void database() {
            if (!std::filesystem::exists(PluginData + "/mute.db")) {
                logger.info("<Mute>: 数据库已创建");
                SQLiteDatabase db(PluginData + "/mute.db");
                db.close();
            }
        }

        class MuteCommand : public Command {
            std::string cause = "";
            int time = 0;
            enum MUTEOP : int {
                add = 1,
                remove = 2,
                gui = 3
            } op;
            CommandSelector<Player> target;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    SQLiteDatabase db(PluginData + "/blacklist.db");
                    i18nLang lang("./plugins/LOICollection/language.json");
                    auto res = target.results(ori);
                    switch (op) {
                        case MUTEOP::add:
                            break;
                        case MUTEOP::remove:
                            break;
                        case MUTEOP::gui:
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
                    registry->registerCommand("mute", "§e§lLOICollection -> §b服务器禁言", CommandPermissionLevel::GameMasters, {(CommandFlagValue)0},{(CommandFlagValue)0x80});
                    registry->addEnum<MUTEOP>("gui", {{"gui", MUTEOP::gui}});
                    registry->addEnum<MUTEOP>("add", {{"add", MUTEOP::add}});
                    registry->addEnum<MUTEOP>("remove", {{"remove", MUTEOP::remove}});
                    registry->registerOverload<MuteCommand>("mute", makeMandatory<CommandParameterDataType::ENUM>(&MuteCommand::op, "op", "gui"));
                    registry->registerOverload<MuteCommand>("mute", makeMandatory<CommandParameterDataType::ENUM>(&MuteCommand::op, "op", "add"), makeMandatory(&MuteCommand::target, "player"));
                    registry->registerOverload<MuteCommand>("mute", makeMandatory<CommandParameterDataType::ENUM>(&MuteCommand::op, "op", "add"), makeMandatory(&MuteCommand::target, "player"), makeMandatory(&MuteCommand::time, "time"));
                    registry->registerOverload<MuteCommand>("mute", makeMandatory<CommandParameterDataType::ENUM>(&MuteCommand::op, "op", "add"), makeMandatory(&MuteCommand::target, "player"), makeMandatory(&MuteCommand::cause, "cause"));
                    registry->registerOverload<MuteCommand>("mute", makeMandatory<CommandParameterDataType::ENUM>(&MuteCommand::op, "op", "add"), makeMandatory(&MuteCommand::target, "player"), makeMandatory(&MuteCommand::cause, "cause"), makeMandatory(&MuteCommand::time, "time"));
                    registry->registerOverload<MuteCommand>("mute", makeMandatory<CommandParameterDataType::ENUM>(&MuteCommand::op, "op", "remove"), makeMandatory(&MuteCommand::target, "player"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                MuteCommand::setup(e.mCommandRegistry);
                return true;
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        database();
        listen();
        logger.info("<Mute>: 插件已加载");
    }
}