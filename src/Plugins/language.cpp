#include <string>
#include <map>
#include <unordered_map>
#include <filesystem>
#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/DynamicCommandAPI.h>
#include <llapi/FormUI.h>
#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/mc/CommandOrigin.hpp>
#include <llapi/mc/CommandOutput.hpp>
#include <llapi/mc/Player.hpp>
#include "../config.h"
#include "../Storage/SQLiteDatabase.h"
#include "i18nLang.h"
#include "language.h"
extern Logger logger;

namespace language {
    namespace {
        void database() {
            if (!std::filesystem::exists(PluginDataPath.append("language.db"))) {
                logger.info("数据库 language.db 已创建");
                SQLiteDatabase db(PluginData + "/language.db");
                db.close();
            }
        }

        void ui(ServerPlayer* player) {
            std::string PlayerLanguage = get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "language.gui.title"));
            form.append(Form::Label("label1", lang.tr(PlayerLanguage, "language.gui.label")));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "language.gui.dropdown"), lang.list()));
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                if (mp.empty())
                    return;
                std::string PlayerSelectLanguage = mp["dropdown"]->getString();
                SQLiteDatabase db(PluginData + "/language.db");
                db.update(pl->getXuid(), PlayerSelectLanguage);
                db.close();
                logger.info("玩家 {} 已更改语言为: {}", pl->getRealName(), PlayerSelectLanguage);
            });
            lang.close();
        }

        void listen() {
            Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& event) {
                std::string xuid = event.mPlayer->getXuid();
                database();
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
                    std::string playerName = origin.getName();
                    ui(origin.getPlayer());
                    output.success("The UI has been opened to player " + playerName);
                }
            );
        }
    }

    std::string get(ServerPlayer* player) {
        SQLiteDatabase db(PluginData + "/language.db");
        std::string playerLang = db.get(player->getXuid());
        db.close();
        return playerLang;
    }

    std::string get(Player* player) {
        SQLiteDatabase db(PluginData + "/language.db");
        std::string playerLang = db.get(player->getXuid());
        db.close();
        return playerLang;
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        listen();
        command();
        database();
        logger.info("插件 <language> 已加载");
    }
}