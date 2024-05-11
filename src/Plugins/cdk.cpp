#include <filesystem>
#include <llapi/FormUI.h>
#include <llapi/EventAPI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/mc/Scoreboard.hpp>
#include <Nlohmann/json.hpp>
#include "../API.h"
#include "../utils/tool.h"
#include "../Storage/JsonManager.h"
#include "include/i18nLang.h"
#include "include/cdk.h"
extern Logger logger;

namespace cdk {
    namespace {
        void cdkConvert(Player* player, const std::string& convertString) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            JsonManager database(PluginData + "/cdk.json");
            std::string log3 = lang.tr(PlayerLanguage, "cdk.log3");
            log3 = tool::replaceString(log3, "${cdk}", convertString);
            logger.info(LOICollectionAPI::translateString(log3, player, true));
            if (database.isKey(convertString)) {
                nlohmann::ordered_json cdkJson(database.get(convertString));
                if (cdkJson.contains("time")) {
                    std::string time = cdkJson["time"].template get<std::string>();
                    if (tool::isReach(time)) {
                        database.remove(convertString);
                        player->sendTextPacket(lang.tr(PlayerLanguage, "cdk.convert.tip1"));
                        lang.close();
                        database.save();
                        return;
                    }
                }
                nlohmann::ordered_json PlayerList = cdkJson.at("player");
                if (tool::isJsonArrayFind(PlayerList, player->getXuid())) {
                    player->sendTextPacket(lang.tr(PlayerLanguage, "cdk.convert.tip2"));
                    lang.close();
                    database.clear();
                    return;
                }
                tool::llmoney::add(player, cdkJson["llmoney"]);
                nlohmann::ordered_json ScoreboardList = cdkJson.at("scores");
                nlohmann::ordered_json ItemList = cdkJson.at("item");
                for (nlohmann::ordered_json::iterator it = ScoreboardList.begin(); it != ScoreboardList.end(); ++it) {
                    int score = ScoreboardList[it.key()].template get<int>();
                    Scoreboard::newObjective(it.key(), "");
                    player->addScore(it.key(), score);
                }
                for (nlohmann::ordered_json::iterator it = ItemList.begin(); it != ItemList.end(); ++it) {
                    auto* item = ItemStack::create(it.key(), it.value()["quantity"]);
                    item->setAuxValue(it.value()["specialvalue"]);
                    item->setCustomName(it.value()["name"]);
                    player->giveItem(item);
                    delete item;
                }
                if (cdkJson["personal"]) database.remove(convertString);
                else {
                    PlayerList.push_back(player->getXuid());
                    cdkJson["player"] = PlayerList;
                    database.set(convertString, cdkJson);
                }
                player->sendTextPacket(lang.tr(PlayerLanguage, "cdk.convert.tip3"));
                lang.close();
                database.save();
                return;
            } else {
                player->sendTextPacket(lang.tr(PlayerLanguage, "cdk.convert.tip1"));
                lang.close();
                database.clear();
                return;
            }
        }

