#include <fstream>
#include <vector>
#include "i18nLang.h"

i18nLang::i18nLang(const std::string& path) {
    std::ifstream langFile(path);
    langFile >> language;
    langFile.close();
}

std::string i18nLang::tr(const std::string& lang, const std::string& title) {
    std::string langTitle = language[lang][title].template get<std::string>();
    return langTitle;
}

std::vector<std::string> i18nLang::list() {
    std::vector<std::string> languageList;
    for (nlohmann::json::iterator it = language.begin(); it != language.end(); ++it) {
        languageList.push_back(it.key());
    }
    return languageList;
}

void i18nLang::close() {
    language.clear();
}