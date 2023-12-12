#ifndef LOICOLLECTION_LANGUAGE_H
#define LOICOLLECTION_LANGUAGE_H

#include <string>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/ServerPlayer.hpp>

namespace language {
    void load(int* OpenPlugin);
    std::string get(ServerPlayer* player);
    std::string get(Player* player);
}

#endif