#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "JsonManager.h"

JsonManager::JsonManager(const std::string& filePath) {
    jsonFilePath = filePath;
    if (std::filesystem::exists(filePath)) {
        std::ifstream file(filePath);
        file >> json;
        file.close();
    } else {
        std::ofstream file(filePath);
        file << json.dump(4);
        file.close();
    }
}

void JsonManager::set(const std::string& key, const nlohmann::ordered_json& value) {
    json[key] = value;
}

void JsonManager::setString(const std::string& key, const std::string& value) {
    json[key] = value;
}

void JsonManager::remove(const std::string& key) {
    json.erase(key);
}

nlohmann::ordered_json JsonManager::get(const std::string& key) {
    if (!json.contains(key)) {
        return nullptr;
    }
    return json[key].template get<nlohmann::ordered_json>();
}

std::string JsonManager::getString(const std::string& key) {
    return json[key].template get<std::string>();
}

int JsonManager::getInt(const std::string& key) {
    return json[key].template get<int>();
}

bool JsonManager::getBool(const std::string& key) {
    return json[key].template get<bool>();
}

bool JsonManager::isKey(const std::string& key) {
    return json.find(key) != json.end();
}

std::vector<std::string> JsonManager::list() {
    std::vector<std::string> keys;
    for (nlohmann::ordered_json::iterator it = json.begin(); it != json.end(); ++it) {
        keys.push_back(it.key());
    }
    return keys;
}

nlohmann::ordered_json JsonManager::toJson() {
    nlohmann::ordered_json data = json;
    clear();
    return data;
}

void JsonManager::save() {
    std::ofstream file(jsonFilePath);
    file << json.dump(4);
    file.close();
    clear();
}

void JsonManager::clear() {
    json.clear();
}