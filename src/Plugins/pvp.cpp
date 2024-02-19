#include <string>
#include <llapi/FormUI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/mc/Player.hpp>
#include "../Storage/SQLiteDatabase.h"
#include "../tools/tool.h"
#include "include/i18nLang.h"
#include "include/pvp.h"
extern Logger logger;

namespace pvp {
    namespace {
        bool getEnable(Player* player) {
            SQLiteDatabase db(PluginData + "/pvp.db");
            if (db.existsTable("XUID" + player->getXuid())) {
                db.setTable("XUID" + player->getXuid());
                bool result = db.get("enable") == "true" ? true : false;
                db.close();
                return result;
            }
            db.close();
            return false;
        }

        void menuGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "pvp.gui.title"), lang.tr(PlayerLanguage, "pvp.gui.label"));
            form.addButton(lang.tr(PlayerLanguage, "pvp.gui.on"), "textures/ui/book_addtextpage_default");
            form.addButton(lang.tr(PlayerLanguage, "pvp.gui.off"), "textures/ui/cancel");
            lang.close();
            form.sendTo(player, [](Player* pl, int id) {
                if (id == -1) {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                SQLiteDatabase db(PluginData + "/pvp.db");
                switch (id) {
                    case 0:
                        db.setTable("XUID" + pl->getXuid());
                        db.update("enable", "true");
                        break;
                    case 1:
                        db.setTable("XUID" + pl->getXuid());
                        db.update("enable", "false");
                        break;
                }
                db.close();
            });
        }
        
        class PvpCommand : public Command {
            enum PVPOP : int {
                gui = 3
            } op;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    switch (op) {
                        case PVPOP::gui: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Pvp: No player selected.");
                                break;
                            }
                            menuGui(ori.getPlayer());
                            std::string playerName = ori.getName();
                            outp.success("The UI has been opened to player " + playerName);
                            break;
                        }
                        default:
                            logger.error("Pvp >> 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Pvp: Instruction error.");
                            break;
                    }
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("pvp", "§e§lLOICollection -> §b服务器PVP", CommandPermissionLevel::Any, {(CommandFlagValue)0},{(CommandFlagValue)0x80});
                    registry->addEnum<PVPOP>("gui", {{"gui", PVPOP::gui}});
                    registry->registerOverload<PvpCommand>("pvp", makeMandatory<CommandParameterDataType::ENUM>(&PvpCommand::op, "op", "gui"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                PvpCommand::setup(e.mCommandRegistry);
                return true;
            });
            Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& e) {
                if (!tool::isBlacklist(e.mPlayer)) {
                    SQLiteDatabase db(PluginData + "/pvp.db");
                    if (!db.existsTable("XUID" + e.mPlayer->getXuid())) {
                        db.setTable("XUID" + e.mPlayer->getXuid());
                        db.createTable();
                        db.set("enable", "false");
                    }
                    db.close();
                }
                return true;
            });
            Event::PlayerAttackEvent::subscribe([](const Event::PlayerAttackEvent& e) {
                if (e.mTarget->isPlayer()) {
                    Player* mobPlayer = tool::toNamePlayer(e.mTarget->getNameTag());
                    Player* sourcePlayer = e.mPlayer;
                    if (!getEnable(mobPlayer)) {
                        std::string PlayerLanguage = tool::get(sourcePlayer);
                        i18nLang lang("./plugins/LOICollection/language.json");
                        sourcePlayer->sendTextPacket(lang.tr(PlayerLanguage, "pvp.off1"));
                        lang.close();
                        return false;
                    } else if (!getEnable(sourcePlayer)) {
                        std::string PlayerLanguage = tool::get(sourcePlayer);
                        i18nLang lang("./plugins/LOICollection/language.json");
                        sourcePlayer->sendTextPacket(lang.tr(PlayerLanguage, "pvp.off2"));
                        lang.close();
                        return false;
                    }
                    return true;
                }
                return true;
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        if (!std::filesystem::exists(PluginData + "/pvp.db")) {
            SQLiteDatabase db(PluginData + "/pvp.db");
            db.close();
        }
        listen();
    }
}