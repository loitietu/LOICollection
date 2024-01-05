#include <llapi/LoggerAPI.h>
#include "include/tpa.h"
extern Logger logger;

namespace tpa {
    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        logger.info("<Tpa>: 插件已加载");
    }
}