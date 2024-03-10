/**
 * @file plugin.cpp
 * @note Plugin Main
 * @author Tietu
 * @copyright Copyright (C) 2024 Tietu
 */

#include <string>
#include <fstream>
#include <unordered_map>
#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/DynamicCommandAPI.h>
#include <Nlohmann/json.hpp>
#include "Plugins/include/language.h"
#include "Plugins/include/blacklist.h"
#include "Plugins/include/mute.h"
#include "Plugins/include/cdk.h"
#include "Plugins/include/menu.h"
#include "Plugins/include/tpa.h"
#include "Plugins/include/shop.h"
#include "Plugins/include/monitor.h"
#include "Plugins/include/pvp.h"
#include "Plugins/include/wallet.h"
#include "Plugins/include/chat.h"
#include "Plugins/include/announcement.h"
#include "Plugins/include/market.h"
#include "utils/internet.h"
#include "utils/tool.h"
#include "version.h"
#include "API.h"
#include "lang.h"
extern Logger logger;
namespace fs = std::filesystem;
const std::string PluginDirectory = "./plugins/LOICollection";
bool blacklistPlugin, mutePlugin, cdkPlugin, menuPlugin, tpaPlugin, shopPlugin, monitorPlugin, pvpPlugin, walletPlugin, chatPlugin, announcementPlugin, marketPlugin = false;
int64_t FakeSeed = 0;

//Update Version data
void update(const std::string* versionInfo) {
    logger.info("开始加载配置文件");
    std::ifstream configFile(PluginDirectory + "/config.json");
    nlohmann::ordered_json config;
    nlohmann::ordered_json configArray = nlohmann::ordered_json::array();
    configFile >> config;
    configFile.close();
    if (!config.contains("FakeSeed")) config["FakeSeed"] = 114514;
    if (!config.contains("Blacklist")) config["Blacklist"] = false;
    if (!config.contains("Mute")) config["Mute"] = false;
    if (!config.contains("Cdk")) config["Cdk"] = false;
    if (!config.contains("Menu")) config["Menu"] = {{"Enable", false},{"ItemId", "minecraft:clock"}};
    if (!config.contains("Tpa")) config["Tpa"] = false;
    if (!config.contains("Shop")) config["Shop"] = false;
    if (!config.contains("Monitor")) config["Monitor"] = {{"Enable", false},{"join", "{player}加入了服务器"},{"left", "{player}退出了服务器"},{"llmoney", "§e§l检测到LLMoney发生变化 §b原值: §f${OriMoney} §a更改: §f${SetMoney} §e现值: §f${GetMoney}"},{"command", configArray},{"tips", "该指令已被禁用"}};
    if (!config.contains("Pvp")) config["Pvp"] = false;
    if (!config.contains("Wallet")) config["Wallet"] = {{"Enable", false},{"llmoney", true},{"score", "money"},{"tax", 0.1}};
    if (!config.contains("Chat")) config["Chat"] = {{"Enable", false},{"chat","<{player}> ${chat}"}};
    if (!config.contains("AnnounCement")) config["AnnounCement"] = false;
    if (!config.contains("Market")) config["Market"] = false;
    std::string configVersion = config["version"].template get<std::string>();
    if (configVersion != (*versionInfo)) {
        logger.info("配置文件版本: " + configVersion);
        logger.info("插件版本: " + (*versionInfo));
        config["version"] = (*versionInfo);
        nlohmann::ordered_json langData;
        std::ifstream languageFile(PluginDirectory + "/language.json");
        languageFile >> langData;
        languageFile.close();
        langData["zh_CN"] = CNLangData;
        std::ofstream languageNewFile(PluginDirectory + "/language.json");
        languageNewFile << langData.dump(4);
        languageNewFile.close();
        langData.clear();
        logger.info("数据文件已更新");
    }
    blacklistPlugin = config["Blacklist"].template get<bool>();
    mutePlugin = config["Mute"].template get<bool>();
    cdkPlugin = config["Cdk"].template get<bool>();
    menuPlugin = config["Menu"]["Enable"].template get<bool>();
    tpaPlugin = config["Tpa"].template get<bool>();
    shopPlugin = config["Shop"].template get<bool>();
    monitorPlugin = config["Monitor"]["Enable"].template get<bool>();
    pvpPlugin = config["Pvp"].template get<bool>();
    walletPlugin = config["Wallet"]["Enable"].template get<bool>();
    chatPlugin = config["Chat"]["Enable"].template get<bool>();
    announcementPlugin = config["AnnounCement"].template get<bool>();
    marketPlugin = config["Market"].template get<bool>();
    FakeSeed = config["FakeSeed"].template get<int64_t>();
    std::ofstream configNewFile(PluginDirectory + "/config.json");
    configNewFile << config.dump(4);
    configNewFile.close();
    config.clear();
}

