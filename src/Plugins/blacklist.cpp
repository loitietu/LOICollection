#include <string>
#include <vector>
#include <ctime>
#include <llapi/FormUI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/RegCommandAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Types.hpp>
#include <llapi/mc/ServerPlayer.hpp>
#include <llapi/mc/Player.hpp>
#include "../Storage/SQLiteDatabase.h"
#include "include/i18nLang.h"
#include "include/blacklist.h"
extern Logger logger;
const std::string PluginData = "./plugins/LOICollection/data";

namespace blacklist {
    namespace {
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

        std::vector<std::string> split(const std::string& s, char delimiter) {
            std::vector<std::string> tokens;
            std::stringstream ss(s);
            std::string token;
            while (std::getline(ss, token, delimiter)) {
                tokens.push_back(token);
            }
            return tokens;
        }

        std::string timeCalculate(int hours) {
            std::time_t currentTime = std::time(nullptr);
            std::tm* timeInfo = std::localtime(&currentTime);
            timeInfo->tm_hour += 1;
            std::time_t laterTime = std::mktime(timeInfo);
            char formattedTime[15];
            std::strftime(formattedTime, sizeof(formattedTime), "%Y%m%d%H%H%S", std::localtime(&laterTime));
            std::string formattedTimeString(formattedTime);
            return formattedTimeString;
        }

        bool isReach(const std::string& timeString) {
            std::time_t currentTime = std::time(nullptr);
            char formattedTime[15];
            std::strftime(formattedTime, sizeof(formattedTime), "%Y%m%d%H%H%S", std::localtime(&currentTime));
            std::string formattedTimeString(formattedTime);
            int64_t formattedTimeInt = std::stoll(formattedTimeString);
            int64_t timeInt = std::stoll(timeString);
            if (formattedTimeInt > timeInt) return true;
            else return false;
        }

        void database() {
            if (!std::filesystem::exists(PluginData + "/blacklist.db")) {
                logger.info("<Blacklist>: 数据库已创建");
                SQLiteDatabase db(PluginData + "/blacklist.db");
                db.close();
            }
        }

        void addGui(Player* player) {
            std::vector<Player*> playerList =  Level::getAllPlayers();
            std::vector<std::string> playerListName;
            for (auto p : playerList) playerListName.push_back(p->getName());
            std::string PlayerLanguage = get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            std::vector<std::string> typeList = { "ip", "xuid" };
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "blacklist.gui.add.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "blacklist.gui.label")));
            form.append(Form::Dropdown("dropdown1", lang.tr(PlayerLanguage, "blacklist.gui.add.dropdown1"), playerListName));
            form.append(Form::Dropdown("dropdown2", lang.tr(PlayerLanguage, "blacklist.gui.add.dropdown2"), typeList));
            form.append(Form::Input("input1", lang.tr(PlayerLanguage, "blacklist.gui.add.input1"), "", lang.tr(PlayerLanguage, "blacklist.cause")));
            form.append(Form::Input("input2", lang.tr(PlayerLanguage, "blacklist.gui.add.input2"), "", "0"));
            lang.close();
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                std::string PlayerLanguage = get(pl);
                i18nLang lang("./plugins/LOICollection/language.json");
                if (mp.empty()) {
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string PlayerSelectName = mp["dropdown1"]->getString();
                std::string PlayerSelectType = mp["dropdown2"]->getString();
                std::string PlayerInputCause = mp["input1"]->getString();
                std::string PlayerInputTime = mp["input2"]->getString();
                pl->runcmd("blacklist add " + PlayerSelectType + " " + PlayerSelectName + " " + PlayerInputCause + " " + PlayerInputTime);
            });
        }

