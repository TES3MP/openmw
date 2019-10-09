#include "Settings.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/MWMPLog.hpp>

#include <apps/openmw-mp/Script/ScriptFunctions.hpp>
#include <apps/openmw-mp/Networking.hpp>

#include <iostream>
using namespace std;

void SettingFunctions::SetDifficulty(unsigned short pid, int difficulty)
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->difficulty = difficulty;
}

void SettingFunctions::SetEnforcedLogLevel(unsigned short pid, int enforcedLogLevel)
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->enforcedLogLevel = enforcedLogLevel;
}

void SettingFunctions::SetPhysicsFramerate(unsigned short pid, double physicsFramerate)
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->physicsFramerate = physicsFramerate;
}

void SettingFunctions::SetConsoleAllowed(unsigned short pid, bool state)
{
    Player *player;
    GET_PLAYER(pid, player,);

    player->consoleAllowed = state;
}

void SettingFunctions::SetBedRestAllowed(unsigned short pid, bool state)
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->bedRestAllowed = state;
}

void SettingFunctions::SetWildernessRestAllowed(unsigned short pid, bool state)
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->wildernessRestAllowed = state;
}

void SettingFunctions::SetWaitAllowed(unsigned short pid, bool state)
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->waitAllowed = state;
}

void SettingFunctions::SendSettings(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player,);

    mwmp::PlayerPacket *packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_GAME_SETTINGS);
    packet->setPlayer(player);

    packet->Send(false);
}