//Initialize the plugin
void Init(const std::string* versionInfo) {
    if (!fs::exists(PluginDirectory)) {
        logger.info("初次运行，正在初始化插件");
        fs::create_directory(PluginDirectory);
        fs::create_directory(PluginDirectory + "/data");
        nlohmann::ordered_json config;
        config["version"] = (*versionInfo);
        std::ofstream configFile(PluginDirectory + "/config.json");
        configFile << config.dump(4);
        configFile.close();
        config.clear();
        std::ofstream languageFile(PluginDirectory + "/language.json");
        languageFile << defaultLangData.dump(4);
        languageFile.close();
        logger.info("初始化成功!");
    }
    update(&(*versionInfo));
}

//Load built-in data features
void loadBuilt(const std::string versionInfo) {
    int OpenPlugin = 0;
    language::load(&OpenPlugin);
    if (blacklistPlugin) blacklist::load(&OpenPlugin);
    if (mutePlugin) mute::load(&OpenPlugin);
    if (cdkPlugin) cdk::load(&OpenPlugin);
    if (menuPlugin) menu::load(&OpenPlugin);
    if (tpaPlugin) tpa::load(&OpenPlugin);
    if (shopPlugin) shop::load(&OpenPlugin);
    if (monitorPlugin) monitor::load(&OpenPlugin);
    if (pvpPlugin) pvp::load(&OpenPlugin);
    if (walletPlugin) wallet::load(&OpenPlugin);
    if (chatPlugin) chat::load(&OpenPlugin);
    if (announcementPlugin) announcement::load(&OpenPlugin);
    if (marketPlugin) market::load(&OpenPlugin);
    logger.info("加载成功，已加载内置插件数量: " + std::to_string(OpenPlugin));
    Event::ServerStartedEvent::subscribe([](const Event::ServerStartedEvent& e) {
        LOICollectionAPI::init();
        return true;
    });
    auto UpgradeCommand = DynamicCommand::createCommand("upgrade", "Update plug-in", CommandPermissionLevel::GameMasters);
    auto& UpgradeCommandEnum = UpgradeCommand->setEnum("data", { "plugin" });
    UpgradeCommand->mandatory("UpgradeCommandEnum", DynamicCommand::ParameterType::Enum, UpgradeCommandEnum);
    UpgradeCommand->addOverload({ UpgradeCommandEnum });
    UpgradeCommand->setCallback([versionInfo](DynamicCommand const& command, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>& results) {
        auto action = results["UpgradeCommandEnum"].get<std::string>();
        switch (do_hash(action.c_str())) {
            case do_hash("plugin"):
                internet::LOICollection::checkForUpdates(versionInfo);
                break;
            default:
                logger.error("Upgrade >> 没有指定的分支.");
                output.error("No specified branch.");
                break;
        }
    });
    DynamicCommand::setup(std::move(UpgradeCommand));
}

//Plug-in entry
void PluginInit() {
    std::stringstream ss;
    ss << PLUGIN_VERSION_MAJOR << "." << PLUGIN_VERSION_MINOR << "." << PLUGIN_VERSION_REVISION;
    std::string versionString = ss.str();
    logger.info("感谢您使用本插件，版本:" + versionString + "，作者:贴图");
    internet::LOICollection::checkForUpdates(versionString);
    Init(&versionString);
    loadBuilt(versionString);
}

/**
 * @Plugin FakeSeed
 * @Reference repositories: https://github.com/LiteLDev/LLEssentials
 */
THook(void*, "?write@StartGamePacket@@UEBAXAEAVBinaryStream@@@Z", void* a, void* b) {
    if (FakeSeed) dAccess<int64_t, 48>(a) = FakeSeed;
    return original(a, b);
}