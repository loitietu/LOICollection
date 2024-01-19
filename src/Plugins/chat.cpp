#include <string>
#include <vector>
#include <filesystem>
#include <llapi/FormUI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/ServerPlayer.hpp>
#include "../Storage/SQLiteDatabase.h"
#include "../Storage/JsonManager.h"
#include "../tools/tool.h"
#include "../API.h"
#include "include/i18nLang.h"
#include "include/chat.h"
extern Logger logger;

namespace chat {
    namespace {
        void settingTitleGui(Player* player) {
            SQLiteDatabase db(PluginData + "/chat.db");
            std::vector<std::string> titleList = db.listTabe("XUID" + player->getXuid() + "TITLE");
            db.close();
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            std::string label = lang.tr(PlayerLanguage, "chat.gui.setTitle.label");
            label = std::string(LOICollectionAPI::translateString(label, player, true));
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "chat.gui.title"));
            form.append(Form::Label("label", label));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "chat.gui.setTitle.dropdown"), titleList));
            lang.close();
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
        }

        void settingGui(ServerPlayer* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "chat.gui.title"), lang.tr(PlayerLanguage, "chat.gui.label"));
            form.addButton(lang.tr(PlayerLanguage, "chat.gui.setTitle"), "textures/ui/backup_replace");
            lang.close();
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
        }

        class ChatCommand : public Command {
            enum CHATOP : int {
                setting = 7
            } op;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    switch (op) {
                        case CHATOP::setting: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Chat: No player selected.");
                                break;
                            }
                            settingGui(ori.getPlayer());
                            std::string playerName = ori.getName();
                            outp.success("The UI has been opened to player " + playerName);
                            break;
                        }
                        default:
                            logger.error("<Chat>: 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Chat: Instruction error.");
                            break;
                    }
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("chat", "§e§lLOICollection -> §b个人称号", CommandPermissionLevel::Any, {(CommandFlagValue)0},{(CommandFlagValue)0x80});
                    registry->addEnum<CHATOP>("setting", {{"setting", CHATOP::setting}});
                    registry->registerOverload<ChatCommand>("chat", makeMandatory<CommandParameterDataType::ENUM>(&ChatCommand::op, "op", "setting"));
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
                        db.set("None", "true");
                    }
                    db.close();
                }
                return true;
            });
            Event::PlayerChatEvent::subscribe([](const Event::PlayerChatEvent& e) {
                if (!tool::isMute(e.mPlayer)) {
                    JsonManager config("./plugins/LOICollection/config.json");
                    nlohmann::ordered_json data = config.get("Chat");
                    config.clear();
                    std::string MessageString = data["chat"];
                    MessageString = std::string(LOICollectionAPI::translateString(MessageString, e.mPlayer, true));
                    MessageString = tool::replaceString(MessageString, "${chat}", e.mMessage);
                    tool::BroadcastText(MessageString);
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