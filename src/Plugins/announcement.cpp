#include <string>
#include <fstream>
#include <filesystem>
#include <llapi/EventAPI.h>
#include <llapi/FormUI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/mc/Player.hpp>
#include <Nlohmann/json.hpp>
#include "../Storage/JsonManager.h"
#include "../utils/tool.h"
#include "include/i18nLang.h"
#include "include/announcement.h"
extern Logger logger;

namespace announcement {
    namespace {
        void menuGui(Player* player) {
            int index = 1;
            JsonManager database(PluginData + "/announcement.json");
            auto form = Form::CustomForm(database.get("title").template get<std::string>());
            for (nlohmann::ordered_json& content : database.get("content")) {
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
            database.clear();    
        }

        void settingGui(Player* player) {
            int index = 1;
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            JsonManager database(PluginData + "/announcement.json");
            std::string titleContent = lang.tr(PlayerLanguage, "announcement.gui.line");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "announcement.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "announcement.gui.label")));
            form.append(Form::Input("input", lang.tr(PlayerLanguage, "announcement.gui.setTitle"), "", database.get("title").template get<std::string>()));
            for (nlohmann::ordered_json& content : database.get("content")) {
                form.append(Form::Input("input" + std::to_string(index), tool::replaceString(titleContent, "${index}", std::to_string(index)), "", content));
                index++;
            }
            form.append(Form::Toggle("toggle1", lang.tr(PlayerLanguage, "announcement.gui.addLine")));
            form.append(Form::Toggle("toggle2", lang.tr(PlayerLanguage, "announcement.gui.removeLine")));
            form.sendTo(player, [index](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string title = mp["input"]->getString();
                JsonManager database(PluginData + "/announcement.json");
                nlohmann::ordered_json data = database.get("content");
                if (mp["toggle1"]->getBool()) {
                    data.push_back("");
                    database.setString("title", title);
                    database.set("content", data);
                    database.save();
                    settingGui(pl);
                } else if (mp["toggle2"]->getBool()) {
                    data.erase(data.end() - 1);
                    database.setString("title", title);
                    database.set("content", data);
                    database.save();
                    settingGui(pl);
                } else {
                    nlohmann::ordered_json dataNewList;
                    for (int i = 1; i < index; i++) {
                        dataNewList.push_back(mp["input" + std::to_string(i)]->getString());
                    }
                    database.setString("title", title);
                    database.set("content", dataNewList);
                    database.save();
                    dataNewList.clear();
                }
                data.clear();
            });
            database.clear();
            lang.close();
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
                            menuGui(ori.getPlayer());
                            outp.success("The UI has been opened to player " + ori.getName());
                            break;
                        }
                        case ANNOUNCEMENTOP::setting: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("AnnounCement: No player selected.");
                                break;
                            }
                            if (ori.getPlayer()->isOP()) {
                                settingGui(ori.getPlayer());
                                outp.success("The UI has been opened to player " + ori.getName());
                            } else {
                                outp.error("AnnounCement: No permission to open the Setting.");
                            }
                            break;
                        }
                        default:
                            logger.error("AnnounCement >> 命令分支 " + std::to_string(op) + " 不存在");
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