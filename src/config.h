#ifndef LOICOLLECTION_CONFIG_H
#define LOICOLLECTION_CONFIG_H

#include <string>
#include <filesystem>

const std::string PluginDirectory = "./plugins/LOICollection";
const std::string PluginData = "./plugins/LOICollection/data";
std::filesystem::path PluginDataPath(PluginData);

#endif