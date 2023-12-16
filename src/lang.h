#ifndef LOICOLLECTION_LANG_H
#define LOICOLLECTION_LANG_H

#include <Nlohmann/json.hpp>

static const nlohmann::json defultLangData = {
    {"zh_CN", {
        {"exit", "§e[LOICollection] §c已退出"},
        {"language.log", "玩家 {} 已更改语言为: {}"},
        {"language.gui.title", "§e§lLOICollection -> §a语言设置"},
        {"language.gui.label", "§a个人语言设置"},
        {"language.gui.dropdown", "§e语言选择"},
        {"blacklist.cause", "§4您已被服务器加入黑名单，§a请联系管理员解决问题"},
        {"blacklist.gui.title", "§e§lLOICollection -> §b服务器黑名单"},
        {"blacklist.gui.label", "§a功能如下:"},
        {"blacklist.gui.addBlacklist", "§c添加黑名单\n§4将玩家添加至黑名单"},
        {"blacklist.gui.removeBlacklist", "§a删除黑名单\n§4将玩家从黑名单中移除"},
        {"blacklist.gui.add.title", "§eLOIBlacklist §b-> §c添加黑名单"},
        {"blacklist.gui.add.dropdown1", "请选择添加至黑名单玩家:"},
        {"blacklist.gui.add.dropdown2", "请选择添加至黑名单类型:"},
        {"blacklist.gui.add.input1", "请输入加入黑名单原因:"},
        {"blacklist.gui.add.input2", "请输入加入黑名单时间[/day](0为永久加入)"},
        {"blacklist.gui.remove.title", "§eLOIBlacklist §b-> §a删除黑名单"},
        {"blacklist.gui.remove.dropdown", "请选择删除黑名单对象:"}
    }}
};

#endif