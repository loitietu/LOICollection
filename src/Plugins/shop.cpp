#include <string>
#include <vector>
#include <filesystem>
#include <llapi/LoggerAPI.h>
#include <llapi/FormUI.h>
#include <llapi/EventAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/mc/Player.hpp>
#include <Nlohmann/json.hpp>
#include "../Storage/SQLiteDatabase.h"
#include "../Storage/JsonManager.h"
#include "../utils/tool.h"
#include "../API.h"
#include "include/shop.h"
extern Logger logger;

namespace shop {
    namespace {
        void ShopGui(Player* player, const std::string& shopName) {
            JsonManager database(PluginData + "/shop.json");
            nlohmann::ordered_json shopDatabase = database.get(shopName);
            database.clear();
            if (shopDatabase != nullptr) {
                if (shopDatabase["type"] == "buy") {
                    auto form = Form::SimpleForm(shopDatabase["title"], shopDatabase["content"]);
                    std::vector<nlohmann::ordered_json> itemLists;
                    for (nlohmann::ordered_json& item : shopDatabase["classiflcation"]) {
                        form.addButton(item["title"], item["image"]);
                        itemLists.push_back(item);
                    }
                    form.sendTo(player, [shopDatabase, itemLists](Player* pl, int id) {
                        if (id == -1) {
                            pl->sendTextPacket(shopDatabase["exit"]);
                            return;
                        }
                        nlohmann::ordered_json item = itemLists.at(id);
                        if (item["type"] == "commodity") {
                            int llmoney = item["llmoney"].template get<int>();
                            nlohmann::ordered_json ScoreboardBase = item["scores"];
                            std::string ScoreboardListString;
                            if (!ScoreboardBase.empty()) {
                                for (nlohmann::ordered_json::iterator it = ScoreboardBase.begin(); it != ScoreboardBase.end(); ++it) {
                                    int score = it.value().template get<int>();
                                    ScoreboardListString += it.key() + ":" + std::to_string(score) + ",";
                                }
                                ScoreboardListString.pop_back();
                            } else {
                                ScoreboardListString = "None";
                            }
                            std::string introduce = std::string(tool::replaceString(item["introduce"], "${llmoney}", std::to_string(llmoney)));
                            introduce = std::string(tool::replaceString(introduce, "${scores}", ScoreboardListString));
                            auto form = Form::CustomForm(shopDatabase["title"]);
                            form.append(Form::Label("label", introduce));
                            form.append(Form::Slider("slider", item["number"], 1, 64));
                            form.sendTo(pl, [shopDatabase, item](Player* pl2, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                                if (mp.empty()) {
                                    pl2->sendTextPacket(shopDatabase["exit"]);
                                    return;
                                }
                                volatile bool ScoreboardEnough = true;
                                volatile bool LLMoneyEnough = true;
                                int buyNumber = mp["slider"]->getInt();
                                nlohmann::ordered_json ScoreboardList = item.at("scores");
                                for (nlohmann::ordered_json::iterator it = ScoreboardList.begin(); it != ScoreboardList.end(); ++it) {
                                    int score = ScoreboardList[it.key()].template get<int>();
                                    if ((score * buyNumber) > pl2->getScore(it.key())) {
                                        ScoreboardEnough = false;
                                        break;
                                    }
                                }
                                if ((item["llmoney"] * buyNumber) > tool::llmoney::get(pl2)) LLMoneyEnough = false;
                                if (ScoreboardEnough && LLMoneyEnough) {
                                    tool::llmoney::reduce(pl2, (item["llmoney"] * buyNumber));
                                    for (nlohmann::ordered_json::iterator it = ScoreboardList.begin(); it != ScoreboardList.end(); ++it) {
                                        int score = ScoreboardList[it.key()].template get<int>();
                                        pl2->reduceScore(it.key(), (score * buyNumber));
                                    }
                                    ScoreboardList.clear();
                                    auto* itemStack = ItemStack::create(item["id"].template get<std::string>(), buyNumber);
                                    itemStack->setCustomName(item["name"]);
                                    pl2->giveItem(itemStack);
                                    delete itemStack;
                                } else {
                                    pl2->sendTextPacket(shopDatabase["NoScore"]);
                                    ScoreboardList.clear();
                                    return;
                                } 
                            });
                        } else if (item["type"] == "title") {
                            if (std::filesystem::exists(PluginData + "/chat.db")) {
                                int llmoney = item["llmoney"].template get<int>();
                                nlohmann::ordered_json ScoreboardBase = item["scores"];
                                std::string ScoreboardListString;
                                if (!ScoreboardBase.empty()) {
                                    for (nlohmann::ordered_json::iterator it = ScoreboardBase.begin(); it != ScoreboardBase.end(); ++it) {
                                        int score = it.value().template get<int>();
                                        ScoreboardListString += it.key() + ":" + std::to_string(score) + ",";
                                    }
                                    ScoreboardListString.pop_back();
                                } else {
                                    ScoreboardListString = "None";
                                }
                                std::string introduce = std::string(tool::replaceString(item["introduce"], "${llmoney}", std::to_string(llmoney)));
                                introduce = std::string(tool::replaceString(introduce, "${scores}", ScoreboardListString));
                                auto form = Form::ModalForm(shopDatabase["title"], introduce, item["confirmButton"], item["cancelButton"]);
                                form.sendTo(pl, [shopDatabase, item, llmoney](Player* pl2, bool isConfirm) {
                                    if (isConfirm) {
                                        volatile bool ScoreboardEnough = true;
                                        volatile bool LLMoneyEnough = true;
                                        nlohmann::ordered_json ScoreboardList = item.at("scores");
                                        for (nlohmann::ordered_json::iterator it = ScoreboardList.begin(); it != ScoreboardList.end(); ++it) {
                                            int score = ScoreboardList[it.key()].template get<int>();
                                            if (score > pl2->getScore(it.key())) {
                                                ScoreboardEnough = false;
                                                break;
                                            }
                                        }
                                        if (llmoney > tool::llmoney::get(pl2)) LLMoneyEnough = false;
                                        if (ScoreboardEnough && LLMoneyEnough) {
                                            tool::llmoney::reduce(pl2, item["llmoney"]);
                                            for (nlohmann::ordered_json::iterator it = ScoreboardList.begin(); it != ScoreboardList.end(); ++it) {
                                                int score = ScoreboardList[it.key()].template get<int>();
                                                pl2->reduceScore(it.key(), score);
                                            }
                                            ScoreboardList.clear();
                                            SQLiteDatabase db(PluginData + "/chat.db");
                                            db.setTable("XUID" + pl2->getXuid() + "TITLE");
                                            db.set(item["id"].template get<std::string>(), "true");
                                            db.close();
                                        } else {
                                            pl2->sendTextPacket(shopDatabase["NoScore"]);
                                            ScoreboardList.clear();
                                            return;
                                        } 
                                    } else {
                                        pl2->sendTextPacket(shopDatabase["exit"]);
                                        return;
                                    }
                                });
                            } else {
                                pl->sendTextPacket("Shop: Please open plug-in Chat.");
                                return;
                            }
                        } else if (item["type"] == "from") {
                            ShopGui(pl, item["menu"]);
                        }
                    });
                } else if (shopDatabase["type"] == "sell") {
                    auto form = Form::SimpleForm(shopDatabase["title"], shopDatabase["content"]);
                    std::vector<nlohmann::ordered_json> itemLists;
                    for (nlohmann::ordered_json& item : shopDatabase["classiflcation"]) {
                        form.addButton(item["title"], item["image"]);
                        itemLists.push_back(item);
                    }
                    form.sendTo(player, [shopDatabase, itemLists](Player* pl, int id) {
                        if (id == -1) {
                            pl->sendTextPacket(shopDatabase["exit"]);
                            return;
                        }
                        nlohmann::ordered_json item = itemLists.at(id);
                        if (item["type"] == "commodity") {
                            int llmoney = item["llmoney"].template get<int>();
                            nlohmann::ordered_json ScoreboardBase = item["scores"];
                            std::string ScoreboardListString;
                            if (!ScoreboardBase.empty()) {
                                for (nlohmann::ordered_json::iterator it = ScoreboardBase.begin(); it != ScoreboardBase.end(); ++it) {
                                    int score = it.value().template get<int>();
                                    ScoreboardListString += it.key() + ":" + std::to_string(score) + ",";
                                }
                                ScoreboardListString.pop_back();
                            } else {
                                ScoreboardListString = "None";
                            }
                            std::string introduce = std::string(tool::replaceString(item["introduce"], "${llmoney}", std::to_string(llmoney)));
                            introduce = std::string(tool::replaceString(introduce, "${scores}", ScoreboardListString));
                            auto form = Form::CustomForm(shopDatabase["title"]);
                            form.append(Form::Label("label", introduce));
                            form.append(Form::Slider("slider", item["number"], 1, 64));
                            form.sendTo(pl, [shopDatabase, item](Player* pl2, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                                if (mp.empty()) {
                                    pl2->sendTextPacket(shopDatabase["exit"]);
                                    return;
                                }
                                int llmoney = item["llmoney"].template get<int>();
                                int sellNumber = mp["slider"]->getInt();
                                auto* itemStack = ItemStack::create(item["id"].template get<std::string>(), sellNumber);
                                if (tool::isItemPlayerInventory(pl2, itemStack)) {
                                    tool::clearItem(pl2, itemStack);
                                    tool::llmoney::add(pl2, (llmoney * sellNumber));
                                    nlohmann::ordered_json ScoreboardList = item.at("scores");
                                    for (nlohmann::ordered_json::iterator it = ScoreboardList.begin(); it != ScoreboardList.end(); ++it) {
                                        int score = ScoreboardList[it.key()].template get<int>();
                                        pl2->addScore(it.key(), (score * sellNumber));
                                    }
                                    ScoreboardList.clear();
                                    delete itemStack;
                                } else {
                                    pl2->sendTextPacket(shopDatabase["NoItem"]);
                                    delete itemStack;
                                    return;
                                }
                            });
                        } else if (item["type"] == "title") {
                            if (std::filesystem::exists(PluginData + "/chat.db")) {
                                int llmoney = item["llmoney"].template get<int>();
                                nlohmann::ordered_json ScoreboardBase = item["scores"];
                                std::string ScoreboardListString;
                                if (!ScoreboardBase.empty()) {
                                    for (nlohmann::ordered_json::iterator it = ScoreboardBase.begin(); it != ScoreboardBase.end(); ++it) {
                                        int score = it.value().template get<int>();
                                        ScoreboardListString += it.key() + ":" + std::to_string(score) + ",";
                                    }
                                    ScoreboardListString.pop_back();
                                } else {
                                    ScoreboardListString = "None";
                                }
                                std::string introduce = std::string(tool::replaceString(item["introduce"], "${llmoney}", std::to_string(llmoney)));
                                introduce = std::string(tool::replaceString(introduce, "${scores}", ScoreboardListString));
                                auto form = Form::ModalForm(shopDatabase["title"], introduce, item["confirmButton"], item["cancelButton"]);
                                form.sendTo(pl, [shopDatabase, item](Player* pl2, bool isConfirm) {
                                    if (isConfirm) {
                                        int llmoney = item["llmoney"].template get<int>();
                                        std::string titleId = item["id"].template get<std::string>();
                                        SQLiteDatabase db(PluginData + "/chat.db");
                                        db.setTable("XUID" + pl2->getXuid() + "TITLE");
                                        if (db.exists(titleId)) {
                                            db.remove(titleId);
                                            db.setTable("XUID" + pl2->getXuid());
                                            if (db.get("title") == titleId) {
                                                db.update("title", "None");
                                            }
                                            tool::llmoney::add(pl2, llmoney);
                                            nlohmann::ordered_json ScoreboardList = item.at("scores");
                                            for (nlohmann::ordered_json::iterator it = ScoreboardList.begin(); it != ScoreboardList.end(); ++it) {
                                                int score = ScoreboardList[it.key()].template get<int>();
                                                pl2->addScore(it.key(), score);
                                            }
                                            ScoreboardList.clear();
                                            db.close();
                                        } else {
                                            pl2->sendTextPacket(shopDatabase["NoTitle"]);
                                            db.close();
                                            return;
                                        }
                                    } else {
                                        pl2->sendTextPacket(shopDatabase["exit"]);
                                        return;
                                    }
                                });
                            } else {
                                pl->sendTextPacket("Shop: Please open plug-in Chat.");
                                return;
                            }
                        } else if (item["type"] == "from") {
                            ShopGui(pl, item["menu"]);
                        }
                    });
                    shopDatabase.clear();
                    itemLists.clear();
                } else {
                    return;
                }
            } else {
                logger.error("<Shop>: 商店文件读取失败");
                return;
            }
        }

