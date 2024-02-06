#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include <llapi/FormUI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/mc/Player.hpp>
#include "../Storage/SQLiteDatabase.h"
#include "../tools/tool.h"
#include "include/i18nLang.h"
#include "include/market.h"
extern Logger logger;

namespace market {
    namespace {
        void buyGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            SQLiteDatabase db(PluginData + "/market.db");
            db.setTable("Item");
            if (!db.existsTable("Item")) db.createTable();
            std::vector<std::string> data = db.listTable("Item");
            auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "market.gui.title"), lang.tr(PlayerLanguage, "market.gui.label"));
            for (auto& i : data) {
                std::string UploadPlayer = db.get(i);
                db.setTable(UploadPlayer + "LIST");
                std::string idString = tool::replaceString(i, UploadPlayer, "");
                std::string name = db.getMap(idString, "name");
                std::string icon = db.getMap(idString, "icon");
                form.addButton(name, icon);
                db.setTable("Item");
            }
            lang.close();
            db.close();
            form.sendTo(player, [data](Player* pl, int id) {
                std::string PlayerLanguage = tool::get(pl);
                i18nLang lang("./plugins/LOICollection/language.json");
                if (id == -1) {
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                auto& idItem = data.at(id);
                SQLiteDatabase db(PluginData + "/market.db");
                db.setTable("Item");
                std::string UploadPlayer = db.get(idItem);
                db.setTable(UploadPlayer + "LIST");
                std::string idString = tool::replaceString(idItem, UploadPlayer, "");
                std::string introduce = lang.tr(PlayerLanguage, "market.gui.sell.introduce");
                introduce = tool::replaceString(introduce, "${introduce}", db.getMap(idString, "introduce"));
                introduce = tool::replaceString(introduce, "${llmoney}", db.getMap(idString, "llmoney"));
                introduce = tool::replaceString(introduce, "${nbt}", db.getMap(idString, "nbt"));
                introduce = tool::replaceString(introduce, "${player}", db.getMap(idString, "player"));
                auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "market.gui.title"), introduce);
                form.addButton(lang.tr(PlayerLanguage, "market.gui.sell.buy.button1"), "");
                if (pl->isOP()) form.addButton(lang.tr(PlayerLanguage, "market.gui.sell.sellItemContent.button1"), "");
                lang.close();
                db.close();
                form.sendTo(pl, [idString, UploadPlayer](Player* pl2, int id) {
                    std::string PlayerLanguage = tool::get(pl2);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    if (id == -1) {
                        pl2->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                        lang.close();
                        return;
                    }
                    SQLiteDatabase db(PluginData + "/market.db");
                    switch (id) {
                        case 0: {
                            db.setTable(UploadPlayer + "LIST");
                            bool LLMoneyEnough = true;
                            int llmoney = std::stoi(db.getMap(idString, "llmoney"));
                            if (llmoney > tool::llmoney::get(pl2)) LLMoneyEnough = false;
                            if (LLMoneyEnough) {
                                tool::llmoney::reduce(pl2, llmoney);
                                std::string name = db.getMap(idString, "name");
                                std::string nbt = db.getMap(idString, "nbt");
                                std::string xuid = db.getMap(idString, "xuid");
                                auto* item = ItemStack::create(CompoundTag::fromSNBT(nbt));
                                pl2->giveItem(item);
                                db.removeMap(idString);
                                db.setTable(UploadPlayer);
                                if (tool::toXuidPlayer(xuid) == nullptr) {
                                    int score = std::stoi(db.get("Money"));
                                    score += llmoney;
                                    db.set("Money", std::to_string(score));
                                } else {
                                    Player* xuidPlayer = tool::toXuidPlayer(xuid);
                                    std::string tips = lang.tr(PlayerLanguage, "market.gui.sell.sellItem.tips2");
                                    tips = tool::replaceString(tips, "${item}", name);
                                    xuidPlayer->sendTextPacket(tips);
                                    tool::llmoney::add(xuidPlayer, llmoney);
                                }
                                db.setTable("Item");
                                db.remove(UploadPlayer + idString);
                                delete item;
                            } else {
                                pl2->sendTextPacket(lang.tr(PlayerLanguage, "market.gui.sell.sellItem.tips4"));
                            }
                            break;
                        }
                        case 1: {
                            db.setTable(UploadPlayer + "LIST");
                            std::string name = db.getMap(idString, "name");
                            std::string tips = lang.tr(PlayerLanguage, "market.gui.sell.sellItem.tips3");
                            tips = tool::replaceString(tips, "${item}", name);
                            pl2->sendTextPacket(tips);
                            db.removeMap(idString);
                            db.setTable("Item");
                            db.remove(UploadPlayer + idString);
                            break;
                        }
                    }
                    db.close();
                    lang.close();
                });
            });
        }

        void sellItemGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "market.gui.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "market.gui.label")));
            form.append(Form::Input("input1", lang.tr(PlayerLanguage, "market.gui.sell.sellItem.input1"), "", "Item"));
            form.append(Form::Input("input2", lang.tr(PlayerLanguage, "market.gui.sell.sellItem.input2"), "", "textures/items/diamond"));
            form.append(Form::Input("input3", lang.tr(PlayerLanguage, "market.gui.sell.sellItem.input3"), "", "Introduce"));
            form.append(Form::Input("input4", lang.tr(PlayerLanguage, "market.gui.sell.sellItem.input4"), "", "100"));
            lang.close();
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                std::thread([&] {
                    std::string PlayerLanguage = tool::get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    if (mp.empty()) {
                        pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                        lang.close();
                        return;
                    }
                    if (!pl->getHandSlot()->isNull()) {
                        int price = 100;
                        try {
                            price = std::stoi(mp["input4"]->getString());
                        } catch (std::exception& e) { 
                            price = 100;
                        }
                        if (price < 0) price = (price * -1);
                        std::string itemName = mp["input1"]->getString();
                        std::string itemIcon = mp["input2"]->getString();
                        std::string introduce = mp["input3"]->getString();
                        SQLiteDatabase db(PluginData + "/market.db");
                        db.setTable("Item");
                        if (!db.existsTable("Item")) db.createTable();
                        db.setTable("XUID" + pl->getXuid() + "LIST");
                        if (!db.existsTable("XUID" + pl->getXuid() + "LIST")) db.createTable();
                        int index = 1;
                        while (db.existsMap("Item" + std::to_string(index))) {
                            index++;
                        }
                        ItemStack* item = pl->getHandSlot();
                        auto snbt = const_cast<ItemStack*>(item)->getNbt()->toSNBT();
                        int count = item->getCount();
                        std::string mapName = "Item" + std::to_string(index);
                        db.createMap(mapName);
                        db.setMap(mapName, "name", itemName);
                        db.setMap(mapName, "icon", itemIcon);
                        db.setMap(mapName, "introduce", introduce);
                        db.setMap(mapName, "llmoney", std::to_string(price));
                        db.setMap(mapName, "nbt", snbt);
                        db.setMap(mapName, "xuid", pl->getXuid());
                        db.setMap(mapName, "player", pl->getRealName());
                        db.setTable("Item");
                        db.set("XUID" + pl->getXuid() + mapName, "XUID" + pl->getXuid());
                        db.close();
                        item->remove(count);
                        pl->refreshInventory();
                    } else {
                        pl->sendTextPacket(lang.tr(PlayerLanguage, "market.gui.sell.sellItem.tips1"));
                    }
                    lang.close();
                }).detach();
            });
        }

        void sellItemContentGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            SQLiteDatabase db(PluginData + "/market.db");
            db.setTable("XUID" + player->getXuid() + "LIST");
            if (!db.existsTable("XUID" + player->getXuid() + "LIST")) db.createTable();
            std::vector<std::string> data = db.listTable("XUID" + player->getXuid() + "LIST");
            auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "market.gui.title"), lang.tr(PlayerLanguage, "market.gui.label"));
            for (auto& i : data) {
                std::string name = db.getMap(i, "name");
                std::string icon = db.getMap(i, "icon");
                form.addButton(name, icon);
            }
            lang.close();
            db.close();
            form.sendTo(player, [data](Player* pl, int id) {
                std::string PlayerLanguage = tool::get(pl);
                i18nLang lang("./plugins/LOICollection/language.json");
                if (id == -1) {
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                auto& idString = data.at(id);
                SQLiteDatabase db(PluginData + "/market.db");
                db.setTable("XUID" + pl->getXuid() + "LIST");
                std::string introduce = lang.tr(PlayerLanguage, "market.gui.sell.introduce");
                introduce = tool::replaceString(introduce, "${introduce}", db.getMap(idString, "introduce"));
                introduce = tool::replaceString(introduce, "${llmoney}", db.getMap(idString, "llmoney"));
                introduce = tool::replaceString(introduce, "${nbt}", db.getMap(idString, "nbt"));
                introduce = tool::replaceString(introduce, "${player}", db.getMap(idString, "player"));
                auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "market.gui.title"), introduce);
                form.addButton(lang.tr(PlayerLanguage, "market.gui.sell.sellItemContent.button1"), "");
                lang.close();
                db.close();
                form.sendTo(pl, [idString](Player* pl2, int id) {
                    std::string PlayerLanguage = tool::get(pl2);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    if (id == -1) {
                        pl2->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                        lang.close();
                        return;
                    }
                    SQLiteDatabase db(PluginData + "/market.db");
                    switch (id) {
                        case 0: {
                            db.setTable("XUID" + pl2->getXuid() + "LIST");
                            std::string name = db.getMap(idString, "name");
                            std::string nbt = db.getMap(idString, "nbt");
                            auto* item = ItemStack::create(CompoundTag::fromSNBT(nbt));
                            pl2->giveItem(item);
                            std::string tips = lang.tr(PlayerLanguage, "market.gui.sell.sellItem.tips3");
                            tips = tool::replaceString(tips, "${item}", name);
                            pl2->sendTextPacket(tips);
                            db.removeMap(idString);
                            db.setTable("Item");
                            db.remove("XUID" + pl2->getXuid() + idString);
                            delete item;
                            break;
                        }
                    }
                    db.close();
                    lang.close();
                });
            });
        }

        void sellGui(Player* player) {
            std::string PlayerLanguage = tool::get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "market.gui.title"), lang.tr(PlayerLanguage, "market.gui.label"));
            form.addButton(lang.tr(PlayerLanguage, "market.gui.sell.sellItem"), "textures/items/diamond");
            form.addButton(lang.tr(PlayerLanguage, "market.gui.sell.sellItemContent"), "textures/items/diamond_axe");
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
                        sellItemGui(pl);
                        break;
                    case 1:
                        sellItemContentGui(pl);
                        break;
                }
            });
        }

        class MarketCommand : public Command {
            enum MARKETOP : int {
                gui = 3,
                sell = 8
            } op;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    switch (op) {
                        case MARKETOP::gui: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Market: No player selected.");
                                break;
                            }
                            buyGui(ori.getPlayer());
                            std::string playerName = ori.getName();
                            outp.success("The UI has been opened to player " + playerName);
                            break;
                        }
                        case MARKETOP::sell: {
                            if (ori.getPlayer() == nullptr) {
                                outp.error("Market: No player selected.");
                                break;
                            }
                            sellGui(ori.getPlayer());
                            std::string playerName = ori.getName();
                            outp.success("The UI has been opened to player " + playerName);
                            break;
                        }
                        default:
                            logger.error("<Market>: 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Market: Instruction error.");
                            break;
                    }
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("market", "§e§lLOICollection -> §b玩家市场", CommandPermissionLevel::Any, {(CommandFlagValue)0},{(CommandFlagValue)0x80});
                    registry->addEnum<MARKETOP>("gui", {{"gui", MARKETOP::gui}});
                    registry->addEnum<MARKETOP>("sell", {{"sell", MARKETOP::sell}});
                    registry->registerOverload<MarketCommand>("market", makeMandatory<CommandParameterDataType::ENUM>(&MarketCommand::op, "op", "gui"));
                    registry->registerOverload<MarketCommand>("market", makeMandatory<CommandParameterDataType::ENUM>(&MarketCommand::op, "op", "sell"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                MarketCommand::setup(e.mCommandRegistry);
                return true;
            });
            Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& e) {
                if (!tool::isBlacklist(e.mPlayer)) {
                    Player* player = e.mPlayer;
                    SQLiteDatabase db(PluginData + "/market.db");
                    if (db.existsTable("XUID" + player->getXuid())) {
                        db.setTable("XUID" + player->getXuid());
                        int score = std::stoi(db.get("Money"));
                        if (score > 0) {
                            tool::llmoney::add(player, score);
                            db.set("Money", "0");
                        }
                    } else {
                        db.setTable("XUID" + player->getXuid());
                        db.createTable();
                        db.set("Money", "0");
                        db.setTable("XUID" + player->getXuid() + "LIST");
                        db.createTable();
                    }
                    db.close();
                }
                return true;
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        if (!std::filesystem::exists(PluginData + "/market.db")) {
            SQLiteDatabase db(PluginData + "/market.db");
            db.close();
        }
        listen();
    }
}