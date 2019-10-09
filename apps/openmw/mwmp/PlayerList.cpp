#include <components/openmw-mp/MWMPLog.hpp>
#include <apps/openmw/mwclass/creature.hpp>

#include "../mwbase/environment.hpp"

#include "../mwclass/npc.hpp"

#include "../mwmechanics/creaturestats.hpp"

#include "../mwworld/cellstore.hpp"
#include "../mwworld/player.hpp"
#include "../mwworld/worldimp.hpp"

#include "PlayerList.hpp"
#include "Main.hpp"
#include "DedicatedPlayer.hpp"
#include "CellController.hpp"
#include "GUIController.hpp"


using namespace mwmp;
using namespace std;

std::map <RakNet::RakNetGUID, DedicatedPlayer *> PlayerList::players;

void PlayerList::update(float dt)
{
    for (auto &p : players)
    {
        DedicatedPlayer *player = p.second;
        if (player == 0) continue;

        player->update(dt);
    }
}

DedicatedPlayer *PlayerList::newPlayer(RakNet::RakNetGUID guid)
{
    LOG_APPEND(MWMPLog::LOG_INFO, "- Creating new DedicatedPlayer with guid %s", guid.ToString());

    players[guid] = new DedicatedPlayer(guid);

    LOG_APPEND(MWMPLog::LOG_INFO, "- There are now %i DedicatedPlayers", players.size());

    return players[guid];
}

void PlayerList::deletePlayer(RakNet::RakNetGUID guid)
{
    if (players[guid]->reference)
        players[guid]->deleteReference();

    delete players[guid];
    players.erase(guid);
}

void PlayerList::cleanUp()
{
    for (auto &p : players)
        delete p.second;
}

DedicatedPlayer *PlayerList::getPlayer(RakNet::RakNetGUID guid)
{
    return players[guid];
}

DedicatedPlayer *PlayerList::getPlayer(const MWWorld::Ptr &ptr)
{
    for (auto &p : players)
    {
        if (p.second == 0 || p.second->getPtr().mRef == 0)
            continue;
        
        string refId = ptr.getCellRef().getRefId();
        
        if (p.second->getPtr().getCellRef().getRefId() == refId)
            return p.second;
    }
    return 0;
}

bool PlayerList::isDedicatedPlayer(const MWWorld::Ptr &ptr)
{
    if (ptr.mRef == nullptr)
        return false;

    // Players always have 0 as their refNum and mpNum
    if (ptr.getCellRef().getRefNum().mIndex != 0 || ptr.getCellRef().getMpNum() != 0)
        return false;

    return (getPlayer(ptr) != 0);
}

/*
    Go through all DedicatedPlayers checking if their mHitAttemptActorId matches this one
    and set it to -1 if it does

    This resets the combat target for a DedicatedPlayer's followers in Actors::update()
*/
void PlayerList::clearHitAttemptActorId(int actorId)
{
    for (auto &p : players)
    {
        if (p.second == 0 || p.second->getPtr().mRef == 0)
            continue;

        MWMechanics::CreatureStats &playerCreatureStats = p.second->getPtr().getClass().getCreatureStats(p.second->getPtr());

        if (playerCreatureStats.getHitAttemptActorId() == actorId)
            playerCreatureStats.setHitAttemptActorId(-1);
    }
}
