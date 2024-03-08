#include <string>
#include <map>
#include <unordered_map>
#include <filesystem>
#include <llapi/FormUI.h>
#include <llapi/EventAPI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/DynamicCommandAPI.h>
#include <llapi/mc/CommandOrigin.hpp>
#include <llapi/mc/CommandOutput.hpp>
#include <llapi/mc/Player.hpp>
#include "../API.h"
#include "../utils/tool.h"
#include "../Storage/SQLiteDatabase.h"
#include "include/i18nLang.h"
#include "include/language.h"
extern Logger logger;

namespace language {
    namespace {
        void ui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "language.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "language.gui.label")));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "language.gui.dropdown"), lang.list()));
            lang.close();
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                std::string PlayerLanguage = tool::get(pl);
                i18nLang lang("./plugins/LOICollection/language.json");
                if (mp.empty()) {
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string PlayerSelectLanguage = mp["dropdown"]->getString();
                SQLiteDatabase db(PluginData + "/language.db");
                db.update(pl->getXuid(), PlayerSelectLanguage);
                db.close();
                logger.info(LOICollectionAPI::translateString(lang.tr(PlayerLanguage, "language.log"), pl, true));
                lang.close();
            });
        }

        void listen() {
            Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& event) {
                std::string xuid = event.mPlayer->getXuid();
                SQLiteDatabase db(PluginData + "/language.db");
                if (!db.exists(xuid))
                    db.set(xuid, "zh_CN");
                db.close();
                return true;
            });
        }

        void command() {
            DynamicCommand::setup(
                "language",
                "§e§lLOICollection -> §a语言设置",
                {}, {}, { {}, },
                [](
                    DynamicCommand const& command,
                    CommandOrigin const& origin,
                    CommandOutput& output,
                    std::unordered_map<std::string, DynamicCommand::Result>& results
                ) {
                    if (origin.getPlayer() == nullptr) {
                        output.error("Language: No player selected.");
                        return;
                    }
                    std::string playerName = origin.getName();
                    ui(origin.getPlayer());
                    output.success("The UI has been opened to player " + playerName);
                }
            );
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        if (!std::filesystem::exists(PluginData + "/language.db")) {
            SQLiteDatabase db(PluginData + "/language.db");
            db.close();
        }
        listen();
        command();
    }
}