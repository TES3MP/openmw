#include "Shapeshift.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/MWMPLog.hpp>

#include <apps/openmw-mp/Script/ScriptFunctions.hpp>
#include <apps/openmw-mp/Networking.hpp>

#include <iostream>
using namespace std;

double ShapeshiftFunctions::GetScale(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, 0.0f);

    return player->scale;
}

bool ShapeshiftFunctions::IsWerewolf(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, 0);

    return player->isWerewolf;
}

const char *ShapeshiftFunctions::GetCreatureRefId(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, 0);

    return player->creatureRefId.c_str();
}

bool ShapeshiftFunctions::GetCreatureNameDisplayState(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, 0);

    return player->displayCreatureName;
}

void ShapeshiftFunctions::SetScale(unsigned short pid, double scale) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->scale = scale;
}

void ShapeshiftFunctions::SetWerewolfState(unsigned short pid, bool isWerewolf) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->isWerewolf = isWerewolf;
}

void ShapeshiftFunctions::SetCreatureRefId(unsigned short pid, const char *refId) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->creatureRefId = refId;
}

void ShapeshiftFunctions::SetCreatureNameDisplayState(unsigned short pid, bool displayState) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->displayCreatureName = displayState;
}

void ShapeshiftFunctions::SendShapeshift(unsigned short pid)
{
    Player *player;
    GET_PLAYER(pid, player, );

    mwmp::PlayerPacket *packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_SHAPESHIFT);
    packet->setPlayer(player);

    packet->Send(false);
    packet->Send(true);
}
