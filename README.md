# LOICollection
## Minecraft Bedrock Server LiteLoaderBDS Plugin

### [English](README.md) | [简体中文](README.zh.md)

## Implementation plug-in
Realize the function | accomplish
--- | :---:
Blacklist | $\color{#00FF00}{√}$
Mute | $\color{#00FF00}{√}$
Cdk | $\color{#00FF00}{√}$
Menu | $\color{#00FF00}{√}$
Tpa | $\color{#00FF00}{√}$
Shop | $\color{#00FF00}{√}$
Monitor | $\color{#00FF00}{√}$
Pvp | $\color{#FF0000}{×}$
Wallet | $\color{#FF0000}{×}$
Chat | $\color{#00FF00}{√}$
Command | $\color{#FF0000}{×}$
AnnounCement | $\color{#FF0000}{×}$

### How to install the plugin?
1. Place the file `LOICollection.dll` in the server directory `plugins`.
- `LOICollection.dll` (You can download it in the Release page.).
2. Start the server
3. Wait for the output to load the text
4. Complete the installation

### How to configure the file?
1. Open the plugin directory `LOICollection`
2. Open file `config.json`
3. When you open it, you'll see something like this:
```json
{
    "version": "1.2.6",
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
        "Enable": true,
        "join": "{player}加入了服务器",
        "left": "{player}退出了服务器",
        "llmoney": "§e§l检测到金钱发生变化 §b原值: §f${OriMoney} §a更改: §f${SetMoney} §e现值: §f${GetMoney}"
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
    "Command": {
        "Enable": false,
        "command": [],
        "tips": "该指令已被禁用" 
    },
    "AnnounCement": false
}
```
- Please follow the Json specification (https://www.json.org/) for changes.
4. You can change it freely and save it.

### How to compile locally?
1. Open the local cmd and execute the command:
```cmd
git clone --recursive https://github.com/loitietu/LOICollection.git
```
2. Open ide (It is recommended to use Visual Studio Code).
3. Open the cloned project.
4. Wait for the automatic configuration to complete and then compile.


### LICENSE
- This plugin is licensed under the [GPL-3.0](LICENSE) license.