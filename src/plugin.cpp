#include <fstream>
#include <string>
#include <llapi/LoggerAPI.h>
#include <Nlohmann/json.hpp>
#include "Plugins/include/language.h"
#include "Plugins/include/blacklist.h"
#include "Plugins/include/mute.h"
#include "lang.h"
#include "version.h"
extern Logger logger;
const std::string PluginDirectory = "./plugins/LOICollection";
bool blacklistPlugin = false;
bool mutePlugin = false;

void update(std::string& versionInfo) {
    logger.info("开始加载配置文件");
    std::ifstream configFile(PluginDirectory + "/config.json");
    nlohmann::ordered_json config;
    nlohmann::ordered_json configArray = nlohmann::ordered_json::array();
    configFile >> config;
    configFile.close();
    if (!config.contains("Blacklist")) config["Blacklist"] = false;
    if (!config.contains("Mute")) config["Mute"] = false;
    if (!config.contains("Cdk")) config["Cdk"] = false;
    if (!config.contains("Menu")) config["Menu"] = {{"Enable", false},{"ItemId", "minecraft:clock"}};
    if (!config.contains("Tpa")) config["Tpa"] = false;
    if (!config.contains("Shop")) config["Shop"] = false;
    if (!config.contains("Monitor")) config["Monitor"] = false;
    if (!config.contains("Pvp")) config["Pvp"] = false;
    if (!config.contains("Wallet")) config["Wallet"] = {{"Enable", false},{"llmoney", true},{"score", "money"},{"tax", 0.1}};
    if (!config.contains("Chat")) config["Chat"] = {{"Enable", false},{"chat","<{player}> {chat}"}};
    if (!config.contains("Command")) config["Command"] = {{"Enable", false},{"command", configArray},{"tips", "该指令已被禁用"}};
    if (!config.contains("ServerTips")) config["ServerTips"] = {{"Enable", false},{"join", "{player}加入了服务器"},{"left", "{player}退出了服务器"}};
    if (!config.contains("AnnounCement")) config["AnnounCement"] = false;
    std::string configVersion = config["version"].template get<std::string>();
    if (configVersion != versionInfo) {
        logger.info("配置文件版本: " + configVersion);
        logger.info("插件版本: " + versionInfo);
        config["version"] = versionInfo;
        logger.info("配置文件已更新");
    }
    blacklistPlugin = config["Blacklist"].template get<bool>();
    mutePlugin = config["Mute"].template get<bool>();
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
    logger.info("加载成功，已加载内置插件数量: " + std::to_string(OpenPlugin));
}

void PluginInit() {
    std::stringstream ss;
    ss << PLUGIN_VERSION_MAJOR << "." << PLUGIN_VERSION_MINOR << "." << PLUGIN_VERSION_REVISION;
    std::string versionString = ss.str();
    logger.info("感谢您使用本插件，版本:" + versionString + "，作者:贴图");
    Init(versionString);
    load();
}
