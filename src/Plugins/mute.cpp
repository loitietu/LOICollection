#include <string>
#include <vector>
#include <filesystem>
#include <llapi/LoggerAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/FormUI.h>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Player.hpp>
#include "../API.h"
#include "../utils/tool.h"
#include "../Storage/SQLiteDatabase.h"
#include "include/i18nLang.h"
#include "include/mute.h"
extern Logger logger;

namespace mute {
    namespace {
        void addGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "mute.gui.add.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "mute.gui.label")));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "mute.gui.add.dropdown"), tool::getAllPlayerName()));
            form.append(Form::Input("input1", lang.tr(PlayerLanguage, "mute.gui.add.input1"), "", lang.tr(PlayerLanguage, "mute.cause")));
            form.append(Form::Input("input2", lang.tr(PlayerLanguage, "mute.gui.add.input2"), "", "0"));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string PlayerSelectName = mp["dropdown"]->getString();
                std::string PlayerInputCause = mp["input1"]->getString();
                int PlayerInputTime = tool::toInt(mp["input2"]->getString(), 0);
                Level::runcmdEx("mute add " + PlayerSelectName + " " + PlayerInputCause + " " + std::to_string(PlayerInputTime));
            });
            lang.close();
        }

        void removeGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "mute.gui.remove.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "mute.gui.label")));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "mute.gui.remove.dropdown"), tool::getAllPlayerName()));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string PlayerSelectString = mp["dropdown"]->getString();
                Level::runcmdEx("mute remove " + PlayerSelectString);
            });
            lang.close();
        }

        void menuGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "mute.gui.title"), lang.tr(PlayerLanguage, "mute.gui.label"));
            form.addButton(lang.tr(PlayerLanguage, "mute.gui.addMute"), "textures/ui/backup_replace");
            form.addButton(lang.tr(PlayerLanguage, "mute.gui.removeMute"), "textures/ui/free_download_symbol");
            form.sendTo(player, [](Player* pl, int id) {
                if (id == -1) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                switch (id) {
                    case 0:
                        addGui(pl);
                        break;
                    case 1:
                        removeGui(pl);
                        break;
                }
            });
            lang.close();
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
                    std::string PlayerLanguage = tool::get(ori.getPlayer());
                    i18nLang lang("./plugins/LOICollection/language.json");
                    auto res = target.results(ori);
                    switch (op) {
                        case MUTEOP::add:
                            if (res.empty()) {
                                outp.error("Mute: No player selected.");
                                break;
                            }
                            for (auto& i : res) {
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
                                std::string log1 = lang.tr(PlayerLanguage, "mute.log1");
                                log1 = tool::replaceString(log1, "${cause}", MuteCause);
                                logger.info(LOICollectionAPI::translateString(log1, i, true));
                            }
                            outp.success("Mute: Adding players succeeded.");
                            break;
                        case MUTEOP::remove:
                            if (res.empty()) {
                                outp.error("Mute: No player selected.");
                                break;
                            }
                            for (auto& i : res) {
                                std::string xuid = i->getXuid();
                                if (db.existsTable("XUID" + xuid)) {
                                    db.removeTable("XUID" + xuid);
                                    logger.info(LOICollectionAPI::translateString(lang.tr(PlayerLanguage, "mute.log2"), i, true));
                                }
                            }
                            outp.success("Mute: The player silence status has been deleted.");
                            break;
                        case MUTEOP::gui: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Mute: No player selected.");
                                break;
                            }
                            menuGui(ori.getPlayer());
                            outp.success("The UI has been opened to player " + ori.getName());
                            break;
                        }
                        default:
                            logger.error("Mute >> 命令分支 " + std::to_string(op) + " 不存在");
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
                std::string PlayerLanguage = tool::get(e.mPlayer);
                i18nLang lang("./plugins/LOICollection/language.json");
                SQLiteDatabase db(PluginData + "/mute.db");
                if(db.existsTable("XUID" + xuid)) {
                    db.setTable("XUID" + xuid);
                    std::string timeString = db.get("Time");
                    std::string MuteCause = db.get("Cause");
                    if (tool::isReach(timeString)) {
                        db.removeTable("XUID" + xuid);
                        logger.info(LOICollectionAPI::translateString(lang.tr(PlayerLanguage, "mute.log2"), e.mPlayer, true));
                        lang.close();
                        db.close();
                        return true;
                    }
                    e.mPlayer->sendTextPacket(MuteCause);
                    std::string log3 = lang.tr(PlayerLanguage, "mute.log3");
                    log3 = tool::replaceString(log3, "${message}", e.mMessage);
                    logger.info(LOICollectionAPI::translateString(log3, e.mPlayer, true));
                    lang.close();
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
        if (!std::filesystem::exists(PluginData + "/mute.db")) {
            SQLiteDatabase db(PluginData + "/mute.db");
            db.close();
        }
        listen();
    }
}