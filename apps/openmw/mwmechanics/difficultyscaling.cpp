#include "difficultyscaling.hpp"

#include <components/settings/values.hpp>

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

#include "../mwbase/world.hpp"
=======
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
#include "../mwbase/environment.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwworld/ptr.hpp"

#include "actorutil.hpp"

float scaleDamage(float damage, const MWWorld::Ptr& attacker, const MWWorld::Ptr& victim)
{
    const MWWorld::Ptr& player = MWMechanics::getPlayer();

    static const float fDifficultyMult
        = MWBase::Environment::get().getESMStore()->get<ESM::GameSetting>().find("fDifficultyMult")->mValue.getFloat();

<<<<<<< HEAD
    /*
        Start of tes3mp change (major)

        Use difficulty setting received from server instead of basing it on client settings
    */
    difficultySetting = mwmp::Main::get().getLocalPlayer()->difficulty;
    /*
        End of tes3mp change (major)
    */

    static const float fDifficultyMult = MWBase::Environment::get().getWorld()->getStore().get<ESM::GameSetting>().find("fDifficultyMult")->mValue.getFloat();

    float difficultyTerm = 0.01f * difficultySetting;
=======
    const float difficultyTerm = 0.01f * Settings::game().mDifficulty;
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

    float x = 0;
    if (victim == player)
    {
        if (difficultyTerm > 0)
            x = fDifficultyMult * difficultyTerm;
        else
            x = difficultyTerm / fDifficultyMult;
    }
    else if (attacker == player)
    {
        if (difficultyTerm > 0)
            x = -difficultyTerm / fDifficultyMult;
        else
            x = fDifficultyMult * (-difficultyTerm);
    }

    damage *= 1 + x;
    return damage;
}
