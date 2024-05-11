#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/FormUI.h>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Player.hpp>
#include "../Storage/SQLiteDatabase.h"
#include "../utils/tool.h"
#include "../API.h"
#include "include/i18nLang.h"
#include "include/tpa.h"
extern Logger logger;

namespace tpa {
    namespace {
        bool getInvite(Player* player) {
            SQLiteDatabase db(PluginData + "/tpa.db");
            if (db.existsTable("XUID" + player->getXuid())) {
                db.setTable("XUID" + player->getXuid());
                bool result = db.get("Toggle1") == "true" ? true : false;
                db.close();
                return result;
            } else {
                db.close();
                return false;
            }
        }

        void menuGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "tpa.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "tpa.gui.label")));
            form.append(Form::Dropdown("dropdown1", lang.tr(PlayerLanguage, "tpa.gui.dropdown1"), tool::getAllPlayerName()));
            form.append(Form::Dropdown("dropdown2", lang.tr(PlayerLanguage, "tpa.gui.dropdown2"), { "tpa", "tphere" }));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string PlayerSelectType = mp["dropdown2"]->getString();
                Player* PlayerSelect = tool::toNamePlayer(mp["dropdown1"]->getString());
                if (!getInvite(PlayerSelect)) {
                    std::string PlayerLanguage = tool::get(PlayerSelect);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    auto form = Form::ModalForm(lang.tr(PlayerLanguage, "tpa.gui.title"), "", lang.tr(PlayerLanguage, "tpa.yes"), lang.tr(PlayerLanguage, "tpa.no"));
                    if (PlayerSelectType == "tpa") {
                        std::string contentString = lang.tr(PlayerLanguage, "tpa.there");
                        contentString = std::string(LOICollectionAPI::translateString(contentString, pl, true)); 
                        form.setContent(contentString);
                    } else {
                        std::string contentString = lang.tr(PlayerLanguage, "tpa.here");
                        contentString = std::string(LOICollectionAPI::translateString(contentString, pl, true));
                        form.setContent(contentString);
                    }
                    form.sendTo(PlayerSelect, [pl, PlayerSelectType](Player* pl2, bool isConfirm) {
                        std::string PlayerLanguage = tool::get(pl);
                        i18nLang lang("./plugins/LOICollection/language.json");
                        if (isConfirm) {
                            std::string log = lang.tr(PlayerLanguage, "tpa.log");
                            if (PlayerSelectType == "tpa") {
                                Level::runcmdEx("tp " + pl->getName() + " " + pl2->getName());
                                log = tool::replaceString(log, "${player1}", pl->getName());
                                log = tool::replaceString(log, "${player2}", pl2->getName());
                                logger.info(log);
                            } else {
                                Level::runcmdEx("tp " + pl2->getName() + " " + pl->getName());
                                log = tool::replaceString(log, "${player1}", pl2->getName());
                                log = tool::replaceString(log, "${player2}", pl->getName());
                                logger.info(log);
                            }
                            return;
                        } else {
                            pl->sendTextPacket(lang.tr(PlayerLanguage, "tpa.no.tips"));
                            lang.close();
                            return;
                        }
                    });
                    lang.close();
                } else {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "tpa.no.tips"));
                    lang.close();
                    return;
                }
            });
            lang.close();
        }

        void settingGui(Player* player) {
            SQLiteDatabase db(PluginData + "/tpa.db");
            db.setTable("XUID" + player->getXuid());
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "tpa.gui.setting.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "tpa.gui.setting.label")));
            form.append(Form::Toggle("Toggle1", lang.tr(PlayerLanguage, "tpa.gui.setting.switch1"), db.get("Toggle1") == "true" ? true : false));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                SQLiteDatabase db(PluginData + "/tpa.db");
                db.setTable("XUID" + pl->getXuid());
                db.update("Toggle1", mp["Toggle1"]->getBool() ? "true" : "false");
                db.close();
            });
            lang.close();
            db.close();
        }

        class TpaCommand : public Command {
            enum TPAOP : int {
                gui = 3,
                setting = 7
            } op;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    switch (op) {
                        case TPAOP::gui: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Tpa: No player selected.");
                                break;
                            }
                            menuGui(ori.getPlayer());
                            outp.success("The UI has been opened to player " + ori.getName());
                            break;
                        }
                        case TPAOP::setting: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Tpa: No player selected.");
                                break;
                            }
                            settingGui(ori.getPlayer());
                            outp.success("The UI has been opened to player " + ori.getName());
                            break;
                        }
                        default:
                            logger.error("Tpa >> 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Tpa: Instruction error.");
                            break;
                    }
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("tpa", "§e§lLOICollection -> §b玩家互传", CommandPermissionLevel::Any, {(CommandFlagValue)0},{(CommandFlagValue)0x80});
                    registry->addEnum<TPAOP>("gui", {{"gui", TPAOP::gui}});
                    registry->addEnum<TPAOP>("setting", {{"setting", TPAOP::setting}});
                    registry->registerOverload<TpaCommand>("tpa", makeMandatory<CommandParameterDataType::ENUM>(&TpaCommand::op, "op", "gui"));
                    registry->registerOverload<TpaCommand>("tpa", makeMandatory<CommandParameterDataType::ENUM>(&TpaCommand::op, "op", "setting"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                TpaCommand::setup(e.mCommandRegistry);
                return true;
            });
            Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& e) {
                if (!tool::isBlacklist(e.mPlayer)) {
                    SQLiteDatabase db(PluginData + "/tpa.db");
                    if (!db.existsTable("XUID" + e.mPlayer->getXuid())) {
                        db.setTable("XUID" + e.mPlayer->getXuid());
                        db.createTable();
                        db.set("Toggle1", "false");
                    }
                    db.close();
                }
                return true;
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        if (!std::filesystem::exists(PluginData + "/tpa.db")) {
            SQLiteDatabase db(PluginData + "/tpa.db");
            db.close();
        }
        listen();
    }
}