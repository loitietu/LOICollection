#include <llapi/LoggerAPI.h>
#include "include/mute.h"
extern Logger logger;

namespace mute {
    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        logger.info("<Mute>: 插件已加载");
    }
}