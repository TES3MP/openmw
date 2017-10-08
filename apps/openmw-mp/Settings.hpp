//
// Created by koncord on 12.08.17.
//

#pragma once

class LuaState;
class Player;


class GameSettings
{
public:
    static void Init(LuaState &lua);
public:

    explicit GameSettings(Player *player);
    ~GameSettings();
    /**
    * \brief Set whether the console is allowed for a player.
    *
    * This changes the console permission for that player in the server memory, but does not
    * by itself send a packet.
    *
    * \param The console permission state.
    */
    void SetConsoleAllow(bool state);

    /**
    * \brief Set the difficulty for a player.
    *
    * This changes the difficulty for that player in the server memory, but does not by itself
    * send a packet.
    *
    * \param The difficulty.
    */
    void SetDifficulty(int difficulty);

    void update();
private:
    Player *player;
    bool changed;
};