        void removeGui(Player* player) {
            SQLiteDatabase db(PluginData + "/blacklist.db");
            std::string PlayerLanguage = get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::CustomForm(lang.tr(PlayerLanguage, "blacklist.gui.remove.title"));
            form.append(Form::Label("label", lang.tr(PlayerLanguage, "blacklist.gui.label")));
            form.append(Form::Dropdown("dropdown", lang.tr(PlayerLanguage, "blacklist.gui.remove.dropdown"), db.list()));
            lang.close();
            db.close();
            form.sendTo(player, [](Player* pl, std::map<std::string, std::shared_ptr<Form::CustomFormElement>> mp) {
                std::string PlayerLanguage = get(pl);
                i18nLang lang("./plugins/LOICollection/language.json");
                if (mp.empty()) {
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                std::string PlayerSelectString = mp["dropdown"]->getString();
                pl->runcmd("blacklist remove " + PlayerSelectString);
            });
        }

        void menuGui(ServerPlayer* player) {
            std::string PlayerLanguage = get(player);
            i18nLang lang("./plugins/LOICollection/language.json");
            auto form = Form::SimpleForm(lang.tr(PlayerLanguage, "blacklist.gui.title"), lang.tr(PlayerLanguage, "blacklist.gui.label"));
            form.addButton(lang.tr(PlayerLanguage, "blacklist.gui.addBlacklist"), "textures/ui/backup_replace");
            form.addButton(lang.tr(PlayerLanguage, "blacklist.gui.removeBlacklist"), "textures/ui/free_download_symbol");
            lang.close();
            form.sendTo(player, [](Player* pl, int id) {
                if (id == -1) {
                    std::string PlayerLanguage = get(pl);
                    i18nLang lang("./plugins/LOICollection/language.json");
                    pl->sendTextPacket(lang.tr(PlayerLanguage, "exit"));
                    lang.close();
                    return;
                }
                switch(id) {
                    case 0:
                        addGui(pl);
                        break;
                    case 1:
                        removeGui(pl);
                        break;
                }
            });
        }

        class BlacklistCommand : public Command {
            std::string cause = "";
            std::string PlayerString = "";
            int time = 0;
            enum BLACKLISTOP : int {
                add = 1,
                remove = 2,
                list = 3,
                gui = 4
            } op;
            enum BLACKLISTYPE : int {
                xuid = 1,
                ip = 2
            } BlacklisType;
            CommandSelector<Player> target;
            public:
                void execute(CommandOrigin const& ori, CommandOutput& outp) const {
                    SQLiteDatabase db(PluginData + "/blacklist.db");
                    i18nLang lang("./plugins/LOICollection/language.json");
                    auto res = target.results(ori);
                    switch (op) {
                        case BLACKLISTOP::add:
                            switch (BlacklisType) {
                                case BLACKLISTYPE::xuid:
                                    for (auto i : res) {
                                        std::string xuid = i->getXuid();
                                        std::string BlackCause = cause;
                                        std::string timeString = std::to_string(time);
                                        if (cause.empty()) BlackCause = lang.tr(get(i), "blacklist.cause");
                                        if (time) timeString = timeCalculate(time);
                                        if (!db.existsTable("XUID" + xuid)) {
                                            db.setTable("XUID" + xuid);
                                            db.createTable();
                                            db.set("Cause", BlackCause);
                                            db.set("Time", timeString);
                                        }
                                        i->kick(BlackCause);
                                    }
                                    outp.success("Blacklist: Adding players succeeded.");
                                    break;
                                case BLACKLISTYPE::ip:
                                    for (auto i : res) {
                                        std::string mIp = split(i->getIP(), ':')[0];
                                        std::replace(mIp.begin(), mIp.end(), '.', 'A');
                                        std::string BlackCause = cause;
                                        std::string timeString = std::to_string(time);
                                        if (cause.empty()) BlackCause = lang.tr(get(i), "blacklist.cause");
                                        if (time) timeString = timeCalculate(time);
                                        if (!db.existsTable("IP" + mIp)) {
                                            db.setTable("IP" + mIp);
                                            db.createTable();
                                            db.set("Cause", BlackCause);
                                            db.set("Time", timeString);
                                        }
                                        i->kick(BlackCause);
                                    }
                                    outp.success("Blacklist: Adding players succeeded.");
                                    break;
                                default:
                                    outp.error("Blacklist: Failed to add player.");
                                    break;
                            }
                            break;
                        case BLACKLISTOP::remove:
                            if (PlayerString != "data") {
                                if (db.existsTable(PlayerString)) {
                                    db.removeTable(PlayerString);
                                    outp.success("Blacklist: Data table " + PlayerString + " has been successfully deleted.");
                                } else {
                                    outp.error("Blacklist: There is no data table " + PlayerString);
                                }
                            } else {
                                outp.error("Blacklist: Deleting data table data is prohibited.");
                            }
                            break;
                        case BLACKLISTOP::list: {
                            std::vector<std::string> listPlayer = db.list();
                            std::stringstream ss;
                            for (const auto& pl : listPlayer) ss << pl << ",";
                            outp.success("Blacklist: Add list - " + ss.str());
                            break;
                        }
                        case BLACKLISTOP::gui: {
                            std::string playerName = ori.getName();
                            menuGui(ori.getPlayer());
                            outp.success("The UI has been opened to player " + playerName);
                            break;
                        }
                        default:
                            logger.error("<Blacklist>: 命令分支 " + std::to_string(op) + " 不存在");
                            outp.error("Blacklist: Instruction error.");
                            break;
                    }
                    lang.close();
                    db.close();
                }

                static void setup(CommandRegistry* registry) {
                    using RegisterCommandHelper::makeMandatory;
                    registry->registerCommand("blacklist", "§e§lLOICollection -> §b服务器黑名单", CommandPermissionLevel::GameMasters, {(CommandFlagValue)0},{(CommandFlagValue)0x200});
                    registry->addEnum<BLACKLISTOP>("gui", {{"gui", BLACKLISTOP::gui}});
                    registry->addEnum<BLACKLISTOP>("add", {{"add", BLACKLISTOP::add}});
                    registry->addEnum<BLACKLISTOP>("remove", {{"remove", BLACKLISTOP::remove}});
                    registry->addEnum<BLACKLISTOP>("list", {{"list", BLACKLISTOP::list}});
                    registry->addEnum<BLACKLISTYPE>("type", {{"xuid", BLACKLISTYPE::xuid},{"ip", BLACKLISTYPE::ip}});
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "gui"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "list"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "add"), makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::BlacklisType, "type", "type"), makeMandatory(&BlacklistCommand::target, "player"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "add"), makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::BlacklisType, "type", "type"), makeMandatory(&BlacklistCommand::target, "player"), makeMandatory(&BlacklistCommand::time, "time"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "add"), makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::BlacklisType, "type", "type"), makeMandatory(&BlacklistCommand::target, "player"), makeMandatory(&BlacklistCommand::cause, "cause"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "add"), makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::BlacklisType, "type", "type"), makeMandatory(&BlacklistCommand::target, "player"), makeMandatory(&BlacklistCommand::cause, "cause"), makeMandatory(&BlacklistCommand::time, "time"));
                    registry->registerOverload<BlacklistCommand>("blacklist", makeMandatory<CommandParameterDataType::ENUM>(&BlacklistCommand::op, "op", "remove"), makeMandatory(&BlacklistCommand::PlayerString, "string"));
                }
        };

        void listen() {
            Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& e) {
                BlacklistCommand::setup(e.mCommandRegistry);
                return true;
            });
            Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& e) {
                SQLiteDatabase db(PluginData + "/blacklist.db");
                std::string xuid = e.mPlayer->getXuid();
                std::string ip = split(e.mPlayer->getIP(), ':')[0];
                std::replace(ip.begin(), ip.end(), '.', 'A');
                if (db.existsTable("XUID" + xuid)) {
                    db.setTable("XUID" + xuid);
                    std::string timeString = db.get("Time");
                    if (timeString == "0") {
                        e.mPlayer->kick(db.get("Cause"));
                        db.close();
                        return false;
                    }
                    if (isReach(timeString)) {
                        db.removeTable("XUID" + xuid);
                        db.close();
                        return true;
                    } else {
                        e.mPlayer->kick(db.get("Cause"));
                        db.close();
                        return false;
                    }
                } else if(db.existsTable("IP" + ip)) {
                    db.setTable("IP" + ip);
                    std::string timeString = db.get("Time");
                    if (timeString == "0") {
                        e.mPlayer->kick(db.get("Cause"));
                        db.close();
                        return false;
                    }
                    if (isReach(timeString)) {
                        db.removeTable("IP" + ip);
                        db.close();
                        return true;
                    } else {
                        e.mPlayer->kick(db.get("Cause"));
                        db.close();
                        return false;
                    }
                } else {
                    db.close();
                    return true;
                }
            });
        }
    }

    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        listen();
        database();
        logger.info("<Blacklist>: 插件已加载");
    }
}