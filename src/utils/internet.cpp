#define CPPHTTPCLIENT_OPENSSL_SUPPORT

#include <string>
#include <fstream>
#include <llapi/LLAPI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/utils/NetworkHelper.h>
#include <httplib/httplib.h>
#include <Nlohmann/json.hpp>
#include "internet.h"
extern Logger logger;

namespace internet {
    namespace upgrade {
        std::string getLastData(int ResId) {
            try {
                std::string url = ("https://api.minebbs.com/api/openapi/v1/resources/" + std::to_string(ResId));
                std::string info;
                int status = -1;
                if (!HttpGetSync(url, &status, &info, 60) || status != 200) {
                    return nullptr;
                }
                nlohmann::ordered_json data = nlohmann::ordered_json::parse(info, nullptr, true, true);
                if (data.contains("data")) {
                    nlohmann::ordered_json data2 = data["data"];
                    return data2.dump();
                } else {
                    return nullptr;
                }
            } catch (...) {
                return nullptr;
            }
        }

        std::string getLastVersion(int ResId) {
            nlohmann::ordered_json data = nlohmann::ordered_json::parse(getLastData(ResId));
            if (data.contains("version")) {
                return data["version"].get<std::string>();
            } else {
                return nullptr;
            }
        }

        std::string getLastUrl(int ResId) {
            nlohmann::ordered_json data = nlohmann::ordered_json::parse(getLastData(ResId));
            if (data.contains("view_url")) {
                return data["view_url"].get<std::string>();
            } else {
                return nullptr;
            }
        }

        bool isNewVersionAvailable(const std::string version, int ResId) {
            ll::Version varRemote = ll::Version::parse(getLastVersion(ResId));
            ll::Version varLocal = ll::Version::parse(version);
            if (varRemote > varLocal) {
                return true;
            } else {
                return false;
            }
        }
    }

    namespace LOICollection {
        void checkForUpdates(const std::string versionInfo) {
            if (internet::upgrade::isNewVersionAvailable(versionInfo, 5570)) {
                std::string url = internet::upgrade::getLastUrl(5570);
                std::string version = internet::upgrade::getLastVersion(5570);
                logger.info("发现新版本: " + version);
                logger.info("下载地址: " + url);
                logger.info("请复制地址手动下载");
            } else {
                logger.info("当前已是最新版本");
            }
        }
    }
}