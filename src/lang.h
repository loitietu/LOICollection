#ifndef LOICOLLECTION_LANG_H
#define LOICOLLECTION_LANG_H

#include <Nlohmann/json.hpp>

static const nlohmann::ordered_json defultLangData = {
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
        {"blacklist.gui.remove.dropdown", "请选择删除黑名单对象:"},
        {"mute.cause", "§4您已被禁言，无法发送消息"},
        {"mute.gui.title", "§eLOIMute §b-> §c服务器禁言"},
        {"mute.gui.label", "§a功能如下:"},
        {"mute.gui.addMute", "§c添加禁言\n§4将玩家添加至禁言"},
        {"mute.gui.removeMute", "§a删除禁言\n§4将玩家从禁言中移除"},
        {"mute.gui.add.title", "§eLOIMute §b-> §c添加禁言"},
        {"mute.gui.add.dropdown", "请选择添加至禁言玩家:"},
        {"mute.gui.add.input1", "请输入禁言原因:"},
        {"mute.gui.add.input2", "请输入禁言时间[/day](0为永久禁言)"},
        {"mute.gui.remove.title", "§eLOIMute §b-> §a删除禁言"},
        {"mute.gui.remove.dropdown", "请选择删除禁言对象:"},
        {"cdk.convert.tip1", "§4CDK不存在，无法总换"},
        {"cdk.convert.tip2", "§4总换失败，该总换码已经使用过了"},
        {"cdk.convert.tip3", "§a总换成功"},
        {"cdk.gui.title", "§eLOICdk §b-> §a总换码"},
        {"cdk.gui.label", "§b功能如下:"},
        {"cdk.gui.convert.input", "§e请输入CDK"},
        {"cdk.gui.addCdk", "§a创建CDK\n§7添加一个空白CDK"},
        {"cdk.gui.removeCdk", "§4删除CDK\n§7删除一个CDK"},
        {"cdk.gui.addAward", "§a添加奖励\n§7将奖励添加至CDK中"},
        {"cdk.gui.new.input1", "§e请输入CDK"},
        {"cdk.gui.new.input2", "§aLLmoney金钱奖励"},
        {"cdk.gui.new.switch", "§e是否只能总换一次"},
        {"cdk.gui.remove.dropdown", "§b请选择要删除的CDK"},
        {"cdk.gui.award.score", "§a添加计分板奖励"},
        {"cdk.gui.award.item", "§e添加物品奖励"},
        {"cdk.gui.award.dropdown", "§a请选择要添加的CDK"},
        {"cdk.gui.award.score.input1", "§e请输入计分板名称"},
        {"cdk.gui.award.score.input2", "§b请输入奖励数量"},
        {"cdk.gui.award.item.input1", "§b请输入物品ID"},
        {"cdk.gui.award.item.input2", "§e请输入物品名称"},
        {"cdk.gui.award.item.input3", "§e请输入物品数量"},
        {"cdk.gui.award.item.input4", "§e请输入物品特殊值"}
    }}
};

#endif