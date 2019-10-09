//
// Created by koncord on 05.01.16.
//

#ifndef OPENMW_PLAYER_HPP
#define OPENMW_PLAYER_HPP

#include <map>
#include <string>
#include <chrono>
#include <RakNetTypes.h>

#include <components/esm/npcstats.hpp>
#include <components/esm/cellid.hpp>
#include <components/esm/loadnpc.hpp>
#include <components/esm/loadcell.hpp>

#include <components/openmw-mp/MWMPLog.hpp>
#include <components/openmw-mp/Base/BasePlayer.hpp>
#include <components/openmw-mp/Packets/Player/PlayerPacket.hpp>
#include "Cell.hpp"
#include "CellController.hpp"

typedef std::map<RakNet::RakNetGUID, Player*> TPlayers;
typedef std::map<unsigned short, Player*> TSlots;

class Players
{
public:
    static void newPlayer(RakNet::RakNetGUID guid);
    static void deletePlayer(RakNet::RakNetGUID guid);
    static Player *getPlayer(RakNet::RakNetGUID guid);
    static Player *getPlayer(unsigned short id);
    static TPlayers *getPlayers();
    static unsigned short getLastPlayerId();
    static bool doesPlayerExist(RakNet::RakNetGUID guid);

private:
    static TPlayers players;
    static TSlots slots;
};

class Player : public mwmp::BasePlayer
{
    friend class Cell;
    unsigned short id;
public:

    enum
    {
        NOTLOADED=0,
        LOADED,
        POSTLOADED,
        KICKED
    };
    Player(RakNet::RakNetGUID guid);

    unsigned short getId();
    void setId(unsigned short id);

    bool isHandshaked();
    int getHandshakeAttempts();
    void incrementHandshakeAttempts();
    void setHandshake();

    void setLoadState(int state);
    int getLoadState();

    virtual ~Player();

    CellController::TContainer *getCells();
    void sendToLoaded(mwmp::PlayerPacket *myPacket);

    void forEachLoaded(std::function<void(Player *pl, Player *other)> func);

private:
    CellController::TContainer cells;
    int loadState;
    int handshakeCounter;

};

#endif //OPENMW_PLAYER_HPP
