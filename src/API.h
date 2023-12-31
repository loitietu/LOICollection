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
    LOICOLLECTION_API void translateString(std::string* str, Player* player);
}
#ifdef __cplusplus
}
#endif