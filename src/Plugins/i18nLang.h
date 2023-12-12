#ifndef LOICOLLECTION_I18NLANG_H
#define LOICOLLECTION_I18NLANG_H

#include <string>
#include <vector>
#include <Nlohmann/json.hpp>

class i18nLang {
private:
    nlohmann::json language;

public:
    i18nLang(const std::string& path);
    std::string tr(const std::string& lang, const std::string& title);
    std::vector<std::string> list();
    void close();
};

#endif