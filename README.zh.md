# LOICollection
## Minecraft Bedrock Server LiteLoaderBDS Plugin

### [English](README.md) | [简体中文](README.zh.md)

## 实现插件
实现功能 | 完成
--- | :---:
Blacklist | $\color{#00FF00}{√}$
Mute | $\color{#00FF00}{√}$
Cdk | $\color{#00FF00}{√}$
Menu | $\color{#00FF00}{√}$
Tpa | $\color{#00FF00}{√}$
Shop | $\color{#00FF00}{√}$
Monitor | $\color{#00FF00}{√}$
Pvp | $\color{#00FF00}{√}$
Wallet | $\color{#00FF00}{√}$
Chat | $\color{#00FF00}{√}$
AnnounCement | $\color{#00FF00}{√}$
Market | $\color{#00FF00}{√}$

## 功能命令
#### language
命令 | 描述 | 权限
--- | :---: | :---:
language | 切换语言 | Any
#### Blacklist
命令 | 描述 | 权限
--- | :---: | :---:
blacklist gui | 打开黑名单UI | OP
blacklist list | 显示黑名单列表 | OP
blacklist add <type(xuid/ip)> <player> ([cause]/[time]) [time] | 添加玩家到黑名单 | OP
blacklist remove <string> | 从黑名单中移除玩家 | OP
#### Mute
命令 | 描述 | 权限
--- | :---: | :---:
mute gui | 打开Mute UI | OP
mute add <player> ([cause]/[time]) [time] | 禁言玩家 | OP
mute remove <player> | 解除禁言 | OP
#### Cdk
命令 | 描述 | 权限
--- | :---: | :---:
cdk gui | 打开CDK总换UI | Any
cdk gui true | 打开CDK设置UI | OP
cdk convert <string> | 总换CDK | Any
#### Menu
命令 | 描述 | 权限
--- | :---: | :---:
menu gui | 打开菜单UI | Any
menu gui <string> | 打开指定ID菜单UI | OP
menu clock | 获取菜单物品 | Any
#### Tpa
命令 | 描述 | 权限
--- | :---: | :---:
tpa gui | 打开TPA UI | Any
tpa setting | 打开个人TPA设置 | Any
#### Shop
命令 | 描述 | 权限
--- | :---: | :---:
shop gui | 打开商店售卖UI | Any
shop sell | 打开商店售出UI | Any
#### Pvp
命令 | 描述 | 权限
--- | :---: | :---:
pvp gui | 打开PVP UI | Any
#### Wallet
命令 | 描述 | 权限
--- | :---: | :---:
wallet gui | 打开钱包UI | Any
#### Chat
命令 | 描述 | 权限
--- | :---: | :---:
chat setting | 打开个人聊天设置UI | Any
#### AnnounCement
命令 | 描述 | 权限
--- | :---: | :---:
announcement gui | 打开公告UI | Any
announcement setting | 打开公告设置 | OP
#### Market
命令 | 描述 | 权限
--- | :---: | :---:
market gui | 打开玩家市场UI | Any
market sell | 打开玩家市场个人售出UI | Any

### 如何安装插件？
1. 将文件 `LOICollection.dll` 放置在服务器目录的 `plugins` 文件夹中。
- `LOICollection.dll`（您可以在发布页面下载）。
2. 启动服务器。
3. 等待输出加载文本。
4. 完成安装。

### 如何配置文件？
1. 打开插件目录 `LOICollection`。
2. 打开文件 `config.json`。
3. 打开后，您会看到类似以下内容的内容：
```json
{
    "version": "1.2.11",
    "FakeSeed": 114514,
    "Blacklist": false,
    "Mute": false, 
    "Cdk": false,
    "Menu": {
        "Enable": false,
        "ItemId": "minecraft:clock" 
    },
    "Tpa": false,
    "Shop": false,
    "Monitor": {
        "Enable": false,
        "join": "{player}加入了服务器",
        "left": "{player}退出了服务器",
        "llmoney": "§e§l检测到金钱发生变化 §b原值: §f${OriMoney} §a更改: §f${SetMoney} §e现值: §f${GetMoney}",
        "command": [],
        "tips": "该指令已被禁用" 
    },
    "Pvp": false,
    "Wallet": {
        "Enable": false,
        "llmoney": true,
        "score": "money",
        "tax": 0.1 
    },
    "Chat": {
        "Enable": false,
        "chat": "<{player}> ${chat}"
    },
    "AnnounCement": false,
    "Market": false
}
```
- 请按照 Json 规范 (https://www.json.org/) 进行更改。
4. 您可以自由地进行更改并保存。

### 如何在本地编译？
1. 打开本地命令提示符(cmd)并执行以下命令：
```cmd
git clone --recursive https://github.com/loitietu/LOICollection.git
```
2. 打开 IDE（建议使用 Visual Studio Code）。
3. 打开克隆的项目。
4. 等待自动配置完成，然后进行编译。

### LICENSE
- 本插件根据 [GPL-3.0](LICENSE) 许可证进行许可。