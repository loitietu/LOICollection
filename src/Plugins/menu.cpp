#include <string>
#include <llapi/FormUI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <Nlohmann/json.hpp>
#include "../tool.h"
#include "../Storage/JsonManager.h"
#include "include/i18nLang.h"
#include "include/menu.h"
extern Logger logger;

namespace menu {
    namespace {
        void menuGui(Player* player, const std::string& main) {

        }

        class MenuCommand : public Command {
            std::string uiName = "main";
            enum MENUOP : int {
                gui = 3,
                clock = 6
            } op;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    switch (op) {
                        case MENUOP::gui: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Menu: No player selected.");
                                break;
                            }
                            std::string playerName = ori.getName();
                            outp.success("Menu: The UI has been opened to player " + playerName);
                            menuGui(tool::toServerPlayer(ori.getPlayer()), uiName);
                            break;
                        }
                        case MENUOP::clock: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Menu: No player selected.");
                                break;
                            }
                            JsonManager config("./plugins/LOICollection/config.json");
                            nlohmann::ordered_json menu = config.get("Menu");
                            config.clear();
                            std::string ItemId = menu["ItemId"].template get<std::string>();
                            auto* item = ItemStack::create(ItemId, 1);
                            ori.getPlayer()->giveItem(item);
                            delete item;
                            std::string playerName = ori.getName();
                            outp.success("Menu: The clock has been given to player " + playerName);
                            break;
                        }
                        default:
                            logger.error("<Menu>: 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Menu: Instruction error.");
                            break;
                    }
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("menu", "§e§lLOICollection -> §b菜单", CommandPermissionLevel::Any, {(CommandFlagValue)0},{(CommandFlagValue)0x80});
                    registry->addEnum<MENUOP>("gui", {{"gui", MENUOP::gui}});
                    registry->addEnum<MENUOP>("clock", {{"clock", MENUOP::clock}});
                    registry->registerOverload<MenuCommand>("menu", makeMandatory<CommandParameterDataType::ENUM>(&MenuCommand::op, "op", "gui"));
                    registry->registerOverload<MenuCommand>("menu", makeMandatory<CommandParameterDataType::ENUM>(&MenuCommand::op, "op", "gui"), makeMandatory(&MenuCommand::uiName, "uiName"));
                    registry->registerOverload<MenuCommand>("menu", makeMandatory<CommandParameterDataType::ENUM>(&MenuCommand::op, "op", "clock"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                MenuCommand::setup(e.mCommandRegistry);
                return true;
            });
            Event::PlayerUseItemEvent::subscribe([](const Event::PlayerUseItemEvent& e) {
                JsonManager config("./plugins/LOICollection/config.json");
                nlohmann::ordered_json menu = config.get("Menu");
                config.clear();
                std::string ItemId = menu["ItemId"].template get<std::string>();
                if (e.mItemStack->getTypeName() == ItemId) menuGui(e.mPlayer, "main");
                return true;
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        listen();
        JsonManager database(PluginData + "/menu.json");
        database.save();
        logger.info("<Menu>: 插件已加载");
    }
}