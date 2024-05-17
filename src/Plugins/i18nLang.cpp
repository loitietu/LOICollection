#include <vector>
#include <fstream>
#include <filesystem>
#include <Nlohmann/json.hpp>
#include "include/i18nLang.h"

i18nLang::i18nLang(const std::string& path) {
    if (std::filesystem::exists(path)) {
        std::ifstream langFile(path);
        langFile >> language;
        langFile.close();
    }
}

std::string i18nLang::tr(const std::string& lang, const std::string& title) {
    if (language.contains(lang) && language[lang].contains(title)) {
        std::string langTitle = language[lang][title].template get<std::string>();
        return langTitle;
    } else {
        return "None";
    }
}

std::vector<std::string> i18nLang::list() {
    std::vector<std::string> languageList;
    for (nlohmann::ordered_json::iterator it = language.begin(); it != language.end(); ++it) {
        languageList.push_back(it.key());
    }
    return languageList;
}

void i18nLang::close() {
    language.clear();
}