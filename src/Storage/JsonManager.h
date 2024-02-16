#ifndef LOICOLLECTION_JSONMANAGER_H
#define LOICOLLECTION_JSONMANAGER_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class JsonManager {
private:
    nlohmann::ordered_json json = nlohmann::ordered_json::object();
    std::string jsonFilePath;

public:
    JsonManager(const std::string& filePath);
    void set(const std::string& key, const nlohmann::ordered_json& value);
    void remove(const std::string& key);
    nlohmann::ordered_json get(const std::string& key);
    std::string getString(const std::string& key);
    int getInt(const std::string& key);
    bool getBool(const std::string& key);
    bool isKey(const std::string& key);
    std::vector<std::string> list();
    nlohmann::ordered_json toJson();
    void save();
    void clear();
};

#endif