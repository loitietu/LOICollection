#include <string>
#include <vector>
#include <llapi/FormUI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <Nlohmann/json.hpp>
#include "../utils/tool.h"
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
                            int llmoney = button["llmoney"].template get<int>();
                            volatile bool ScoreboardEnough = true;
                            volatile bool LLMoneyEnough = true;
                            for (nlohmann::ordered_json::iterator it = button["scores"].begin(); it != button["scores"].end(); ++it) {
                                int score = button["scores"][it.key()].template get<int>();
                                if (score > pl->getScore(it.key())) {
                                    ScoreboardEnough = false;
                                    break;
                                }
                            }
                            if (llmoney > tool::llmoney::get(pl)) LLMoneyEnough = false;
                            if (ScoreboardEnough && LLMoneyEnough) {
                                tool::llmoney::reduce(pl, llmoney);
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
                            int llmoney = button["llmoney"].template get<int>();
                            volatile bool ScoreboardEnough = true;
                            volatile bool LLMoneyEnough = true;
                            for (nlohmann::ordered_json::iterator it = button["scores"].begin(); it != button["scores"].end(); ++it) {
                                int score = button["scores"][it.key()].template get<int>();
                                if (score > pl->getScore(it.key())) {
                                    ScoreboardEnough = false;
                                    break;
                                }
                            }
                            if (llmoney > tool::llmoney::get(pl)) LLMoneyEnough = false;
                            if (ScoreboardEnough && LLMoneyEnough) {
                                tool::llmoney::reduce(pl, llmoney);
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
                            int llmoney = button["llmoney"].template get<int>();
                            volatile bool ScoreboardEnough = true;
                            volatile bool LLMoneyEnough = true;
                            for (nlohmann::ordered_json::iterator it = button["scores"].begin(); it != button["scores"].end(); ++it) {
                                int score = button["scores"][it.key()].template get<int>();
                                if (score > pl->getScore(it.key())) {
                                    ScoreboardEnough = false;
                                    break;
                                }
                            }
                            if (llmoney > tool::llmoney::get(pl)) LLMoneyEnough = false;
                            if (ScoreboardEnough && LLMoneyEnough) {
                                tool::llmoney::reduce(pl, llmoney);
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
                            int llmoney = button["llmoney"].template get<int>();
                            volatile bool ScoreboardEnough = true;
                            volatile bool LLMoneyEnough = true;
                            for (nlohmann::ordered_json::iterator it = button["scores"].begin(); it != button["scores"].end(); ++it) {
                                int score = button["scores"][it.key()].template get<int>();
                                if (score > pl->getScore(it.key())) {
                                    ScoreboardEnough = false;
                                    break;
                                }
                            }
                            if (llmoney > tool::llmoney::get(pl)) LLMoneyEnough = false;
                            if (ScoreboardEnough && LLMoneyEnough) {
                                tool::llmoney::reduce(pl, llmoney);
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
                            if (uiName == "main") {
                                outp.success("Menu: The UI has been opened to player " + ori.getName());
                                menuGui(ori.getPlayer(), uiName);
                            } else if (ori.getPlayer()->isOP()) {
                                outp.success("Menu: The UI has been opened to player " + ori.getName());
                                menuGui(ori.getPlayer(), uiName);
                            } else {
                                outp.error("Menu: No permission to open the Setting.");
                            }
                            break;
                        }
                        case MENUOP::clock: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Menu: No player selected.");
                                break;
                            }
                            nlohmann::ordered_json data = tool::getJson("./plugins/LOICollection/config.json")["Menu"];
                            std::string ItemId = data["ItemId"].template get<std::string>();
                            auto* item = ItemStack::create(ItemId, 1);
                            ori.getPlayer()->giveItem(item);
                            if (!item->isNull()) { 
                                outp.success("Menu: The MenuItem has been given to player " + ori.getName());
                            } else outp.error("Menu: Failed to give the MenuItem to player " + ori.getName());
                            data.clear();
                            delete item;
                            break;
                        }
                        default:
                            logger.error("Menu >> 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Menu: Instruction error.");
                            break;
                    }
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("menu", "§e§lLOICollection -> §b服务器菜单", CommandPermissionLevel::Any, {(CommandFlagValue)0},{(CommandFlagValue)0x80});
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
                nlohmann::ordered_json data = tool::getJson("./plugins/LOICollection/config.json")["Menu"];
                std::string ItemId = data["ItemId"].template get<std::string>();
                if (e.mItemStack->getTypeName() == ItemId) {
                    menuGui(e.mPlayer, "main");
                }
                data.clear();
                return true;
            });
            Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& e) {
                nlohmann::ordered_json data = tool::getJson("./plugins/LOICollection/config.json")["Menu"];
                std::string ItemId = data["ItemId"].template get<std::string>();
                ItemStack* item = ItemStack::create(ItemId, 1);
                if (!tool::isItemPlayerInventory(e.mPlayer, item) && !item->isNull()) {
                    e.mPlayer->giveItem(item);
                }
                data.clear();
                delete item;
                return true;
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        JsonManager database(PluginData + "/menu.json");
        database.save();
        listen();
    }
}