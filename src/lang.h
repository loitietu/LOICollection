#ifndef LOICOLLECTION_LANG_H
#define LOICOLLECTION_LANG_H

#include <Nlohmann/json.hpp>

static const nlohmann::ordered_json CNLangData = {
    {"exit", "§e[LOICollection] §c已退出"},
    {"language.log", "language >> 玩家 {player} 将语言更改为 {language}"},
    {"language.gui.title", "§e§lLOICollection -> §a语言设置"},
    {"language.gui.label", "§a个人语言设置"},
    {"language.gui.dropdown", "§e语言选择"},
    {"blacklist.log1", "Blacklist >> 已将玩家 {player} 添加进黑名单"},
    {"blacklist.log2", "Blacklist >> 已将 ${blacklist} 从黑名单中移除"},
    {"blacklist.log3", "Blacklist >> 玩家 {player} 尝试加入服务器，等待客户端初始化后移除"},
    {"blacklist.cause", "§4您已被服务器加入黑名单，§a请联系管理员解决问题"},
    {"blacklist.gui.title", "§e§lLOICollection -> §b服务器黑名单"},
    {"blacklist.gui.label", "§a功能如下:"},
    {"blacklist.gui.addBlacklist", "§c添加黑名单\n§4将玩家添加至黑名单"},
    {"blacklist.gui.removeBlacklist", "§a删除黑名单\n§4将玩家从黑名单中移除"},
    {"blacklist.gui.add.title", "§eLOIBlacklist §b-> §c添加黑名单"},
    {"blacklist.gui.add.dropdown1", "请选择添加至黑名单玩家:"},
    {"blacklist.gui.add.dropdown2", "请选择添加至黑名单类型:"},
    {"blacklist.gui.add.input1", "请输入加入黑名单原因:"},
    {"blacklist.gui.add.input2", "请输入加入黑名单时间[/hour](0为永久加入)"},
    {"blacklist.gui.remove.title", "§eLOIBlacklist §b-> §a删除黑名单"},
    {"blacklist.gui.remove.dropdown", "请选择删除黑名单对象:"},
    {"mute.log1", "Mute >> 玩家 {player} 被禁言，原因: ${cause}"},
    {"mute.log2", "Mute >> 玩家 {player} 已解除禁言"},
    {"mute.log3", "Mute >> 玩家 {player} 已被禁言，但仍在尝试发送内容: ${message}"},
    {"mute.cause", "§4您已被禁言，无法发送消息"},
    {"mute.gui.title", "§eLOIMute §b-> §c服务器禁言"},
    {"mute.gui.label", "§a功能如下:"},
    {"mute.gui.addMute", "§c添加禁言\n§4将玩家添加至禁言"},
    {"mute.gui.removeMute", "§a删除禁言\n§4将玩家从禁言中移除"},
    {"mute.gui.add.title", "§eLOIMute §b-> §c添加禁言"},
    {"mute.gui.add.dropdown", "请选择添加至禁言玩家:"},
    {"mute.gui.add.input1", "请输入禁言原因:"},
    {"mute.gui.add.input2", "请输入禁言时间[/hour](0为永久禁言)"},
    {"mute.gui.remove.title", "§eLOIMute §b-> §a删除禁言"},
    {"mute.gui.remove.dropdown", "请选择删除禁言对象:"},
    {"cdk.log1", "Cdk >> 新的CDK: ${cdk} 已创建"},
    {"cdk.log2", "Cdk >> CDK: ${cdk} 已被删除"},
    {"cdk.log3", "Cdk >> 玩家 {player} 总换CDK: ${cdk}"},
    {"cdk.convert.tip1", "§4CDK不存在，无法总换"},
    {"cdk.convert.tip2", "§4总换失败，该总换码已经使用过了"},
    {"cdk.convert.tip3", "§a总换成功"},
    {"cdk.gui.title", "§eLOICdk §b-> §a总换码"},
    {"cdk.gui.label", "§b功能如下:"},
    {"cdk.gui.convert.input", "§e请输入CDK"},
    {"cdk.gui.addCdk", "§a创建CDK\n§4添加一个空白CDK"},
    {"cdk.gui.removeCdk", "§4删除CDK\n§4删除一个CDK"},
    {"cdk.gui.addAward", "§a添加奖励\n§4将奖励添加至CDK中"},
    {"cdk.gui.new.input1", "§e请输入CDK"},
    {"cdk.gui.new.input2", "§aLLmoney金钱奖励"},
    {"cdk.gui.new.input3", "§bCDK存在时间[/hour](0为永久存在)"},
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
    {"cdk.gui.award.item.input4", "§e请输入物品特殊值"},
    {"tpa.log", "Tpa >> 已将玩家 ${player1} 传送到玩家 ${player2}"},
    {"tpa.here", "§e玩家 §b{player} §e邀请你传送到他的位置"},
    {"tpa.there", "§e玩家 §f{player} §e请求传送到你的位置"},
    {"tpa.yes", "§a同意传送"},
    {"tpa.no", "§c拒绝传送"},
    {"tpa.no.tips", "§c对方拒绝了你的请求"},
    {"tpa.gui.title", "§eLOITpa §b-> §a玩家互传"},
    {"tpa.gui.label", "§b功能如下:"},
    {"tpa.gui.dropdown1", "§e请选择玩家:"},
    {"tpa.gui.dropdown2", "§b请选择类型:"},
    {"tpa.gui.setting.title", "§eLOITpa §b-> §a个人设置"},
    {"tpa.gui.setting.label", "§b功能如下:"},
    {"tpa.gui.setting.switch1", "§b拒绝所有人的tpa请求"},
    {"pvp.log1", "Pvp >> 玩家 {player} 开启了PVP"},
    {"pvp.log2", "Pvp >> 玩家 {player} 关闭了PVP"},
    {"pvp.off1", "§4对方未开启PVP无法造成伤害"},
    {"pvp.off2", "§4你未开启PVP无法造成伤害"},
    {"pvp.gui.title", "§eLOIPvp §b-> §a服务器PVP"},
    {"pvp.gui.label", "§b功能如下:"},
    {"pvp.gui.on", "§4开启PVP"},
    {"pvp.gui.off", "§a关闭PVP"},
    {"wallet.log", "Wallet >> 玩家 ${player1} 向玩家 ${player2} 转账: ${money}"},
    {"wallet.showOff", "{player}炫出了自己的钱包: ${money}"},
    {"wallet.gui.title", "§eLOIWallet §b-> §a个人钱包"},
    {"wallet.gui.label", "§b拥有金钱: ${money}\n§e今日税率: ${tax}"},
    {"wallet.gui.stepslider", "§a请选择类型§r"},
    {"wallet.gui.stepslider.dropdown", "请选择玩家"},
    {"wallet.gui.stepslider.input", "请输入金额"},
    {"chat.gui.title", "§eLOIChat §b-> §a个人称号"},
    {"chat.gui.label", "§b功能如下:"},
    {"chat.gui.setTitle", "§a设置称号\n§4设置自己的称号显示"},
    {"chat.gui.setTitle.label", "§a当前配戴称号: §f{title}\n§e称号到期时间: §f{title.time}"},
    {"chat.gui.setTitle.dropdown", "§b请选择配戴称号"},
    {"chat.gui.manager.add", "§a添加称号\n§4添加称号给指定玩家"},
    {"chat.gui.manager.remove", "§c删除称号\n§4从玩家上删除指定称号"},
    {"chat.gui.manager.add.dropdown", "§a请选择要添加的玩家"},
    {"chat.gui.manager.add.input1", "§e请输入要添加的称号"},
    {"chat.gui.manager.add.input2", "§d请输入称号存在的时间(0为永久存在)"},
    {"chat.gui.manager.remove.dropdown1", "§a请选择要删除的玩家"},
    {"chat.gui.manager.remove.dropdown2", "§e请选择要删除的称号"},
    {"announcement.gui.title", "§eLOIAnnounCement §b-> §a公告系统"},
    {"announcement.gui.label", "§b功能如下:"},
    {"announcement.gui.setTitle", "§e设置标题"},
    {"announcement.gui.line", "§a第${index}行"},
    {"announcement.gui.addLine", "§b添加一行"},
    {"announcement.gui.removeLine", "§c删除一行"},
    {"market.gui.title", "§e§lLOIMarket -> §b玩家市场"},
    {"market.gui.label", "§b功能如下:"},
    {"market.gui.sell.introduce", "§b物品介绍: §r${introduce}\n§e物品价格: §r${llmoney}\n§a物品Nbt: §r${nbt}\n§d上传玩家: §r${player}"},
    {"market.gui.sell.sellItem", "§a售出物品\n§4售出自己的手持物品"},
    {"market.gui.sell.sellItemContent", "§b查看售出物品\n§4查看自己售出的物品"},
    {"market.gui.sell.sellItem.input1", "§e请输入物品名称"},
    {"market.gui.sell.sellItem.input2", "§e请输入物品图标路径"},
    {"market.gui.sell.sellItem.input3", "§e请输入物品介绍"},
    {"market.gui.sell.sellItem.input4", "§e请输入物品售卖LLMoney价格"},
    {"market.gui.sell.sellItem.tips1", "§4请手持物品"},
    {"market.gui.sell.sellItem.tips2", "§a您的物品§f${item}§a已售出"},
    {"market.gui.sell.sellItem.tips3", "§a物品§f${item}§a已下架"},
    {"market.gui.sell.sellItem.tips4", "§c您没有足够的分数购买这个物品"},
    {"market.gui.sell.sellItemContent.button1", "§c下架物品\n§4下架该物品"},
    {"market.gui.sell.buy.button1", "§e购买物品\n§4购买当前物品"},
    {"market.gui.sell.buy.button2", "§a预览物品\n§4预览当前物品"}
};

static const nlohmann::ordered_json defaultLangData = {
    {"zh_CN", CNLangData}
};

#endif