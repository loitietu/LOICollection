#include <llapi/LoggerAPI.h>
#include "include/cdk.h"
extern Logger logger;

namespace cdk {
    void load(int* OpenPlugin) {
        (*OpenPlugin)++;
        logger.info("<cdk>: 插件已加载");
    }
}