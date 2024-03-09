#ifndef LOICOLLECTION_INTERNET_H
#define LOICOLLECTION_INTERNET_H

#include <string>

namespace internet {
    namespace upgrade {
        std::string getLastData(int ResId);
        std::string getLastVersion(int ResId);
        bool isNewVersionAvailable(const std::string version, int ResId);
        void downloadAndInstallUpgrade(int ResId);
    }
    void downloadFile(const std::string url, const std::string path);
}

#endif