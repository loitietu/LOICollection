#pragma once

#ifdef LOICOLLECTION_API
#else
#define LOICOLLECTION_API __declspec(dllimport)
#endif

#include <string>

#include <llapi/mc/Player.hpp>

#ifdef __cplusplus
extern "C" {
#endif
namespace LOICollectionAPI {
    LOICOLLECTION_API void init();
    LOICOLLECTION_API unsigned short getTransactionPerSecond();
    LOICOLLECTION_API unsigned short getTransactionPerMinute();
    LOICOLLECTION_API unsigned int GetMemory();
    LOICOLLECTION_API double GetMemoryMB();
    LOICOLLECTION_API const char* translateString(std::string str, Player* player, bool enable);
}
#ifdef __cplusplus
}
#endif