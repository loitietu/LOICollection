#include <fstream>
#include <string>
#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
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
#include "Plugins/include/chat.h"
#include "API.h"
#include "lang.h"
#include "version.h"
extern Logger logger;
const std::string PluginDirectory = "./plugins/LOICollection";
bool blacklistPlugin, mutePlugin, cdkPlugin, menuPlugin, tpaPlugin, shopPlugin, monitorPlugin, pvpPlugin, chatPlugin = false;
int64_t FakeSeed = 0;

void update(std::string& versionInfo) {
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
    if (!config.contains("Monitor")) config["Monitor"] = {{"Enable", false},{"join", "{player}加入了服务器"},{"left", "{player}退出了服务器"},{"llmoney", "§e§l检测到金钱发生变化 §b原值: §f${OriMoney} §a更改: §f${SetMoney} §e现值: §f${GetMoney}"}};
    if (!config.contains("Pvp")) config["Pvp"] = false;
    if (!config.contains("Wallet")) config["Wallet"] = {{"Enable", false},{"llmoney", true},{"score", "money"},{"tax", 0.1}};
    if (!config.contains("Chat")) config["Chat"] = {{"Enable", false},{"chat","<{player}> ${chat}"}};
    if (!config.contains("Command")) config["Command"] = {{"Enable", false},{"command", configArray},{"tips", "该指令已被禁用"}};
    if (!config.contains("AnnounCement")) config["AnnounCement"] = false;
    std::string configVersion = config["version"].template get<std::string>();
    if (configVersion != versionInfo) {
        logger.info("配置文件版本: " + configVersion);
        logger.info("插件版本: " + versionInfo);
        config["version"] = versionInfo;
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
    chatPlugin = config["Chat"]["Enable"].template get<bool>();
    FakeSeed = config["FakeSeed"].template get<int64_t>();
    std::ofstream configNewFile(PluginDirectory + "/config.json");
    configNewFile << config.dump(4);
    configNewFile.close();
    config.clear();
}

void Init(std::string& versionInfo) {
    std::filesystem::path dir(PluginDirectory);
    if (!std::filesystem::exists(dir)) {
        logger.info("初次运行，正在初始化插件");
        std::filesystem::create_directory(dir);
        std::filesystem::create_directory(dir.append("data"));
        nlohmann::ordered_json config;
        config["version"] = versionInfo;
        std::ofstream configFile(PluginDirectory + "/config.json");
        configFile << config.dump(4);
        configFile.close();
        config.clear();
        std::ofstream languageFile(PluginDirectory + "/language.json");
        languageFile << defultLangData.dump(4);
        languageFile.close();
        logger.info("初始化成功!");
    }
    update(versionInfo);
}

void load() {
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
    if (chatPlugin) chat::load(&OpenPlugin);
    logger.info("加载成功，已加载内置插件数量: " + std::to_string(OpenPlugin));
    Event::ServerStartedEvent::subscribe([](const Event::ServerStartedEvent& e) {
        LOICollectionAPI::init();
        return true;
    });
}

void PluginInit() {
    std::stringstream ss;
    ss << PLUGIN_VERSION_MAJOR << "." << PLUGIN_VERSION_MINOR << "." << PLUGIN_VERSION_REVISION;
    std::string versionString = ss.str();
    logger.info("感谢您使用本插件，版本:" + versionString + "，作者:贴图");
    Init(versionString);
    load();
}

/*
* Plugin: FakeSeed
* @ Reference repositories: https://github.com/LiteLDev/LLEssentials
*/
THook(void*, "?write@StartGamePacket@@UEBAXAEAVBinaryStream@@@Z", void* a, void* b) {
    if (FakeSeed) dAccess<int64_t, 48>(a) = FakeSeed;
    return original(a, b);
}