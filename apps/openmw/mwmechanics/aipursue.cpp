#include "aipursue.hpp"

#include <components/esm3/aisequence.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/world.hpp"

#include "../mwworld/class.hpp"

<<<<<<< HEAD
/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include <components/openmw-mp/TimedLog.hpp>
#include "../mwgui/windowmanagerimp.hpp"
#include "../mwmp/Main.hpp"
#include "../mwmp/LocalPlayer.hpp"
/*
    End of tes3mp addition
*/

#include "movement.hpp"
=======
#include "actorutil.hpp"
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
#include "creaturestats.hpp"

namespace MWMechanics
{

<<<<<<< HEAD
AiPursue::AiPursue(const MWWorld::Ptr& actor)
{
    mTargetActorId = actor.getClass().getCreatureStats(actor).getActorId();
}

AiPursue::AiPursue(const ESM::AiSequence::AiPursue *pursue)
{
    mTargetActorId = pursue->mTargetActorId;
}

bool AiPursue::execute (const MWWorld::Ptr& actor, CharacterController& characterController, AiState& state, float duration)
{
    if(actor.getClass().getCreatureStats(actor).isDead())
        return true;

    const MWWorld::Ptr target = MWBase::Environment::get().getWorld()->searchPtrViaActorId(mTargetActorId); //The target to follow

    // Stop if the target doesn't exist
    // Really we should be checking whether the target is currently registered with the MechanicsManager
    if (target == MWWorld::Ptr() || !target.getRefData().getCount() || !target.getRefData().isEnabled())
        return true;

    if (isTargetMagicallyHidden(target) && !MWBase::Environment::get().getMechanicsManager()->awarenessCheck(target, actor))
        return false;

    if (target.getClass().getCreatureStats(target).isDead())
        return true;

    /*
        Start of tes3mp addition

        Because multiplayer does not pause the game, prevent infinite arrest loops by ignoring
        players already engaged in dialogue while retaining the AiPursue package

        Additionally, do not arrest players who are currently jailed
    */
    if (target == MWBase::Environment::get().getWorld()->getPlayerPtr())
    {
        if (MWBase::Environment::get().getWindowManager()->containsMode(MWGui::GM_Dialogue) ||
            MWBase::Environment::get().getWindowManager()->containsMode(MWGui::GM_Jail))
        {
            return false;
        }
    }
    /*
        End of tes3mp addition
    */

    actor.getClass().getCreatureStats(actor).setDrawState(DrawState_Nothing);

    //Set the target destination
    const osg::Vec3f dest = target.getRefData().getPosition().asVec3();
    const osg::Vec3f actorPos = actor.getRefData().getPosition().asVec3();

    const float pathTolerance = 100.f;

    // check the true distance in case the target is far away in Z-direction
    bool reached = pathTo(actor, dest, duration, pathTolerance) &&
                   std::abs(dest.z() - actorPos.z()) < pathTolerance;

    if (reached)
    {
        if (!MWBase::Environment::get().getWorld()->getLOS(target, actor))
            return false;

        /*
            Start of tes3mp addition

            Record that the player has not died since the last attempt to arrest them

            Close the player's inventory or open container and cancel any drag and drops
        */
        LOG_MESSAGE_SIMPLE(TimedLog::LOG_INFO, "After being pursued by %s, diedSinceArrestAttempt is now false", actor.getCellRef().getRefId().c_str());
        mwmp::Main::get().getLocalPlayer()->diedSinceArrestAttempt = false;
        mwmp::Main::get().getLocalPlayer()->closeInventoryWindows();
        /*
            End of tes3mp addition
        */

        MWBase::Environment::get().getWindowManager()->pushGuiMode(MWGui::GM_Dialogue, actor); //Arrest player when reached
        return true;
=======
    AiPursue::AiPursue(const MWWorld::Ptr& actor)
    {
        mTargetActorId = actor.getClass().getCreatureStats(actor).getActorId();
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
    }

    AiPursue::AiPursue(const ESM::AiSequence::AiPursue* pursue)
    {
        mTargetActorId = pursue->mTargetActorId;
    }

    bool AiPursue::execute(
        const MWWorld::Ptr& actor, CharacterController& characterController, AiState& state, float duration)
    {
        if (actor.getClass().getCreatureStats(actor).isDead())
            return true;

        const MWWorld::Ptr target
            = MWBase::Environment::get().getWorld()->searchPtrViaActorId(mTargetActorId); // The target to follow

        // Stop if the target doesn't exist
        // Really we should be checking whether the target is currently registered with the MechanicsManager
        if (target == MWWorld::Ptr() || !target.getRefData().getCount() || !target.getRefData().isEnabled())
            return true;

        if (isTargetMagicallyHidden(target)
            && !MWBase::Environment::get().getMechanicsManager()->awarenessCheck(target, actor))
            return false;

        if (target.getClass().getCreatureStats(target).isDead())
            return true;

        actor.getClass().getCreatureStats(actor).setDrawState(DrawState::Nothing);

        // Set the target destination
        const osg::Vec3f dest = target.getRefData().getPosition().asVec3();
        const osg::Vec3f actorPos = actor.getRefData().getPosition().asVec3();

        const float pathTolerance = 100.f;

        // check the true distance in case the target is far away in Z-direction
        bool reached = pathTo(actor, dest, duration, pathTolerance, (actorPos - dest).length(), PathType::Partial)
            && std::abs(dest.z() - actorPos.z()) < pathTolerance;

        if (reached)
        {
            if (!MWBase::Environment::get().getWorld()->getLOS(target, actor))
                return false;
            MWBase::Environment::get().getWindowManager()->pushGuiMode(
                MWGui::GM_Dialogue, actor); // Arrest player when reached
            return true;
        }

        actor.getClass().getCreatureStats(actor).setMovementFlag(
            MWMechanics::CreatureStats::Flag_Run, true); // Make NPC run

        return false;
    }

    MWWorld::Ptr AiPursue::getTarget() const
    {
        if (!mCachedTarget.isEmpty())
        {
            if (mCachedTarget.getRefData().isDeleted() || !mCachedTarget.getRefData().isEnabled())
                mCachedTarget = MWWorld::Ptr();
            else
                return mCachedTarget;
        }
        mCachedTarget = MWBase::Environment::get().getWorld()->searchPtrViaActorId(mTargetActorId);
        return mCachedTarget;
    }

    void AiPursue::writeState(ESM::AiSequence::AiSequence& sequence) const
    {
        auto pursue = std::make_unique<ESM::AiSequence::AiPursue>();
        pursue->mTargetActorId = mTargetActorId;

        ESM::AiSequence::AiPackageContainer package;
        package.mType = ESM::AiSequence::Ai_Pursue;
        package.mPackage = std::move(pursue);
        sequence.mPackages.push_back(std::move(package));
    }

} // namespace MWMechanics
