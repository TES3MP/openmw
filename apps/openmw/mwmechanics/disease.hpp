#ifndef OPENMW_MECHANICS_DISEASE_H
#define OPENMW_MECHANICS_DISEASE_H

#include <components/esm3/loadmgef.hpp>
#include <components/misc/rng.hpp>
#include <components/misc/strings/format.hpp>

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

#include "../mwbase/windowmanager.hpp"
=======
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/world.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwworld/ptr.hpp"

#include "actorutil.hpp"
#include "creaturestats.hpp"
#include "spells.hpp"

namespace MWMechanics
{

    /// Call when \a actor has got in contact with \a carrier (e.g. hit by him, or loots him)
    /// @param actor The actor that will potentially catch diseases. Currently only the player can catch diseases.
    /// @param carrier The disease carrier.
    inline void diseaseContact(const MWWorld::Ptr& actor, const MWWorld::Ptr& carrier)
    {
        if (!carrier.getClass().isActor() || actor != getPlayer())
            return;

        float fDiseaseXferChance = MWBase::Environment::get()
                                       .getESMStore()
                                       ->get<ESM::GameSetting>()
                                       .find("fDiseaseXferChance")
                                       ->mValue.getFloat();

        const MagicEffects& actorEffects = actor.getClass().getCreatureStats(actor).getMagicEffects();

        Spells& spells = carrier.getClass().getCreatureStats(carrier).getSpells();
        for (const ESM::Spell* spell : spells)
        {
            if (actor.getClass().getCreatureStats(actor).getSpells().hasSpell(spell->mId))
                continue;

            float resist = 0.f;
            if (Spells::hasCorprusEffect(spell))
                resist = 1.f
                    - 0.01f
                        * (actorEffects.getOrDefault(ESM::MagicEffect::ResistCorprusDisease).getMagnitude()
                            - actorEffects.getOrDefault(ESM::MagicEffect::WeaknessToCorprusDisease).getMagnitude());
            else if (spell->mData.mType == ESM::Spell::ST_Disease)
                resist = 1.f
                    - 0.01f
                        * (actorEffects.getOrDefault(ESM::MagicEffect::ResistCommonDisease).getMagnitude()
                            - actorEffects.getOrDefault(ESM::MagicEffect::WeaknessToCommonDisease).getMagnitude());
            else if (spell->mData.mType == ESM::Spell::ST_Blight)
                resist = 1.f
                    - 0.01f
                        * (actorEffects.getOrDefault(ESM::MagicEffect::ResistBlightDisease).getMagnitude()
                            - actorEffects.getOrDefault(ESM::MagicEffect::WeaknessToBlightDisease).getMagnitude());
            else
                continue;

            int x = static_cast<int>(fDiseaseXferChance * 100 * resist);
            auto& prng = MWBase::Environment::get().getWorld()->getPrng();
            if (Misc::Rng::rollDice(10000, prng) < x)
            {
                // Contracted disease!
                actor.getClass().getCreatureStats(actor).getSpells().add(spell);
                MWBase::Environment::get().getWorld()->applyLoopingParticles(actor);

<<<<<<< HEAD
                /*
                    Start of tes3mp addition

                    Send an ID_PLAYER_SPELLBOOK packet every time a player gains a disease
                */
                mwmp::Main::get().getLocalPlayer()->sendSpellChange(it->first->mId, mwmp::SpellbookChanges::ADD);
                /*
                    End of tes3mp addition
                */

                std::string msg = "sMagicContractDisease";
                msg = MWBase::Environment::get().getWorld()->getStore().get<ESM::GameSetting>().find(msg)->mValue.getString();
=======
                std::string msg = MWBase::Environment::get()
                                      .getESMStore()
                                      ->get<ESM::GameSetting>()
                                      .find("sMagicContractDisease")
                                      ->mValue.getString();
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
                msg = Misc::StringUtils::format(msg, spell->mName);
                MWBase::Environment::get().getWindowManager()->messageBox(msg);
            }
        }
    }
}

#endif
