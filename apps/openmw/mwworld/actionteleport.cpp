#include "actionteleport.hpp"

/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include <components/openmw-mp/MWMPLog.hpp>
#include "../mwbase/windowmanager.hpp"
#include "../mwmp/Main.hpp"
#include "../mwmp/Networking.hpp"
#include "../mwmp/ActorList.hpp"
#include "../mwmp/CellController.hpp"
#include "../mwmp/MechanicsHelper.hpp"
/*
    End of tes3mp addition
*/

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/mechanicsmanager.hpp"

#include "../mwmechanics/creaturestats.hpp"

#include "../mwworld/class.hpp"

#include "player.hpp"

namespace MWWorld
{
    ActionTeleport::ActionTeleport (const std::string& cellName,
        const ESM::Position& position, bool teleportFollowers)
    : Action (true), mCellName (cellName), mPosition (position), mTeleportFollowers(teleportFollowers)
    {
    }

    void ActionTeleport::executeImp (const Ptr& actor)
    {
        if (mTeleportFollowers)
        {
            // Find any NPCs that are following the actor and teleport them with him
            std::set<MWWorld::Ptr> followers;
            getFollowersToTeleport(actor, followers);

            for (std::set<MWWorld::Ptr>::iterator it = followers.begin(); it != followers.end(); ++it)
                teleport(*it);
        }

        teleport(actor);
    }

    void ActionTeleport::teleport(const Ptr &actor)
    {
        MWBase::World* world = MWBase::Environment::get().getWorld();
        actor.getClass().getCreatureStats(actor).land(actor == world->getPlayerPtr());
        if(actor == world->getPlayerPtr())
        {
            world->getPlayer().setTeleported(true);
            if (mCellName.empty())
                world->changeToExteriorCell (mPosition, true);
            else
                world->changeToInteriorCell (mCellName, mPosition, true);
        }
        else
        {
            /*
                Start of tes3mp addition

                Track the original cell of this actor so we can use it when sending a packet
            */
            ESM::Cell originalCell = *actor.getCell()->getCell();
            /*
                End of tes3mp addition
            */

            /*
                Start of tes3mp change (minor)

                If this is a DedicatedActor, get their new cell and override their stored cell with it
                so their cell change is approved in World::moveObject()
            */
            MWWorld::CellStore *newCellStore;
            mwmp::CellController *cellController = mwmp::Main::get().getCellController();

            if (mCellName.empty())
            {
                int cellX;
                int cellY;
                world->positionToIndex(mPosition.pos[0],mPosition.pos[1],cellX,cellY);

                newCellStore = world->getExterior(cellX, cellY);
                if (cellController->isDedicatedActor(actor))
                    cellController->getDedicatedActor(actor)->cell = *newCellStore->getCell();

                world->moveObject(actor,world->getExterior(cellX,cellY),
                    mPosition.pos[0],mPosition.pos[1],mPosition.pos[2]);
            }
            else
            {
                newCellStore = world->getInterior(mCellName);
                if (cellController->isDedicatedActor(actor))
                    cellController->getDedicatedActor(actor)->cell = *newCellStore->getCell();

                world->moveObject(actor,world->getInterior(mCellName),mPosition.pos[0],mPosition.pos[1],mPosition.pos[2]);
            }
            /*
                Start of tes3mp change (minor)
            */

            /*
                Start of tes3mp addition

                Send ActorCellChange packets when an actor follows us across cells, regardless of
                whether we're the cell authority or not; the server can decide if it wants to comply
                with them

                Afterwards, send an ActorAI packet about this actor being our follower, to ensure
                they remain our follower even if the destination cell has another player as its
                cell authority
            */
            mwmp::BaseActor baseActor;
            baseActor.refNum = actor.getCellRef().getRefNum().mIndex;
            baseActor.mpNum = actor.getCellRef().getMpNum();
            baseActor.cell = *newCellStore->getCell();
            baseActor.position = actor.getRefData().getPosition();
            baseActor.isFollowerCellChange = true;

            mwmp::ActorList *actorList = mwmp::Main::get().getNetworking()->getActorList();
            actorList->reset();
            actorList->cell = originalCell;

            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Sending ID_ACTOR_CELL_CHANGE about %s %i-%i to server",
                actor.getCellRef().getRefId().c_str(), baseActor.refNum, baseActor.mpNum);

            LOG_APPEND(MWMPLog::LOG_INFO, "- Moved from %s to %s", actorList->cell.getDescription().c_str(),
                baseActor.cell.getDescription().c_str());

            actorList->addCellChangeActor(baseActor);
            actorList->sendCellChangeActors();

            // Send ActorAI to bring all players in the new cell up to speed with this follower
            actorList->cell = baseActor.cell;
            baseActor.aiAction = mwmp::BaseActorList::FOLLOW;
            baseActor.aiTarget = MechanicsHelper::getTarget(world->getPlayerPtr());
            actorList->addAiActor(baseActor);
            actorList->sendAiActors();
            /*
                End of tes3mp addition
            */
        }
    }

    void ActionTeleport::getFollowersToTeleport(const MWWorld::Ptr& actor, std::set<MWWorld::Ptr>& out) {
        std::set<MWWorld::Ptr> followers;
        MWBase::Environment::get().getMechanicsManager()->getActorsFollowing(actor, followers);

        for(std::set<MWWorld::Ptr>::iterator it = followers.begin();it != followers.end();++it)
        {
            MWWorld::Ptr follower = *it;

            std::string script = follower.getClass().getScript(follower);
            if (!script.empty() && follower.getRefData().getLocals().getIntVar(script, "stayoutside") == 1)
                continue;

            if ((follower.getRefData().getPosition().asVec3() - actor.getRefData().getPosition().asVec3()).length2() <= 800*800)
                out.insert(follower);
        }
    }
}
