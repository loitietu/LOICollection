#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <llapi/FormUI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Level.hpp>
#include "../Storage/SQLiteDatabase.h"
#include "../Storage/JsonManager.h"
#include "../utils/tool.h"
#include "../API.h"
#include "include/i18nLang.h"
#include "include/chat.h"
extern Logger logger;

namespace chat {
    namespace {
        void addManagerGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "chat.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "chat.gui.label")));
            form.append(Form::Input("input1", lang.tr(PlayerLanguage, "chat.gui.manager.add.input1"), "", "None"));
            form.append(Form::Input("input2", lang.tr(PlayerLanguage, "chat.gui.manager.add.input2"), "", "0"));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "chat.gui.manager.add.dropdown"), tool::getAllPlayerName()));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string title = mp["input1"]->getString();
                std::string playerName = mp["dropdown"]->getString();
                int time = tool::toInt(mp["input2"]->getString(), 0);
                Level::runcmdAs(pl, "chat add " + playerName + " " + title + " " + std::to_string(time));
            });
            lang.close();
        }

        void removeManagerGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "chat.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "chat.gui.label")));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "chat.gui.manager.remove.dropdown1"), tool::getAllPlayerName()));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                std::string PlayerLanguage = tool::get(pl);
                i18nLang lang("./plugins/LOICollection/language.json");
                if (mp.empty()) {
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                Player* player = tool::toNamePlayer(mp["dropdown"]->getString());
                tool::updateChat(player);
                SQLiteDatabase db(PluginData + "/chat.db");
                auto form = Form::CustomForm(lang.tr(PlayerLanguage, "chat.gui.title"));
                form.append(Form::Label("label", lang.tr(PlayerLanguage, "chat.gui.label")));
                form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "chat.gui.manager.remove.dropdown2"), db.listTable("XUID" + player->getXuid() + "TITLE")));
                form.sendTo(pl, [player](Player* pl2, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                    if (mp.empty()) {
                        std::string PlayerLanguage = tool::get(pl2);
                        i18nLang lang("./plugins/LOICollection/language.json");
                        pl2->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                        lang.close();
                        return;
                    }
                    Level::runcmdAs(pl2, "chat remove " + player->getName() + " " + mp["dropdown"]->getString());
                });
                lang.close();
                db.close();
            });
            lang.close();
        }

        void menuGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "chat.gui.title"), lang.tr(PlayerLanguage, "chat.gui.label"));
            form.addButton(lang.tr(PlayerLanguage, "chat.gui.manager.add"), "textures/ui/backup_replace");
            form.addButton(lang.tr(PlayerLanguage, "chat.gui.manager.remove"), "textures/ui/free_download_symbol");
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
                        addManagerGui(pl);
                        break;
                    case 1:
                        removeManagerGui(pl);
                        break;
                }
            });
            lang.close();
        }

        void settingTitleGui(Player* player) {
            SQLiteDatabase db(PluginData + "/chat.db");
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            std::string label = lang.tr(PlayerLanguage, "chat.gui.setTitle.label");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "chat.gui.title"));
            form.append(Form::Label("label", std::string(LOICollectionAPI::translateString(label, player, true))));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "chat.gui.setTitle.dropdown"), db.listTable("XUID" + player->getXuid() + "TITLE")));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty()) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string PlayerSelectTitle = mp["dropdown"]->getString();
                SQLiteDatabase db(PluginData + "/chat.db");
                db.setTable("XUID" + pl->getXuid());
                db.update("title", PlayerSelectTitle);
                db.close();
            });
            lang.close();
            db.close();
        }

        void settingGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "chat.gui.title"), lang.tr(PlayerLanguage, "chat.gui.label"));
            form.addButton(lang.tr(PlayerLanguage, "chat.gui.setTitle"), "textures/ui/backup_replace");
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
                        settingTitleGui(pl);
                        break;
                }
            });
            lang.close();
        }

        class ChatCommand : public Command {
            int time = 0;
            std::string title = "None";
            enum CHATOP : int {
                add = 1,
                remove = 2,
                gui = 3,
                list = 4,
                setting = 7
            } op;
            CommandSelector<Player> target;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    SQLiteDatabase db(PluginData + "/chat.db");
                    auto res = target.results(ori);
                    switch (op) {
                        case CHATOP::setting: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Chat: No player selected.");
                                break;
                            }
                            settingGui(ori.getPlayer());;
                            outp.success("The UI has been opened to player " + ori.getName());
                            break;
                        }
                        case CHATOP::gui: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Chat: No player selected.");
                                break;
                            }
                            if (!ori.getPlayer()->isOP()) {
                                outp.error("Chat: No permission to use this feature.");
                                return;
                            }
                            menuGui(ori.getPlayer());
                            outp.success("The UI has been opened to player " + ori.getName());
                            break;
                        }
                        case CHATOP::list: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Chat: No player selected.");
                                break;
                            }
                            std::vector<std::string> titleList = db.listTable("XUID" + ori.getPlayer()->getXuid() + "TITLE");
                            std::stringstream ss;
                            for (const auto& l : titleList) ss << l << ",";
                            std::string chatList = ss.str();
                            chatList.pop_back();
                            outp.success("Chat: Title list - " + chatList);
                            break;
                        }
                        case CHATOP::add: {
                            if (!ori.getPlayer()->isOP()) {
                                outp.error("Chat: No permission to use this feature.");
                                return;
                            }
                            for (auto& i : res) {
                                db.setTable("XUID" + i->getXuid() + "TITLE");
                                db.set(title, tool::timeCalculate(time));
                            }
                            outp.success("Chat: Adding players succeeded.");
                            break;
                        }
                        case CHATOP::remove: {
                            if (!ori.getPlayer()->isOP()) {
                                outp.error("Chat: No permission to use this feature.");
                                return;
                            }
                            for (auto& i : res) {
                                if (title != "None") {
                                    db.setTable("XUID" + i->getXuid() + "TITLE");
                                    if (db.exists(title)) {
                                        db.remove(title);
                                    }
                                    db.setTable("XUID" + i->getXuid());
                                    if (db.get("title") == title) {
                                        db.update("title", "None");
                                    }
                                }
                            }
                            outp.success("Chat: Removing players succeeded.");
                            break;
                        }
                        default:
                            logger.error("Chat >> 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Chat: Instruction error.");
                            break;
                    }
                    db.close();
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("chat", "§e§lLOICollection -> §b个人称号", CommandPermissionLevel::Any, {(CommandFlagValue)0},{(CommandFlagValue)0x80});
                    registry->addEnum<CHATOP>("add", {{"add", CHATOP::add}});
                    registry->addEnum<CHATOP>("remove", {{"remove", CHATOP::remove}});
                    registry->addEnum<CHATOP>("gui", {{"gui", CHATOP::gui}});
                    registry->addEnum<CHATOP>("list", {{"list", CHATOP::list}});
                    registry->addEnum<CHATOP>("setting", {{"setting", CHATOP::setting}});
                    registry->registerOverload<ChatCommand>("chat", makeMandatory<CommandParameterDataType::ENUM>(&ChatCommand::op, "op", "setting"));
                    registry->registerOverload<ChatCommand>("chat", makeMandatory<CommandParameterDataType::ENUM>(&ChatCommand::op, "op", "gui"));
                    registry->registerOverload<ChatCommand>("chat", makeMandatory<CommandParameterDataType::ENUM>(&ChatCommand::op, "op", "list"));
                    registry->registerOverload<ChatCommand>("chat", makeMandatory<CommandParameterDataType::ENUM>(&ChatCommand::op, "op", "add"), makeMandatory(&ChatCommand::target, "player"), makeMandatory(&ChatCommand::title, "title"));
                    registry->registerOverload<ChatCommand>("chat", makeMandatory<CommandParameterDataType::ENUM>(&ChatCommand::op, "op", "add"), makeMandatory(&ChatCommand::target, "player"), makeMandatory(&ChatCommand::title, "title"), makeMandatory(&ChatCommand::time, "time"));
                    registry->registerOverload<ChatCommand>("chat", makeMandatory<CommandParameterDataType::ENUM>(&ChatCommand::op, "op", "remove"), makeMandatory(&ChatCommand::target, "player"), makeMandatory(&ChatCommand::title, "title"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                ChatCommand::setup(e.mCommandRegistry);
                return true;
            });
            Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& e) {
                if (!tool::isBlacklist(e.mPlayer)) {
                    SQLiteDatabase db(PluginData + "/chat.db");
                    if (!db.existsTable("XUID" + e.mPlayer->getXuid())) {
                        db.setTable("XUID" + e.mPlayer->getXuid());
                        db.createTable();
                        db.set("title", "None");
                    }
                    if (!db.existsTable("XUID" + e.mPlayer->getXuid() + "TITLE")) {
                        db.setTable("XUID" + e.mPlayer->getXuid() + "TITLE");
                        db.createTable();
                        db.set("None", "0");
                    }
                    db.close();
                }
                return true;
            });
            Event::PlayerChatEvent::subscribe([](const Event::PlayerChatEvent& e) {
                if (!tool::isMute(e.mPlayer)) {
                    nlohmann::ordered_json data = tool::getJson("./plugins/LOICollection/config.json")["Chat"];
                    std::string MessageString = std::string(LOICollectionAPI::translateString(data["chat"], e.mPlayer, true));
                    MessageString = tool::replaceString(MessageString, "${chat}", e.mMessage);
                    Level::broadcastText(MessageString, TextType::SYSTEM);
                    data.clear();
                    return false;
                } else {
                    return true;
                }
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        if (!std::filesystem::exists(PluginData + "/chat.db")) {
            SQLiteDatabase db(PluginData + "/chat.db");
            db.close();
        }
        listen();
    }
}