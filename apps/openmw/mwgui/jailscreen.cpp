#include <MyGUI_ScrollBar.h>

/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include "../mwmp/Main.hpp"
#include "../mwmp/LocalPlayer.hpp"
/*
    End of tes3mp addition
*/

#include <components/misc/rng.hpp>
#include <components/misc/strings/format.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/world.hpp"

#include "../mwmechanics/actorutil.hpp"
#include "../mwmechanics/npcstats.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwworld/store.hpp"

#include "jailscreen.hpp"

namespace MWGui
{
    JailScreen::JailScreen()
        : WindowBase("openmw_jail_screen.layout")
        , mDays(1)
        , mFadeTimeRemaining(0)
        , mTimeAdvancer(0.01f)
    {
        getWidget(mProgressBar, "ProgressBar");

        mTimeAdvancer.eventProgressChanged += MyGUI::newDelegate(this, &JailScreen::onJailProgressChanged);
        mTimeAdvancer.eventFinished += MyGUI::newDelegate(this, &JailScreen::onJailFinished);

        center();
    }

    void JailScreen::goToJail(int days)
    {
        mDays = days;

        MWBase::Environment::get().getWindowManager()->fadeScreenOut(0.5);
        mFadeTimeRemaining = 0.5;

        setVisible(false);
        mProgressBar->setScrollRange(100 + 1);
        mProgressBar->setScrollPosition(0);
        mProgressBar->setTrackSize(0);

        /*
            Start of tes3mp addition

            If we've received a packet overriding the default jail progress text, use the new text
        */
        if (!mwmp::Main::get().getLocalPlayer()->jailProgressText.empty())
            setText("LoadingText", mwmp::Main::get().getLocalPlayer()->jailProgressText);
        /*
            End of tes3mp addition
        */
    }

    void JailScreen::onFrame(float dt)
    {
        mTimeAdvancer.onFrame(dt);

        if (mFadeTimeRemaining <= 0)
            return;

        mFadeTimeRemaining -= dt;

        if (mFadeTimeRemaining <= 0)
        {
            MWWorld::Ptr player = MWMechanics::getPlayer();
<<<<<<< HEAD

            /*
                Start of tes3mp change (minor)

                Prevent teleportation to jail if specified
            */
            if (!mwmp::Main::get().getLocalPlayer()->ignoreJailTeleportation)
            {
                MWBase::Environment::get().getWorld()->teleportToClosestMarker(player, "prisonmarker");
                MWBase::Environment::get().getWindowManager()->fadeScreenOut(0.f); // override fade-in caused by cell transition
            }
            /*
                End of tes3mp change (minor)
            */
=======
            MWBase::Environment::get().getWorld()->teleportToClosestMarker(
                player, ESM::RefId::stringRefId("prisonmarker"));
            MWBase::Environment::get().getWindowManager()->fadeScreenOut(
                0.f); // override fade-in caused by cell transition
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

            setVisible(true);
            mTimeAdvancer.run(100);
        }
    }

    void JailScreen::onJailProgressChanged(int cur, int /*total*/)
    {
        mProgressBar->setScrollPosition(0);
        mProgressBar->setTrackSize(
            static_cast<int>(cur / (float)(mProgressBar->getScrollRange()) * mProgressBar->getLineSize()));
    }

    void JailScreen::onJailFinished()
    {
        MWBase::Environment::get().getWindowManager()->removeGuiMode(MWGui::GM_Jail);
        MWBase::Environment::get().getWindowManager()->fadeScreenIn(0.5);

        MWWorld::Ptr player = MWMechanics::getPlayer();

        /*
            Start of tes3mp addition

            Declare pointer to LocalPlayer for use in other additions
        */
        mwmp::LocalPlayer* localPlayer = mwmp::Main::get().getLocalPlayer();
        /*
            End of tes3mp addition
        */

        MWBase::Environment::get().getMechanicsManager()->rest(mDays * 24, true);

        /*
            Start of tes3mp change (major)

            Multiplayer requires that time not get advanced here
        */
        //MWBase::Environment::get().getWorld()->advanceTime(mDays * 24);
        /*
            End of tes3mp change (major)
        */

        // We should not worsen corprus when in prison
        player.getClass().getCreatureStats(player).getActiveSpells().skipWorsenings(mDays * 24);

        const auto& skillStore = MWBase::Environment::get().getESMStore()->get<ESM::Skill>();
        std::set<const ESM::Skill*> skills;
        for (int day = 0; day < mDays; ++day)
        {
            auto& prng = MWBase::Environment::get().getWorld()->getPrng();
            const ESM::Skill* skill = skillStore.searchRandom({}, prng);
            skills.insert(skill);

<<<<<<< HEAD
            MWMechanics::SkillValue& value = player.getClass().getNpcStats(player).getSkill(skill);

            /*
                Start of tes3mp change (minor)

                Disable increases for Security and Sneak when using ignoreJailSkillIncreases
            */
            if (localPlayer->ignoreJailSkillIncreases)
                value.setBase(std::max(0.f, value.getBase()-1));
            else if (skill == ESM::Skill::Security || skill == ESM::Skill::Sneak)
            /*
                End of tes3mp change (minor)
            */
=======
            MWMechanics::SkillValue& value = player.getClass().getNpcStats(player).getSkill(skill->mId);
            if (skill->mId == ESM::Skill::Security || skill->mId == ESM::Skill::Sneak)
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
                value.setBase(std::min(100.f, value.getBase() + 1));
            else
                value.setBase(std::max(0.f, value.getBase() - 1));
        }

        const MWWorld::Store<ESM::GameSetting>& gmst
            = MWBase::Environment::get().getESMStore()->get<ESM::GameSetting>();

        std::string message;
        if (mDays == 1)
            message = gmst.find("sNotifyMessage42")->mValue.getString();
        else
            message = gmst.find("sNotifyMessage43")->mValue.getString();

        /*
            Start of tes3mp addition

            If we've received a packet overriding the default jail end text, use the new text
        */
        if (!localPlayer->jailEndText.empty())
            message = mwmp::Main::get().getLocalPlayer()->jailEndText;
        /*
            End of tes3mp addition
        */

        message = Misc::StringUtils::format(message, mDays);

        for (const ESM::Skill* skill : skills)
        {
            int skillValue = player.getClass().getNpcStats(player).getSkill(skill->mId).getBase();
            std::string skillMsg = gmst.find("sNotifyMessage44")->mValue.getString();
<<<<<<< HEAD

            /*
                Start of tes3mp change (minor)

                Account for usage of ignoreJailSkillIncreases
            */
            if (!localPlayer->ignoreJailSkillIncreases &&
                (skill == ESM::Skill::Sneak || skill == ESM::Skill::Security))
            /*
                End of tes3mp change (minor)
            */
=======
            if (skill->mId == ESM::Skill::Sneak || skill->mId == ESM::Skill::Security)
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
                skillMsg = gmst.find("sNotifyMessage39")->mValue.getString();

            skillMsg = Misc::StringUtils::format(skillMsg, skill->mName, skillValue);
            message += "\n" + skillMsg;
        }

        /*
            Start of tes3mp addition

            Reset all PlayerJail-related overrides
        */
        localPlayer->ignoreJailTeleportation = false;
        localPlayer->ignoreJailSkillIncreases = false;
        localPlayer->jailProgressText = "";
        localPlayer->jailEndText = "";
        /*
            End of tes3mp addition
        */

        std::vector<std::string> buttons;
        buttons.emplace_back("#{Interface:OK}");
        MWBase::Environment::get().getWindowManager()->interactiveMessageBox(message, buttons);
    }
}
