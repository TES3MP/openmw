#include "actionteleport.hpp"

<<<<<<< HEAD
/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include <components/openmw-mp/TimedLog.hpp>
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
=======
#include <components/esm3/loadcell.hpp>
#include <components/esm3/loadmgef.hpp>
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

#include "../mwbase/environment.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/world.hpp"

#include "../mwmechanics/actorutil.hpp"
#include "../mwmechanics/creaturestats.hpp"

#include "../mwworld/cellstore.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/worldmodel.hpp"

#include "player.hpp"

namespace MWWorld
{
    ActionTeleport::ActionTeleport(ESM::RefId cellId, const ESM::Position& position, bool teleportFollowers)
        : Action(true)
        , mCellId(cellId)
        , mPosition(position)
        , mTeleportFollowers(teleportFollowers)
    {
    }

    void ActionTeleport::executeImp(const Ptr& actor)
    {
        if (mTeleportFollowers)
        {
            // Find any NPCs that are following the actor and teleport them with him
            std::set<MWWorld::Ptr> followers;

            bool toExterior = MWBase::Environment::get().getWorldModel()->getCell(mCellId).isExterior();
            getFollowers(actor, followers, toExterior, true);

            for (std::set<MWWorld::Ptr>::iterator it = followers.begin(); it != followers.end(); ++it)
                teleport(*it);
        }

        teleport(actor);
    }

    void ActionTeleport::teleport(const Ptr& actor)
    {
        MWBase::World* world = MWBase::Environment::get().getWorld();
        MWWorld::WorldModel* worldModel = MWBase::Environment::get().getWorldModel();
        auto& stats = actor.getClass().getCreatureStats(actor);
        stats.land(actor == world->getPlayerPtr());
        stats.setTeleported(true);

        Ptr teleported;
        if (actor == world->getPlayerPtr())
        {
            world->getPlayer().setTeleported(true);
            world->changeToCell(mCellId, mPosition, true);
            teleported = world->getPlayerPtr();
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

            if (actor.getClass().getCreatureStats(actor).getAiSequence().isInCombat(world->getPlayerPtr()))
            {
<<<<<<< HEAD
                int cellX;
                int cellY;
                world->positionToIndex(mPosition.pos[0],mPosition.pos[1],cellX,cellY);

                newCellStore = world->getExterior(cellX, cellY);
                if (cellController->isDedicatedActor(actor))
                    cellController->getDedicatedActor(actor)->cell = *newCellStore->getCell();

                world->moveObject(actor,world->getExterior(cellX,cellY),
                    mPosition.pos[0],mPosition.pos[1],mPosition.pos[2]);
=======
                actor.getClass().getCreatureStats(actor).getAiSequence().stopCombat();
                return;
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
            }

            else
<<<<<<< HEAD
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

            LOG_MESSAGE_SIMPLE(TimedLog::LOG_INFO, "Sending ID_ACTOR_CELL_CHANGE about %s %i-%i to server",
                actor.getCellRef().getRefId().c_str(), baseActor.refNum, baseActor.mpNum);

            LOG_APPEND(TimedLog::LOG_INFO, "- Moved from %s to %s", actorList->cell.getDescription().c_str(),
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
=======
                teleported = world->moveObject(actor, &worldModel->getCell(mCellId), mPosition.asVec3(), true, true);
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
        }

        if (!world->isWaterWalkingCastableOnTarget(teleported) && MWMechanics::hasWaterWalking(teleported))
            teleported.getClass()
                .getCreatureStats(teleported)
                .getActiveSpells()
                .purgeEffect(actor, ESM::MagicEffect::WaterWalking);
    }

    void ActionTeleport::getFollowers(
        const MWWorld::Ptr& actor, std::set<MWWorld::Ptr>& out, bool toExterior, bool includeHostiles)
    {
        std::set<MWWorld::Ptr> followers;
        MWBase::Environment::get().getMechanicsManager()->getActorsFollowing(actor, followers);

        for (std::set<MWWorld::Ptr>::iterator it = followers.begin(); it != followers.end(); ++it)
        {
            MWWorld::Ptr follower = *it;

            const ESM::RefId& script = follower.getClass().getScript(follower);

            if (!includeHostiles && follower.getClass().getCreatureStats(follower).getAiSequence().isInCombat(actor))
                continue;

            if (!toExterior && !script.empty()
                && follower.getRefData().getLocals().getIntVar(script, "stayoutside") == 1
                && follower.getCell()->getCell()->isExterior())
                continue;

            if ((follower.getRefData().getPosition().asVec3() - actor.getRefData().getPosition().asVec3()).length2()
                > 800 * 800)
                continue;

            out.emplace(follower);
        }
    }
}
