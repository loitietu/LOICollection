#define LOICOLLECTION_API __declspec(dllexport)

#include <thread>
#include <chrono>
#include <string>
#include <regex>
#include <algorithm>
#include <llapi/ScheduleAPI.h>
#include <llapi/mc/Scoreboard.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/mc/Player.hpp>
#include "tool.h"
#include "API.h"

unsigned short ticks = 0;
unsigned short tickPerSecond;
unsigned short tickPerMinute;
std::list<unsigned short> tickPerMinutes;

void scheduleTask() {
    Schedule::repeat([]() {
        ticks++;
    }, 1);
    std::thread([] {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (ticks > 20) {
                ticks = 20;
            }
            tickPerSecond = ticks;
            tickPerMinutes.push_back(tickPerSecond);
            ticks = 0;
            if (tickPerMinutes.size() >= 60) {
                tickPerMinutes.clear();
                continue;
            }
            unsigned short sum = 0;
            for (unsigned short tick : tickPerMinutes) {
                sum = sum + tick;
            }
            tickPerMinute = sum / tickPerMinutes.size();
        }
    }).detach();
}

namespace LOICollectionAPI {
    void init() {
        scheduleTask();
    }

    unsigned short getTransactionPerSecond() {
        return tickPerSecond;
    }

    unsigned short getTransactionPerMinute() {
        return tickPerMinute;
    }

    void translateString(std::string* str, Player* player, bool enable = false) {
        (*str) = tool::replaceString(*str, "{tps}", std::to_string(tickPerSecond));
        (*str) = tool::replaceString(*str, "{tpm}", std::to_string(tickPerMinute));
        (*str) = tool::replaceString(*str, "{player}", player->getName());
        (*str) = tool::replaceString(*str, "{pos}", tool::vec3ToString(player->getPos()));
        (*str) = tool::replaceString(*str, "{blockPos}", tool::vec3ToString(player->getBlockPos().toVec3()));
        (*str) = tool::replaceString(*str, "{lastDeathPos}", tool::vec3ToString(player->getLastDeathPos().value().toVec3()));
        (*str) = tool::replaceString(*str, "{realName}", player->getRealName());
        (*str) = tool::replaceString(*str, "{xuid}", player->getXuid());
        (*str) = tool::replaceString(*str, "{uuid}", player->getUuid());
        (*str) = tool::replaceString(*str, "{canFly}", std::to_string(player->canFly()));
        (*str) = tool::replaceString(*str, "{maxHealth}", std::to_string(player->getMaxHealth()));
        (*str) = tool::replaceString(*str, "{health}", std::to_string(player->getHealth()));
        (*str) = tool::replaceString(*str, "{speed}", std::to_string(player->getSpeed()));
        (*str) = tool::replaceString(*str, "{direction}", std::to_string(player->getDirection()));
        (*str) = tool::replaceString(*str, "{ip}", player->getIP());
        (*str) = tool::replaceString(*str, "{xp}", std::to_string(player->getXpEarnedAtCurrentLevel()));
        (*str) = tool::replaceString(*str, "{HandItem}", player->getHandSlot()->getName());
        (*str) = tool::replaceString(*str, "{OffHand}", player->getOffhandSlot().getName());
        (*str) = tool::replaceString(*str, "{os}", player->getDeviceName());
        (*str) = tool::replaceString(*str, "{os.type}", std::to_string(player->getDeviceType()));
        (*str) = tool::replaceString(*str, "{os.typeName}", player->getDeviceTypeName());
        (*str) = tool::replaceString(*str, "{llmoney}", std::to_string(tool::llmoney::get(player)));
        if (enable) {
            (*str) = tool::replaceString(*str, "{ms}", std::to_string(player->getLastPing()));
            (*str) = tool::replaceString(*str, "{avgms}", std::to_string(player->getAvgPing()));
            (*str) = tool::replaceString(*str, "{Packet}", std::to_string(player->getLastPacketLoss()));
            (*str) = tool::replaceString(*str, "{avgPacket}", std::to_string(player->getAvgPacketLoss()));
        }
        std::regex pattern("\\{score\\((.*?)\\)\\}");
        std::smatch match;
        while (std::regex_search(*str, match, pattern)) {
            std::string extractedContent = match.str(1);
            (*str) = std::regex_replace(*str, pattern, std::to_string(Scoreboard::getScore(player, extractedContent)));
        }
    }
}