        class ShopCommand : public Command {
            enum SHOP : int {
                gui = 3,
                sell = 8
            } op;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    switch (op) {
                        case SHOP::gui: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Shop: No player selected.");
                                break;
                            }
                            ShopGui(ori.getPlayer(), "MainBuy");
                            outp.success("The UI has been opened to player " + ori.getName());
                            break;
                        }
                        case SHOP::sell: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Shop: No player selected.");
                                break;
                            }
                            ShopGui(ori.getPlayer(), "MainSell");
                            outp.success("The UI has been opened to player " + ori.getName());
                            break;
                        }
                        default:
                            logger.error("Shop >> 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Shop: Instruction error.");
                            break;
                    }
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("shop", "§e§lLOICollection -> §b服务器商店", CommandPermissionLevel::Any, {(CommandFlagValue)0},{(CommandFlagValue)0x80});
                    registry->addEnum<SHOP>("gui", {{"gui", SHOP::gui}});
                    registry->addEnum<SHOP>("sell", {{"sell", SHOP::sell}});
                    registry->registerOverload<ShopCommand>("shop", makeMandatory<CommandParameterDataType::ENUM>(&ShopCommand::op, "op", "gui"));
                    registry->registerOverload<ShopCommand>("shop", makeMandatory<CommandParameterDataType::ENUM>(&ShopCommand::op, "op", "sell"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                ShopCommand::setup(e.mCommandRegistry);
                return true;
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        JsonManager database(PluginData + "/shop.json");
        database.save();
        listen();
    }
}