//
// Created by koncord on 12.08.17.
//

#include "Settings.hpp"
#include <components/openmw-mp/NetworkMessages.hpp>
#include "Player.hpp"
#include "Script/LuaState.hpp"
#include "Networking.hpp"


void GameSettings::Init(LuaState &lua)
{
    lua.getState()->new_usertype<GameSettings>("Settings",
                                           "setConsoleAllow", &GameSettings::SetConsoleAllow,
                                           "setDifficulty", &GameSettings::SetDifficulty
    );
}

GameSettings::GameSettings(Player *player) : player(player), changed(false)
{

}

GameSettings::~GameSettings()
{
}

void GameSettings::SetConsoleAllow(bool state)
{
    player->consoleAllowed = state;
    changed = true;
}

void GameSettings::SetDifficulty(int difficulty)
{
    player->difficulty = difficulty;
    changed = true;
}

void GameSettings::update()
{
    if (!changed)
        return;
    changed = false;

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_GAME_SETTINGS);
    packet->setPlayer(player);
    packet->Send(false);
}
