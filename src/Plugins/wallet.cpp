#include <string>
#include <vector>
#include <unordered_map>
#include <llapi/EventAPI.h>
#include <llapi/FormUI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Scoreboard.hpp>
#include <Nlohmann/json.hpp>
#include "../utils/tool.h"
#include "../API.h"
#include "include/i18nLang.h"
#include "include/wallet.h"
extern Logger logger;

namespace wallet {
    namespace {
        void transfer(Player* player) {
            nlohmann::ordered_json data = tool::getJson("./plugins/LOICollection/config.json")["Wallet"];
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            std::string ScoreboardID = data["score"];
            std::string labelString = std::string(LOICollectionAPI::translateString(lang.tr(PlayerLanguage, "wallet.gui.label"), player, true));
            labelString = tool::replaceString(labelString, "${tax}", std::to_string((float) data["tax"]));
            if ((bool) data["llmoney"]) {
                labelString = tool::replaceString(labelString, "${money}", std::to_string(tool::llmoney::get(player)));
            } else {
                Scoreboard::newObjective(ScoreboardID, "");
                player->addScore(ScoreboardID, 0);
                labelString = tool::replaceString(labelString, "${money}", std::to_string(player->getScore(ScoreboardID)));
            }
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "wallet.gui.title"));
            form.append(Form::Label("label", labelString));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "wallet.gui.stepslider.dropdown"), tool::getAllPlayerName()));
            form.append(Form::Input("input", lang.tr(PlayerLanguage, "wallet.gui.stepslider.input"), "", "100"));
            form.sendTo(player, [data, ScoreboardID](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                std::string PlayerLanguage = tool::get(pl);
                i18nLang lang("./plugins/LOICollection/language.json");
                if (mp.empty()) {
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                int money = tool::toInt(mp["input"]->getString(), 0);
                int moneys = (money - money * (float) data["tax"]);
                if (moneys < 0) moneys = (moneys * -1);
                Player* PlayerSelect = tool::toNamePlayer(mp["dropdown"]->getString());
                if (tool::llmoney::get(pl) >= money && (bool) data["llmoney"]) {
                    tool::llmoney::reduce(pl, money);
                    tool::llmoney::add(PlayerSelect, moneys);
                } else if (pl->getScore(ScoreboardID) >= money && !(bool) data["llmoney"]) {
                    pl->reduceScore(ScoreboardID, money);
                    PlayerSelect->addScore(ScoreboardID, moneys);
                }
                std::string log = lang.tr(PlayerLanguage, "wallet.log");
                log = tool::replaceString(log, "${player1}", pl->getName());
                log = tool::replaceString(log, "${player2}", PlayerSelect->getName());
                log = tool::replaceString(log, "${money}", std::to_string(money));
                logger.info(log);
                lang.close();
            });
            lang.close();
            data.clear();
        }

        void wealth(Player* player) {
            nlohmann::ordered_json data = tool::getJson("./plugins/LOICollection/config.json")["Wallet"];
            i18nLang lang("./plugins/LOICollection/language.json");
            std::string wealthString = lang.tr(tool::get(player), "wallet.showOff");
            wealthString = std::string(LOICollectionAPI::translateString(wealthString, player, true));
            wealthString = tool::replaceString(wealthString, "${tax}", std::to_string((float) data["tax"]));
            if ((bool) data["llmoney"]) {
                wealthString = tool::replaceString(wealthString, "${money}", std::to_string(tool::llmoney::get(player)));
            } else {
                std::string ScoreboardID = data["score"];
                Scoreboard::newObjective(ScoreboardID, "");
                player->addScore(ScoreboardID, 0);
                wealthString = tool::replaceString(wealthString, "${money}", std::to_string(player->getScore(ScoreboardID)));
            }
            Level::broadcastText(wealthString, TextType::SYSTEM);
            data.clear();
            lang.close();
        }

        void menuGui(Player* player) {
            nlohmann::ordered_json data = tool::getJson("./plugins/LOICollection/config.json")["Wallet"];
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            std::string labelString = lang.tr(PlayerLanguage, "wallet.gui.label");
            labelString = std::string(LOICollectionAPI::translateString(labelString, player, true));
            labelString = tool::replaceString(labelString, "${tax}", std::to_string((float) data["tax"]));
            if ((bool) data["llmoney"]) {
                labelString = tool::replaceString(labelString, "${money}", std::to_string(tool::llmoney::get(player)));
            } else {
                std::string ScoreboardID = data["score"];
                Scoreboard::newObjective(ScoreboardID, "");
                player->addScore(ScoreboardID, 0);
                labelString = tool::replaceString(labelString, "${money}", std::to_string(player->getScore(ScoreboardID)));
            }
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "wallet.gui.title"));
            form.append(Form::Label("label", labelString));
            form.append(Form::StepSlider("stepslider", lang.tr(PlayerLanguage, "wallet.gui.stepslider"), { "transfer", "wealth" }));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string id = mp["stepslider"]->getString();
                if (id == "transfer") {
                    transfer(pl);
                } else if (id == "wealth") {
                    wealth(pl);
                }
            });
            lang.close();
            data.clear();
        }

        class WalletCommand : public Command {
            enum WALLETOP : int {
                gui = 3
            } op;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    switch (op) {
                        case WALLETOP::gui: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Wallet: No player selected.");
                                break;
                            }
                            menuGui(ori.getPlayer());
                            outp.success("The UI has been opened to player " + ori.getName());
                            break;
                        }
                        default:
                            logger.error("Wallet >> 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Wallet: Instruction error.");
                            break;
                    }
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("wallet", "§e§lLOICollection -> §b个人钱包", CommandPermissionLevel::Any, {(CommandFlagValue)0},{(CommandFlagValue)0x80});
                    registry->addEnum<WALLETOP>("gui", {{"gui", WALLETOP::gui}});
                    registry->registerOverload<WalletCommand>("wallet", makeMandatory<CommandParameterDataType::ENUM>(&WalletCommand::op, "op", "gui"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                WalletCommand::setup(e.mCommandRegistry);
                return true;
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        listen();
    }
}