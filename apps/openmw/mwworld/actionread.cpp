#include "actionread.hpp"

<<<<<<< HEAD
/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include "../mwmp/Main.hpp"
#include "../mwmp/LocalPlayer.hpp"
/*
    End of tes3mp addition
*/
=======
#include <components/esm3/loadbook.hpp>
#include <components/esm3/loadclas.hpp>
#include <components/esm3/loadskil.hpp>
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/world.hpp"

#include "../mwmechanics/actorutil.hpp"
#include "../mwmechanics/npcstats.hpp"

#include "class.hpp"
#include "esmstore.hpp"

namespace MWWorld
{
    ActionRead::ActionRead(const MWWorld::Ptr& object)
        : Action(false, object)
    {
    }

    void ActionRead::executeImp(const MWWorld::Ptr& actor)
    {

        if (actor != MWMechanics::getPlayer())
            return;

        // Ensure we're not in combat
        if (MWMechanics::isPlayerInCombat()
            // Reading in combat is still allowed if the scroll/book is not in the player inventory yet
            // (since otherwise, there would be no way to pick it up)
            && getTarget().getContainerStore() == &actor.getClass().getContainerStore(actor))
        {
            MWBase::Environment::get().getWindowManager()->messageBox("#{sInventoryMessage4}");
            return;
        }

        LiveCellRef<ESM::Book>* ref = getTarget().get<ESM::Book>();

        if (ref->mBase->mData.mIsScroll)
            MWBase::Environment::get().getWindowManager()->pushGuiMode(MWGui::GM_Scroll, getTarget());
        else
            MWBase::Environment::get().getWindowManager()->pushGuiMode(MWGui::GM_Book, getTarget());

        MWMechanics::NpcStats& npcStats = actor.getClass().getNpcStats(actor);

        // Skill gain from books
        ESM::RefId skill = ESM::Skill::indexToRefId(ref->mBase->mData.mSkillId);
        if (!skill.empty() && !npcStats.hasBeenUsed(ref->mBase->mId))
        {
            MWWorld::LiveCellRef<ESM::NPC>* playerRef = actor.get<ESM::NPC>();

            const ESM::Class* class_
                = MWBase::Environment::get().getESMStore()->get<ESM::Class>().find(playerRef->mBase->mClass);

            npcStats.increaseSkill(skill, *class_, true, true);

<<<<<<< HEAD
            npcStats.flagAsUsed (ref->mBase->mId);

            /*
                Start of tes3mp addition

                Send an ID_PLAYER_BOOK packet every time a player reads a skill book
            */
            mwmp::Main::get().getLocalPlayer()->sendBook(ref->mBase->mId);
            /*
                End of tes3mp addition
            */
=======
            npcStats.flagAsUsed(ref->mBase->mId);
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
        }
    }
}
