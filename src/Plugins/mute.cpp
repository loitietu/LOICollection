#include <string>
#include <llapi/LoggerAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/mc/Player.hpp>
#include "../tool.h"
#include "../Storage/SQLiteDatabase.h"
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
                    SQLiteDatabase db(PluginData + "/mute.db");
                    i18nLang lang("./plugins/LOICollection/language.json");
                    auto res = target.results(ori);
                    switch (op) {
                        case MUTEOP::add:
                            if (res.empty()) {
                                outp.error("Mute: No player selected.");
                                break;
                            }
                            for (auto i : res) {
                                std::string xuid = i->getXuid();
                                std::string MuteCause = cause;
                                std::string timeString = std::to_string(time);
                                if (cause.empty()) MuteCause = lang.tr(tool::get(i), "mute.cause");
                                if (time) timeString = tool::timeCalculate(time);
                                if (!db.existsTable("XUID" + xuid)) {
                                    db.setTable("XUID" + xuid);
                                    db.createTable();
                                    db.set("Cause", MuteCause);
                                    db.set("Time", timeString);
                                }
                            }
                            outp.success("Mute: Adding players succeeded.");
                            break;
                        case MUTEOP::remove:
                            if (res.empty()) {
                                outp.error("Mute: No player selected.");
                                break;
                            }
                            for (auto i : res) {
                                std::string xuid = i->getXuid();
                                if (!db.existsTable("XUID" + xuid)) {
                                    db.removeTable("XUID" + xuid);
                                }
                            }
                            outp.success("Mute: The player silence status has been deleted.");
                            break;
                        case MUTEOP::gui:
                            break;
                        default:
                            logger.error("<Mute>: 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Mute: Instruction error.");
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
            Event::PlayerChatEvent::subscribe([](const Event::PlayerChatEvent& e) {
                std::string xuid = e.mPlayer->getXuid();
                SQLiteDatabase db(PluginData + "/mute.db");
                if(db.existsTable("XUID" + xuid)) {
                    db.setTable("XUID" + xuid);
                    std::string timeString = db.get("Time");
                    if (tool::isReach(timeString) && timeString != "0") {
                        db.removeTable("XUID" + xuid);
                        db.close();
                        return true;
                    }
                    e.mPlayer->sendTextPacket(db.get("Cause"));
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
        database();
        listen();
        logger.info("<Mute>: 插件已加载");
    }
}