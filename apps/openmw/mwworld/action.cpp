#include "action.hpp"

/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include "../mwmp/Main.hpp"
#include "../mwmp/Networking.hpp"
#include "../mwmp/CellController.hpp"
#include "../mwmp/ObjectList.hpp"
/*
    End of tes3mp addition
*/

#include "../mwbase/environment.hpp"

#include "../mwbase/soundmanager.hpp"
#include "../mwbase/windowmanager.hpp"

#include "../mwmechanics/actorutil.hpp"

const MWWorld::Ptr& MWWorld::Action::getTarget() const
{
    return mTarget;
}

void MWWorld::Action::setTarget(const MWWorld::Ptr& target)
{
    mTarget = target;
}

MWWorld::Action::Action(bool keepSound, const Ptr& target)
    : mKeepSound(keepSound)
    , mSoundOffset(0)
    , mTarget(target)
{
}

MWWorld::Action::~Action() {}

void MWWorld::Action::execute(const Ptr& actor, bool noSound)
{
    if (!mSoundId.empty() && !noSound)
    {
        MWSound::PlayMode envType = MWSound::PlayMode::Normal;

        // Action sounds should not have a distortion in GUI mode
        // example: take an item or drink a potion underwater
        if (actor == MWMechanics::getPlayer() && MWBase::Environment::get().getWindowManager()->isGuiMode())
        {
            envType = MWSound::PlayMode::NoEnv;
        }

<<<<<<< HEAD
        if(mKeepSound && actor == MWMechanics::getPlayer())
        {
            MWBase::Environment::get().getSoundManager()->playSound(mSoundId, 1.0, 1.0,
                MWSound::Type::Sfx, envType, mSoundOffset
            );

            /*
                Start of tes3mp addition

                Send an ID_OBJECT_SOUND packet every time the local player makes a sound here
            */
            mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
            objectList->reset();
            objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
            objectList->addObjectSound(actor, mSoundId, 1.0, 1.0);
            objectList->sendObjectSound();
            /*
                End of tes3mp addition
            */
        }
        else
        {
            bool local = mTarget.isEmpty() || !mTarget.isInCell(); // no usable target
            if(mKeepSound)
            {
                MWBase::Environment::get().getSoundManager()->playSound3D(
                    (local ? actor : mTarget).getRefData().getPosition().asVec3(),
                    mSoundId, 1.0, 1.0, MWSound::Type::Sfx, envType, mSoundOffset
                );

                /*
                    Start of tes3mp addition

                    Send an ID_OBJECT_SOUND packet every time a local actor makes a sound here
                */
                if (mwmp::Main::get().getCellController()->isLocalActor(actor))
                {
                    mwmp::ObjectList* objectList = mwmp::Main::get().getNetworking()->getObjectList();
                    objectList->reset();
                    objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
                    objectList->addObjectSound(local ? actor : mTarget, mSoundId, 1.0, 1.0);
                    objectList->sendObjectSound();
                }
                /*
                    End of tes3mp addition
                */
            }
            else
            {
                MWBase::Environment::get().getSoundManager()->playSound3D(local ? actor : mTarget,
                    mSoundId, 1.0, 1.0, MWSound::Type::Sfx, envType, mSoundOffset
                );

                /*
                    Start of tes3mp addition

                    Send an ID_OBJECT_SOUND packet every time a local actor makes a sound here
                */
                if (mwmp::Main::get().getCellController()->isLocalActor(actor))
                {
                    mwmp::ObjectList* objectList = mwmp::Main::get().getNetworking()->getObjectList();
                    objectList->reset();
                    objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
                    objectList->addObjectSound(local ? actor : mTarget, mSoundId, 1.0, 1.0);
                    objectList->sendObjectSound();
                }
                /*
                    End of tes3mp addition
                */
            }
=======
        if (mKeepSound && actor == MWMechanics::getPlayer())
            MWBase::Environment::get().getSoundManager()->playSound(
                mSoundId, 1.0, 1.0, MWSound::Type::Sfx, envType, mSoundOffset);
        else
        {
            bool local = mTarget.isEmpty() || !mTarget.isInCell(); // no usable target
            if (mKeepSound)
                MWBase::Environment::get().getSoundManager()->playSound3D(
                    (local ? actor : mTarget).getRefData().getPosition().asVec3(), mSoundId, 1.0, 1.0,
                    MWSound::Type::Sfx, envType, mSoundOffset);
            else
                MWBase::Environment::get().getSoundManager()->playSound3D(
                    local ? actor : mTarget, mSoundId, 1.0, 1.0, MWSound::Type::Sfx, envType, mSoundOffset);
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
        }
    }

    executeImp(actor);
}

void MWWorld::Action::setSound(const ESM::RefId& id)
{
    mSoundId = id;
}

void MWWorld::Action::setSoundOffset(float offset)
{
    mSoundOffset = offset;
}
