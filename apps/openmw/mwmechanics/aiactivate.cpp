<<<<<<< HEAD
#include "aiactivate.hpp"

#include <components/esm/aisequence.hpp>

/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include "../mwmp/Main.hpp"
#include "../mwmp/Networking.hpp"
#include "../mwmp/ObjectList.hpp"
/*
    End of tes3mp addition
*/

#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"

#include "../mwworld/class.hpp"

#include "creaturestats.hpp"
#include "movement.hpp"
#include "steering.hpp"

namespace MWMechanics
{
    AiActivate::AiActivate(const std::string &objectId)
        : mObjectId(objectId)
    {
    }

    /*
        Start of tes3mp addition

        Allow AiActivate to be initialized using a Ptr instead of a refId
    */
    AiActivate::AiActivate(MWWorld::Ptr object)
        : mObjectId("")
    {
        mObjectPtr = object;
    }
    /*
        End of tes3mp addition
    */

    bool AiActivate::execute(const MWWorld::Ptr& actor, CharacterController& characterController, AiState& state, float duration)
    {
        /*
            Start of tes3mp change (major)

            Only search for an object based on its refId if we haven't provided a specific object already
        */
        const MWWorld::Ptr target = mObjectId.empty() ? mObjectPtr : MWBase::Environment::get().getWorld()->searchPtr(mObjectId, false);
        /*
            End of tes3mp change (major)
        */

        actor.getClass().getCreatureStats(actor).setDrawState(DrawState_Nothing);

        // Stop if the target doesn't exist
        // Really we should be checking whether the target is currently registered with the MechanicsManager
        if (target == MWWorld::Ptr() || !target.getRefData().getCount() || !target.getRefData().isEnabled())
            return true;

        // Turn to target and move to it directly, without pathfinding.
        const osg::Vec3f targetDir = target.getRefData().getPosition().asVec3() - actor.getRefData().getPosition().asVec3();

        zTurn(actor, std::atan2(targetDir.x(), targetDir.y()), 0.f);
        actor.getClass().getMovementSettings(actor).mPosition[1] = 1;
        actor.getClass().getMovementSettings(actor).mPosition[0] = 0;

        if (MWBase::Environment::get().getWorld()->getMaxActivationDistance() >= targetDir.length())
        {
            /*
                Start of tes3mp addition

                Send an ID_OBJECT_ACTIVATE packet every time an object is activated here
            */
            mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
            objectList->reset();
            objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
            objectList->addObjectActivate(target, actor);
            objectList->sendObjectActivate();
            /*
                End of tes3mp addition
            */

            /*
                Start of tes3mp change (major)

                Disable unilateral activation on this client and expect the server's reply to our
                packet to do it instead

                Cancel the package to avoid an infinite activation loop, deviating from the behavior
                established in OpenMW in commit 48aba76ce904738d428e79f1ee24ce170f2a8309
            */
            //MWBase::Environment::get().getWorld()->activate(target, actor);
            return true;
            /*
                End of tes3mp change (major)
            */
        }
        return false;
    }

    void AiActivate::writeState(ESM::AiSequence::AiSequence &sequence) const
    {
        std::unique_ptr<ESM::AiSequence::AiActivate> activate(new ESM::AiSequence::AiActivate());
        activate->mTargetId = mObjectId;

        ESM::AiSequence::AiPackageContainer package;
        package.mType = ESM::AiSequence::Ai_Activate;
        package.mPackage = activate.release();
        sequence.mPackages.push_back(package);
    }

    AiActivate::AiActivate(const ESM::AiSequence::AiActivate *activate)
        : mObjectId(activate->mTargetId)
    {
    }
}
=======
#include "aiactivate.hpp"

#include <components/esm3/aisequence.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"

#include "../mwworld/class.hpp"

#include "creaturestats.hpp"
#include "movement.hpp"
#include "steering.hpp"

namespace MWMechanics
{
    AiActivate::AiActivate(const ESM::RefId& objectId, bool repeat)
        : TypedAiPackage<AiActivate>(repeat)
        , mObjectId(objectId)
    {
    }

    bool AiActivate::execute(
        const MWWorld::Ptr& actor, CharacterController& characterController, AiState& state, float duration)
    {
        const MWWorld::Ptr target
            = MWBase::Environment::get().getWorld()->searchPtr(mObjectId, false); // The target to follow

        actor.getClass().getCreatureStats(actor).setDrawState(DrawState::Nothing);

        // Stop if the target doesn't exist
        // Really we should be checking whether the target is currently registered with the MechanicsManager
        if (target == MWWorld::Ptr() || !target.getRefData().getCount() || !target.getRefData().isEnabled())
            return true;

        // Turn to target and move to it directly, without pathfinding.
        const osg::Vec3f targetDir
            = target.getRefData().getPosition().asVec3() - actor.getRefData().getPosition().asVec3();

        zTurn(actor, std::atan2(targetDir.x(), targetDir.y()), 0.f);
        actor.getClass().getMovementSettings(actor).mPosition[1] = 1;
        actor.getClass().getMovementSettings(actor).mPosition[0] = 0;

        if (MWBase::Environment::get().getWorld()->getMaxActivationDistance() >= targetDir.length())
        {
            // Note: we intentionally do not cancel package after activation here for backward compatibility with
            // original engine.
            MWBase::Environment::get().getWorld()->activate(target, actor);
        }
        return false;
    }

    void AiActivate::writeState(ESM::AiSequence::AiSequence& sequence) const
    {
        auto activate = std::make_unique<ESM::AiSequence::AiActivate>();
        activate->mTargetId = mObjectId;
        activate->mRepeat = getRepeat();

        ESM::AiSequence::AiPackageContainer package;
        package.mType = ESM::AiSequence::Ai_Activate;
        package.mPackage = std::move(activate);
        sequence.mPackages.push_back(std::move(package));
    }

    AiActivate::AiActivate(const ESM::AiSequence::AiActivate* activate)
        : AiActivate(activate->mTargetId, activate->mRepeat)
    {
    }
}
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
