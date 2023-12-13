#ifndef LOICOLLECTION_LANG_H
#define LOICOLLECTION_LANG_H

#include <Nlohmann/json.hpp>

static const nlohmann::json defultLangData = {
    {"zh_CN", {
        {"language.gui.title", "§e§lLOICollection -> §a语言设置"},
        {"language.gui.label", "§a个人语言设置"},
        {"language.gui.dropdown", "§e语言选择"},
        {"blacklist.cause", "§4您已被服务器加入黑名单\n§a请联系管理员解决问题"}
    }}
};

#endif