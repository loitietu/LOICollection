# LOICollection
## Minecraft Bedrock Server LiteLoaderBDS Plugin

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
    "version": "1.2.0", //Plugin Version
    "Blacklist": false, //Whether the plug-in is enabled (true is enabled, false is disabled)
    "Mute": false, 
    "Cdk": false,
    "Menu": {
        "Enable": false,
        "ItemId": "minecraft:clock" //menu to open the item ID
    },
    "Tpa": false,
    "Shop": false,
    "Monitor": false,
    "Pvp": false,
    "Wallet": {
        "Enable": false,
        "llmoney": true, //Whether to enable the llmoney economic core
        "score": "money", //Use the scoreboard name
        "tax": 0.1 //Exchange rates such as transfers
    },
    "Chat": {
        "Enable": false,
        "chat": "<{player}> {chat}" //Format of presentations
    },
    "Command": {
        "Enable": false,
        "command": [], //Disable the command
        "tips": "该指令已被禁用" //Prompted when using the Disable command
    },
    "ServerTips": {
        "Enable": false,
        "join": "{player}加入了服务器", //Player join prompt
        "left": "{player}退出了服务器" //Player exit prompt
    },
    "AnnounCement": false
}
```
- Please follow the Json specification (https://www.json.org/) for changes.
4. You can change it freely and save it.