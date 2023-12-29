#include <filesystem>
#include <llapi/FormUI.h>
#include <llapi/EventAPI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Scoreboard.hpp>
#include <Nlohmann/json.hpp>
#include "../tool.h"
#include "../Storage/SQLiteDatabase.h"
#include "include/i18nLang.h"
#include "include/cdk.h"
extern Logger logger;

namespace cdk {
    namespace {
        void database() {
            if (!std::filesystem::exists(PluginData + "/cdk.db")) {
                SQLiteDatabase db(PluginData + "/cdk.db");
                db.close();
            }
        }

        void cdkConvert(ServerPlayer* player, const std::string& convertString) {
            std::string PlayerLanguage = tool::get(player);
            SQLiteDatabase db(PluginData + "/cdk.db");
            i18nLang lang("./plugins/LOICollection/language.json");
            if (db.existsTable(convertString)) {
                db.setTable(convertString);
                nlohmann::ordered_json PlayerList(db.get("players"));
                if (PlayerList.contains(player->getXuid())) {
                    player->sendTextPacket(lang.tr(PlayerLanguage, "cdk.convert.tip2"));
                    lang.close();
                    db.close();
                    return;
                }
                PlayerList.push_back(player->getXuid());
                db.set("players", PlayerList.dump());
                tool::llmoney::add(player, std::stoi(db.get("llmoney")));
                nlohmann::ordered_json ScoreboardList(db.get("scores"));
                for (nlohmann::ordered_json::iterator it = ScoreboardList.begin(); it != ScoreboardList.end(); ++it) Scoreboard::addScore(player, it.key(), ScoreboardList[it.key()]);
                player->sendTextPacket(lang.tr(PlayerLanguage, "cdk.convert.tip3"));
                lang.close();
                db.close();
            } else {
                player->sendTextPacket(lang.tr(PlayerLanguage, "cdk.convert.tip1"));
                lang.close();
                db.close();
            }
        }

        void cdkGui(ServerPlayer* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "cdk.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "cdk.gui.label")));
            form.append(Form::Input("input", lang.tr(PlayerLanguage, "cdk.gui.convert.input")));
            lang.close();
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string cdk = mp["input"]->getString();
                pl->runcmd("cdk convert " + cdk);
            });
        }

        void cdkSetting(ServerPlayer* player) {
            
        }

        class CdkCommand : public Command {
            std::string cdkString = "";
            int cdkInt = 0;
            bool setting = false;
            enum CDKOP : int {
                gui = 3,
                convert = 5
            } op;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    switch (op) {
                        case CDKOP::gui: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Cdk: No player selected.");
                                break;
                            }
                            if (setting && ori.getPlayer()->isOP()) cdkSetting(ori.getPlayer());
                            else if(setting && !ori.getPlayer()->isOP()) outp.error("Cdk: No permission to open the Setting.");
                            else cdkGui(ori.getPlayer());
                            std::string playerName = ori.getName();
                            outp.success("The UI has been opened to player " + playerName);
                            break;
                        }
                        case CDKOP::convert: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Cdk: No player selected.");
                                break;
                            }
                            std::string cdk = cdkString;
                            if (cdkString.empty()) cdk = std::to_string(cdkInt);
                            std::string playerName = ori.getName();
                            outp.success("The player " + playerName + " has been converted to cdk: " + cdk);
                            cdkConvert(ori.getPlayer(), cdk);
                            break;
                        }
                        default:
                            logger.error("<Cdk>: 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Cdk: Instruction error.");
                            break;
                    }
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("cdk", "§e§lLOICollection -> §b总换码", CommandPermissionLevel::Any, {(CommandFlagValue)0},{(CommandFlagValue)0x80});
                    registry->addEnum<CDKOP>("gui", {{"gui", CDKOP::gui}});
                    registry->addEnum<CDKOP>("convert", {{"convert", CDKOP::convert}});
                    registry->registerOverload<CdkCommand>("cdk", makeMandatory<CommandParameterDataType::ENUM>(&CdkCommand::op, "op", "gui"));
                    registry->registerOverload<CdkCommand>("cdk", makeMandatory<CommandParameterDataType::ENUM>(&CdkCommand::op, "op", "gui"), makeMandatory(&CdkCommand::setting, "setting"));
                    registry->registerOverload<CdkCommand>("cdk", makeMandatory<CommandParameterDataType::ENUM>(&CdkCommand::op, "op", "convert"), makeMandatory(&CdkCommand::cdkString, "cdk"));
                    registry->registerOverload<CdkCommand>("cdk", makeMandatory<CommandParameterDataType::ENUM>(&CdkCommand::op, "op", "convert"), makeMandatory(&CdkCommand::cdkInt, "cdkInt"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                CdkCommand::setup(e.mCommandRegistry);
                return true;
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        database();
        listen();
        logger.info("<cdk>: 插件已加载");
    }
}