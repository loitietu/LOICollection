#include <string>
#include <fstream>
#include <filesystem>
#include <llapi/EventAPI.h>
#include <llapi/FormUI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/ServerPlayer.hpp>
#include <Nlohmann/json.hpp>
#include "../Storage/JsonManager.h"
#include "../tool.h"
#include "include/i18nLang.h"
#include "include/announcement.h"
extern Logger logger;

namespace announcement {
    namespace {
        void menuGui(Player* player) {
            JsonManager database(PluginData + "/announcement.json");
            std::string title = database.get("title").template get<std::string>();
            nlohmann::ordered_json data = database.get("content");
            database.clear();
            int index = 1;
            auto form = Form::CustomForm(title);
            for (nlohmann::ordered_json& content : data) {
                form.append(Form::Label("label" + std::to_string(index), content));
                index++;
            }
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                std::string PlayerLanguage = tool::get(pl);
                i18nLang lang("./plugins/LOICollection/language.json");
                pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                lang.close();
                return;
            });            
        }

        void settingGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            JsonManager database(PluginData + "/announcement.json");
            std::string title = database.get("title").template get<std::string>();
            nlohmann::ordered_json data = database.get("content");
            database.clear();
            int index = 1;
            std::string titleContent = lang.tr(PlayerLanguage, "announcement.gui.line");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "announcement.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "announcement.gui.label")));
            form.append(Form::Input("input", lang.tr(PlayerLanguage, "announcement.gui.setTitle"), "", title));
            for (nlohmann::ordered_json& content : data) {
                form.append(Form::Input("input" + std::to_string(index), tool::replaceString(titleContent, "${index}", std::to_string(index)), "", content));
                index++;
            }
            form.append(Form::Toggle("toggle1", lang.tr(PlayerLanguage, "announcement.gui.addLine")));
            form.append(Form::Toggle("toggle2", lang.tr(PlayerLanguage, "announcement.gui.removeLine")));
            lang.close();
            data.clear();
            form.sendTo(player, [index](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                bool addLine = mp["toggle1"]->getBool();
                bool removeLine = mp["toggle2"]->getBool();
                std::string title = mp["input"]->getString();
                JsonManager database(PluginData + "/announcement.json");
                nlohmann::ordered_json data = database.get("content");
                database.clear();
                if (addLine) {
                    data.push_back("");
                    nlohmann::ordered_json dataNew = {
                        {"title", title},
                        {"content", data}
                    };
                    std::ofstream databaseNewFile(PluginData + "/announcement.json");
                    databaseNewFile << dataNew.dump(4);
                    databaseNewFile.close();
                    dataNew.clear();
                    data.clear();
                    settingGui(pl);
                } else if (removeLine) {
                    data.erase(data.begin());
                    nlohmann::ordered_json dataNew = {
                        {"title", title},
                        {"content", data}
                    };
                    std::ofstream databaseNewFile(PluginData + "/announcement.json");
                    databaseNewFile << dataNew.dump(4);
                    databaseNewFile.close();
                    dataNew.clear();
                    data.clear();
                    settingGui(pl);
                } else {
                    nlohmann::ordered_json dataNewList;
                    for (int i = 1; i < index; i++) dataNewList.push_back(mp["input" + std::to_string(i)]->getString());
                    nlohmann::ordered_json dataNew = {
                        {"title", title},
                        {"content", dataNewList}
                    };
                    std::ofstream databaseNewFile(PluginData + "/announcement.json");
                    databaseNewFile << dataNew.dump(4);
                    databaseNewFile.close();
                    dataNewList.clear();
                    dataNew.clear();
                    data.clear();
                }
            }); 
        }

        class AnnounCementCommand : public Command {
            enum ANNOUNCEMENTOP : int {
                gui = 3,
                setting = 7
            } op;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    switch (op) {
                        case ANNOUNCEMENTOP::gui: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("AnnounCement: No player selected.");
                                break;
                            }
                            menuGui(tool::toServerPlayer(ori.getPlayer()));
                            std::string playerName = ori.getName();
                            outp.success("The UI has been opened to player " + playerName);
                            break;
                        }
                        case ANNOUNCEMENTOP::setting: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("AnnounCement: No player selected.");
                                break;
                            }
                            if (ori.getPlayer()->isOP()) {
                                settingGui(tool::toServerPlayer(ori.getPlayer()));
                                std::string playerName = ori.getName();
                                outp.success("The UI has been opened to player " + playerName);
                            } else {
                                outp.error("AnnounCement: No permission to open the Setting.");
                            }
                            break;
                        }
                        default:
                            logger.error("<AnnounCement>: 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("AnnounCement: Instruction error.");
                            break;
                    }
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("announcement", "§e§lLOICollection -> §a公告系统", CommandPermissionLevel::Any, {(CommandFlagValue)0},{(CommandFlagValue)0x80});
                    registry->addEnum<ANNOUNCEMENTOP>("gui", {{"gui", ANNOUNCEMENTOP::gui}});
                    registry->addEnum<ANNOUNCEMENTOP>("setting", {{"setting", ANNOUNCEMENTOP::setting}});
                    registry->registerOverload<AnnounCementCommand>("announcement", makeMandatory<CommandParameterDataType::ENUM>(&AnnounCementCommand::op, "op", "gui"));
                    registry->registerOverload<AnnounCementCommand>("announcement", makeMandatory<CommandParameterDataType::ENUM>(&AnnounCementCommand::op, "op", "setting"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                AnnounCementCommand::setup(e.mCommandRegistry);
                return true;
            });
            Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& e) {
                if (!tool::isBlacklist(e.mPlayer)) {
                    menuGui(e.mPlayer);
                }
                return true;
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        if (!std::filesystem::exists(PluginData + "/announcement.json")) {
            nlohmann::ordered_json contentArray = nlohmann::ordered_json::array();
            contentArray.push_back("这是一条测试公告，欢迎使用本插件！");
            JsonManager database(PluginData + "/announcement.json");
            database.set("title", "测试公告");
            database.set("content", contentArray);
            database.save();
            contentArray.clear();
        }
        listen();
    }
}