        void cdkGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "cdk.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "cdk.gui.label")));
            form.append(Form::Input("input", lang.tr(PlayerLanguage, "cdk.gui.convert.input"), "", "convert"));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string cdk = mp["input"]->getString();
                Level::runcmdAs(pl, "cdk convert " + cdk);
            });
            lang.close();
        }

        void cdkNewGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "cdk.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "cdk.gui.label")));
            form.append(Form::Input("input1", lang.tr(PlayerLanguage, "cdk.gui.new.input1"), "", "cdk"));
            form.append(Form::Toggle("Toggle", lang.tr(PlayerLanguage, "cdk.gui.new.switch")));
            form.append(Form::Input("input2", lang.tr(PlayerLanguage, "cdk.gui.new.input2"), "", "100"));
            form.append(Form::Input("input3", lang.tr(PlayerLanguage, "cdk.gui.new.input3"), "", "0"));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                std::string PlayerLanguage = tool::get(pl);
                i18nLang lang("./plugins/LOICollection/language.json");
                if (mp.empty()) {
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                JsonManager database(PluginData + "/cdk.json");
                nlohmann::ordered_json emptyArray = nlohmann::ordered_json::array();
                nlohmann::ordered_json emptyObject = nlohmann::ordered_json::object();
                int money = tool::toInt(mp["input2"]->getString(), 0);
                int time = tool::toInt(mp["input3"]->getString(), 0);
                std::string cdkName = mp["input1"]->getString();
                std::string timeString = tool::timeCalculate(time);
                nlohmann::ordered_json dataList = {
                    {"personal", mp["Toggle"]->getBool()},
                    {"player", emptyArray},
                    {"scores", emptyObject},
                    {"item", emptyObject},
                    {"llmoney", money},
                    {"time", timeString}
                };
                std::string log1 = lang.tr(PlayerLanguage, "cdk.log1");
                log1 = tool::replaceString(log1, "${cdk}", cdkName);
                logger.info(LOICollectionAPI::translateString(log1, pl, true));
                database.set(cdkName, dataList);
                database.save();
                lang.close();
            });
            lang.close();
        }

        void cdkRemoveGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            JsonManager database(PluginData + "/cdk.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "cdk.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "cdk.gui.label")));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "cdk.gui.remove.dropdown"), database.list()));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                std::string PlayerLanguage = tool::get(pl);
                i18nLang lang("./plugins/LOICollection/language.json");
                if (mp.empty()) {
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string cdkName = mp["dropdown"]->getString();
                JsonManager database(PluginData + "/cdk.json");
                database.remove(cdkName);
                database.save();
                std::string log2 = lang.tr(PlayerLanguage, "cdk.log2");
                log2 = tool::replaceString(log2, "${cdk}", cdkName);
                logger.info(LOICollectionAPI::translateString(log2, pl, true));
                lang.close();
            });
            database.clear();
            lang.close();
        }

        void cdkAwardScoreGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            JsonManager database(PluginData + "/cdk.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "cdk.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "cdk.gui.label")));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "cdk.gui.award.dropdown"), database.list()));
            form.append(Form::Input("input1", lang.tr(PlayerLanguage, "cdk.gui.award.score.input1"), "", "money"));
            form.append(Form::Input("input2", lang.tr(PlayerLanguage, "cdk.gui.award.score.input2"), "", "100"));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                int money = tool::toInt(mp["input2"]->getString(), 0);
                JsonManager database(PluginData + "/cdk.json");
                nlohmann::ordered_json cdkData = database.get(mp["dropdown"]->getString());
                cdkData["scores"][mp["input1"]->getString()] = money;
                database.set(mp["dropdown"]->getString(), cdkData);
                database.save();
            });
            database.clear();
            lang.close();
        }

        void cdkAwardItemGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            JsonManager database(PluginData + "/cdk.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "cdk.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "cdk.gui.label")));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "cdk.gui.award.dropdown"), database.list()));
            form.append(Form::Input("input1", lang.tr(PlayerLanguage, "cdk.gui.award.item.input1"), "", "minecraft:apple"));
            form.append(Form::Input("input2", lang.tr(PlayerLanguage, "cdk.gui.award.item.input2"), "", "apple"));
            form.append(Form::Input("input3", lang.tr(PlayerLanguage, "cdk.gui.award.item.input3"), "", "1"));
            form.append(Form::Input("input4", lang.tr(PlayerLanguage, "cdk.gui.award.item.input4"), "", "0"));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                int count = tool::toInt(mp["input3"]->getString(), 1);
                int aux = tool::toInt(mp["input4"]->getString(), 0);
                JsonManager database(PluginData + "/cdk.json");
                nlohmann::ordered_json cdkData = database.get(mp["dropdown"]->getString());
                cdkData["item"][mp["input1"]->getString()] = {
                    {"name", mp["input2"]->getString()},
                    {"quantity", count},
                    {"specialvalue", aux}
                };
                database.set(mp["dropdown"]->getString(), cdkData);
                database.save();
            });
            database.clear();
            lang.close();
        }

        void cdkAwardGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "cdk.gui.title"), lang.tr(PlayerLanguage, "cdk.gui.label"));
            form.addButton(lang.tr(PlayerLanguage, "cdk.gui.award.score"), "textures/items/diamond_sword");
            form.addButton(lang.tr(PlayerLanguage, "cdk.gui.award.item"), "textures/items/diamond");
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
                        cdkAwardScoreGui(pl);
                        break;
                    case 1:
                        cdkAwardItemGui(pl);
                        break;
                }
            });
            lang.close();
        }

        void cdkSetting(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "cdk.gui.title"), lang.tr(PlayerLanguage, "cdk.gui.label"));
            form.addButton(lang.tr(PlayerLanguage, "cdk.gui.addCdk"), "textures/ui/book_addtextpage_default");
            form.addButton(lang.tr(PlayerLanguage, "cdk.gui.removeCdk"), "textures/ui/cancel");
            form.addButton(lang.tr(PlayerLanguage, "cdk.gui.addAward"), "textures/ui/color_picker");
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
                        cdkNewGui(pl);
                        break;
                    case 1:
                        cdkRemoveGui(pl);
                        break;
                    case 2:
                        cdkAwardGui(pl);
                        break;
                }
            });
            lang.close();
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
                            if (setting && ori.getPlayer()->isOP()) {
                                outp.success("Cdk: The UI has been opened to player " + ori.getName());
                                cdkSetting(ori.getPlayer());
                            } else if(setting && !ori.getPlayer()->isOP()) {
                                outp.error("Cdk: No permission to open the Setting.");
                            } else {
                                outp.success("Cdk: The UI has been opened to player " + ori.getName());
                                cdkGui(ori.getPlayer());
                            }
                            break;
                        }
                        case CDKOP::convert: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Cdk: No player selected.");
                                break;
                            }
                            std::string cdk = cdkString;
                            if (cdkString.empty()) cdk = std::to_string(cdkInt);
                            outp.success("Cdk: The player " + ori.getName() + " has been converted to cdk: " + cdk);
                            cdkConvert(ori.getPlayer(), cdk);
                            break;
                        }
                        default:
                            logger.error("Cdk >> 命令分支 " + std::to_string(op) + " 不存在");
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
        JsonManager database(PluginData + "/cdk.json");
        database.save();
        listen();
    }
}