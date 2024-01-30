/**
 * @file DLLMain.cpp
 * @note DO NOT modify or remove this file!
 */

#include <iostream>
#include <filesystem>

#include <llapi/LoggerAPI.h>
#include <llapi/ServerAPI.h>

#include "version.h"

void PluginInit();

Logger logger(PLUGIN_NAME);

void CheckProtocolVersion()
{

#ifdef TARGET_BDS_PROTOCOL_VERSION

    auto current_protocol = ll::getServerProtocolVersion();
    if (TARGET_BDS_PROTOCOL_VERSION != current_protocol)
    {
        logger.warn("Protocol version mismatched! Target version: {}. Current version: {}.",
                    TARGET_BDS_PROTOCOL_VERSION, current_protocol);
        logger.warn("This may result in crash. Please switch to the version matching the BDS version!");
    }

#endif // TARGET_BDS_PROTOCOL_VERSION
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ll::registerPlugin(
            PLUGIN_NAME,
            PLUGIN_INTRODUCTION,
            ll::Version(PLUGIN_VERSION_MAJOR, PLUGIN_VERSION_MINOR, PLUGIN_VERSION_REVISION, PLUGIN_LLVERSION_STATUS),
            std::map<std::string, std::string>{
                {"Author", PLUGIN_AUTHOR},
            });
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C"
{
    _declspec(dllexport) void onPostInit()
    {
        if (std::filesystem::exists("./plugins/LOICollection/AGREE")) {
            if (std::filesystem::exists("./plugins/LLMoney.dll")) {
                std::ios::sync_with_stdio(false);
                CheckProtocolVersion();
                PluginInit();
            } else {
                logger.warn("LLMoney.dll 没有找到");
                logger.warn("插件初始化任务已取消");
                logger.warn("请安装插件 LLMoney.dll 到目录 plugins");
                logger.warn("如果插件没有安装，则无法正常使用部分功能");
            }
        } else {
            std::cout << std::endl;
            std::cout << "免责声明" << std::endl; 
            std::cout << " - 1. 插件的使用是基于用户自愿的选择，用户应自行承担使用插件所带来的风险和后果。" << std::endl;
            std::cout << " - 2. 插件的开发者和维护者尽力确保插件的稳定性和安全性，但不对插件的功能、性能和可靠性做出任何明示或暗示的保证。" << std::endl; 
            std::cout << " - 3. 插件的开发者和维护者不对因使用插件而导致的任何直接或间接损失或损害承担责任，包括但不限于数据丢失、系统崩溃、游戏中断等。" << std::endl; 
            std::cout << " - 4. 插件的开发者和维护者不对插件的兼容性问题负责，包括但不限于与其他插件或游戏版本的兼容性。" << std::endl; 
            std::cout << " - 5. 插件的开发者和维护者保留随时修改、更新或终止插件部分功能的权利，而无需提前通知用户。" << std::endl; 
            std::cout << " - 6. 用户在使用插件时应遵守游戏的规则和条款，不得利用插件进行作弊、破坏游戏平衡或侵犯其他玩家的权益。" << std::endl; 
            std::cout << " - 7. 用户在使用插件时应注意个人信息的保护，不要泄露个人敏感信息给插件的开发者或其他不可信的第三方。" << std::endl; 
            std::cout << " - 8. 插件的开发者和维护者不对插件可能存在的安全漏洞或恶意代码负责，用户应自行采取安全措施，确保插件的安全性。" << std::endl;
            std::cout << " - 9. 用户不得将插件用于任何非法目的，包括但不限于违反法律法规、侵犯他人权益等。" << std::endl;
            std::cout << " - 10. 如用户不满以上条款，可拒绝使用插件。如已使用插件并进行游戏后视为默允此条款。" << std::endl;
            std::cout << "在'LOICollection'文件夹下创建'AGREE'文件即同意以上条款" << std::endl;
            std::cout << std::endl;
            system("pause");
            exit(0);
        }
    }
}
