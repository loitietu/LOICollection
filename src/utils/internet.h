#ifndef LOICOLLECTION_INTERNET_H
#define LOICOLLECTION_INTERNET_H

#include <string>

namespace internet {
    namespace upgrade {
        std::string getLastData(int ResId);
        std::string getLastVersion(int ResId);
        std::string getLastUrl(int ResId);
        bool isNewVersionAvailable(const std::string version, int ResId);
    }
    namespace LOICollection {
        void checkForUpdates(const std::string versionInfo);
    }
}

#endif