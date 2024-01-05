#include <string>
#include <vector>
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
#include "include/menu.h"
extern Logger logger;

namespace menu {
    namespace {
        void menuGui(Player* player, const std::string& main) {
            JsonManager database(PluginData + "/menu.json");
            nlohmann::ordered_json data = database.get(main);
            database.clear();
            if (data != nullptr) {
                if (data["type"] == "Simple") {
                    auto form = Form::SimpleForm(data["title"], data["content"]);
                    std::vector<nlohmann::ordered_json> buttonIdList;
                    for (nlohmann::ordered_json& button : data["button"]) {
                        form.addButton(button["title"], button["image"]);
                        buttonIdList.push_back(button);
                    }
                    form.sendTo(player, [data, buttonIdList](Player* pl, int id) {
                        if (id == -1) {
                            pl->sendTextPacket(data["exit"]);
                            return;
                        }
                        nlohmann::ordered_json button = buttonIdList[id];
                        if (button["type"] == "button") {
                            bool ScoreboardEnough = true;
                            bool LLMoneyEnough = true;
                            for (nlohmann::ordered_json::iterator it = button["scores"].begin(); it != button["scores"].end(); ++it) {
                                int score = button["scores"][it.key()].template get<int>();
                                if (score > pl->getScore(it.key())) ScoreboardEnough = false;
                            }
                            if (button["llmoney"] > tool::llmoney::get(pl)) LLMoneyEnough = false;
                            if (ScoreboardEnough && LLMoneyEnough) {
                                tool::llmoney::reduce(pl, button["llmoney"]);
                                for (nlohmann::ordered_json::iterator it = button["scores"].begin(); it != button["scores"].end(); ++it) {
                                    int score = button["scores"][it.key()].template get<int>();
                                    pl->reduceScore(it.key(), score);
                                }
                                pl->runcmd(button["command"]);
                            } else {
                                pl->sendTextPacket(data["NoScore"]);
                                button.clear();
                                return;
                            } 
                        } else if (button["type"] == "from") {
                            bool ScoreboardEnough = true;
                            bool LLMoneyEnough = true;
                            for (nlohmann::ordered_json::iterator it = button["scores"].begin(); it != button["scores"].end(); ++it) {
                                int score = button["scores"][it.key()].template get<int>();
                                if (score > pl->getScore(it.key())) ScoreboardEnough = false;
                            }
                            if (button["llmoney"] > tool::llmoney::get(pl)) LLMoneyEnough = false;
                            if (ScoreboardEnough && LLMoneyEnough) {
                                tool::llmoney::reduce(pl, button["llmoney"]);
                                for (nlohmann::ordered_json::iterator it = button["scores"].begin(); it != button["scores"].end(); ++it) {
                                    int score = button["scores"][it.key()].template get<int>();
                                    pl->reduceScore(it.key(), score);
                                }
                                menuGui(pl, button["menu"]);
                            } else {
                                pl->sendTextPacket(data["NoScore"]);
                                button.clear();
                                return;
                            }
                        } else if (button["type"] == "opbutton") {
                            if (pl->isOP()) {
                                pl->runcmd(button["command"]);
                            } else {
                                pl->sendTextPacket(data["NoPermission"]);
                                button.clear();
                                return;
                            }
                        } else if (button["type"] == "opfrom") {
                            if (pl->isOP()) {
                                menuGui(pl, button["menu"]);
                            } else {
                                pl->sendTextPacket(data["NoPermission"]);
                                button.clear();
                                return;
                            }
                        }
                        button.clear();
                    });
                    data.clear();
                    buttonIdList.clear();
                } else if (data["type"] == "Modal") {
                    auto form = Form::ModalForm(data["title"], data["content"], data["confirmButton"]["title"], data["cancelButton"]["title"]);
                    form.sendTo(player, [data](Player* pl, bool isConfirm) {
                        nlohmann::ordered_json button;
                        if (isConfirm) button = data["confirmButton"];
                        else button = data["cancelButton"];
                        if (button["type"] == "button") {
                            bool ScoreboardEnough = true;
                            bool LLMoneyEnough = true;
                            for (nlohmann::ordered_json::iterator it = button["scores"].begin(); it != button["scores"].end(); ++it) {
                                int score = button["scores"][it.key()].template get<int>();
                                if (score > pl->getScore(it.key())) ScoreboardEnough = false;
                            }
                            if (button["llmoney"] > tool::llmoney::get(pl)) LLMoneyEnough = false;
                            if (ScoreboardEnough && LLMoneyEnough) {
                                tool::llmoney::reduce(pl, button["llmoney"]);
                                for (nlohmann::ordered_json::iterator it = button["scores"].begin(); it != button["scores"].end(); ++it) {
                                    int score = button["scores"][it.key()].template get<int>();
                                    pl->reduceScore(it.key(), score);
                                }
                                pl->runcmd(button["command"]);
                            } else {
                                pl->sendTextPacket(data["NoScore"]);
                                button.clear();
                                return;
                            } 
                        } else if (button["type"] == "from") {
                            bool ScoreboardEnough = true;
                            bool LLMoneyEnough = true;
                            for (nlohmann::ordered_json::iterator it = button["scores"].begin(); it != button["scores"].end(); ++it) {
                                int score = button["scores"][it.key()].template get<int>();
                                if (score > pl->getScore(it.key())) ScoreboardEnough = false;
                            }
                            if (button["llmoney"] > tool::llmoney::get(pl)) LLMoneyEnough = false;
                            if (ScoreboardEnough && LLMoneyEnough) {
                                tool::llmoney::reduce(pl, button["llmoney"]);
                                for (nlohmann::ordered_json::iterator it = button["scores"].begin(); it != button["scores"].end(); ++it) {
                                    int score = button["scores"][it.key()].template get<int>();
                                    pl->reduceScore(it.key(), score);
                                }
                                menuGui(pl, button["menu"]);
                            } else {
                                pl->sendTextPacket(data["NoScore"]);
                                button.clear();
                                return;
                            }
                        } else if (button["type"] == "opbutton") {
                            if (pl->isOP()) {
                                pl->runcmd(button["command"]);
                            } else {
                                pl->sendTextPacket(data["NoPermission"]);
                                button.clear();
                                return;
                            }
                        } else if (button["type"] == "opfrom") {
                            if (pl->isOP()) {
                                menuGui(pl, button["menu"]);
                            } else {
                                pl->sendTextPacket(data["NoPermission"]);
                                button.clear();
                                return;
                            }
                        }
                        button.clear();
                    });
                    data.clear();
                }
            } else {
                logger.error("<Menu>: 菜单文件读取失败");
                return;
            }
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
                            bool isItemNull = item->isNull();
                            ori.getPlayer()->giveItem(item);
                            delete item;
                            std::string playerName = ori.getName();
                            if (!isItemNull) outp.success("Menu: The MenuItem has been given to player " + playerName);
                            else outp.error("Menu: Failed to give the MenuItem to player " + playerName);
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