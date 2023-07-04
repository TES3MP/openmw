#include "recharge.hpp"

#include <components/esm3/loadcrea.hpp>
#include <components/esm3/loadench.hpp>
#include <components/misc/rng.hpp>
#include <components/misc/strings/format.hpp>

<<<<<<< HEAD
/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include "../mwmp/Main.hpp"
#include "../mwmp/Networking.hpp"
#include "../mwmp/LocalPlayer.hpp"
#include "../mwmp/MechanicsHelper.hpp"
/*
    End of tes3mp addition
*/

#include "../mwbase/world.hpp"
=======
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/world.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/containerstore.hpp"
#include "../mwworld/esmstore.hpp"

#include "actorutil.hpp"
#include "creaturestats.hpp"
#include "spellutil.hpp"

namespace MWMechanics
{

    bool rechargeItem(const MWWorld::Ptr& item, const float maxCharge, const float duration)
    {
        float charge = item.getCellRef().getEnchantmentCharge();
        if (charge == -1 || charge == maxCharge)
            return false;

        static const float fMagicItemRechargePerSecond = MWBase::Environment::get()
                                                             .getESMStore()
                                                             ->get<ESM::GameSetting>()
                                                             .find("fMagicItemRechargePerSecond")
                                                             ->mValue.getFloat();

        item.getCellRef().setEnchantmentCharge(std::min(charge + fMagicItemRechargePerSecond * duration, maxCharge));
        return true;
    }

    bool rechargeItem(const MWWorld::Ptr& item, const MWWorld::Ptr& gem)
    {
        if (!gem.getRefData().getCount())
            return false;

        MWWorld::Ptr player = MWMechanics::getPlayer();
        MWMechanics::CreatureStats& stats = player.getClass().getCreatureStats(player);

        float luckTerm = 0.1f * stats.getAttribute(ESM::Attribute::Luck).getModified();
        if (luckTerm < 1 || luckTerm > 10)
            luckTerm = 1;

        float intelligenceTerm = 0.2f * stats.getAttribute(ESM::Attribute::Intelligence).getModified();

        if (intelligenceTerm > 20)
            intelligenceTerm = 20;
        if (intelligenceTerm < 1)
            intelligenceTerm = 1;

        float x = (player.getClass().getSkill(player, ESM::Skill::Enchant) + intelligenceTerm + luckTerm)
            * stats.getFatigueTerm();
        auto& prng = MWBase::Environment::get().getWorld()->getPrng();
        int roll = Misc::Rng::roll0to99(prng);
        if (roll < x)
        {
            const ESM::RefId& soul = gem.getCellRef().getSoul();
            const ESM::Creature* creature = MWBase::Environment::get().getESMStore()->get<ESM::Creature>().find(soul);

            float restored = creature->mData.mSoul * (roll / x);

            const ESM::Enchantment* enchantment
                = MWBase::Environment::get().getESMStore()->get<ESM::Enchantment>().find(
                    item.getClass().getEnchantment(item));
            const int maxCharge = MWMechanics::getEnchantmentCharge(*enchantment);
            item.getCellRef().setEnchantmentCharge(
                std::min(item.getCellRef().getEnchantmentCharge() + restored, static_cast<float>(maxCharge)));

<<<<<<< HEAD
        /*
            Start of tes3mp change (minor)

            Send PlayerInventory packets that replace the original item with the new one
        */
        mwmp::LocalPlayer *localPlayer = mwmp::Main::get().getLocalPlayer();
        mwmp::Item removedItem = MechanicsHelper::getItem(item, 1);

        item.getCellRef().setEnchantmentCharge(
            std::min(item.getCellRef().getEnchantmentCharge() + restored, static_cast<float>(enchantment->mData.mCharge)));

        mwmp::Item addedItem = MechanicsHelper::getItem(item, 1);

        localPlayer->sendItemChange(addedItem, mwmp::InventoryChanges::ADD);
        localPlayer->sendItemChange(removedItem, mwmp::InventoryChanges::REMOVE);
        /*
            End of tes3mp change (minor)
        */

        MWBase::Environment::get().getWindowManager()->playSound("Enchant Success");

        player.getClass().getContainerStore(player).restack(item);

        /*
            Start of tes3mp addition

            Send an ID_OBJECT_SOUND packet every time the player makes a sound here
        */
        mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
        objectList->reset();
        objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
        objectList->addObjectSound(MWMechanics::getPlayer(), "Enchant Success", 1.0, 1.0);
        objectList->sendObjectSound();
        /*
            End of tes3mp addition
        */
    }
    else
    {
        MWBase::Environment::get().getWindowManager()->playSound("Enchant Fail");

        /*
            Start of tes3mp addition

            Send an ID_OBJECT_SOUND packet every time the player makes a sound here
        */
        mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
        objectList->reset();
        objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
        objectList->addObjectSound(MWMechanics::getPlayer(), "Enchant Fail", 1.0, 1.0);
        objectList->sendObjectSound();
        /*
            End of tes3mp addition
        */
    }

    player.getClass().skillUsageSucceeded (player, ESM::Skill::Enchant, 0);
    gem.getContainerStore()->remove(gem, 1, player);

    if (gem.getRefData().getCount() == 0)
    {
        std::string message = MWBase::Environment::get().getWorld()->getStore().get<ESM::GameSetting>().find("sNotifyMessage51")->mValue.getString();
        message = Misc::StringUtils::format(message, gem.getClass().getName(gem));

        MWBase::Environment::get().getWindowManager()->messageBox(message);

        // special case: readd Azura's Star
        if (Misc::StringUtils::ciEqual(gem.get<ESM::Miscellaneous>()->mBase->mId, "Misc_SoulGem_Azura"))
            player.getClass().getContainerStore(player).add("Misc_SoulGem_Azura", 1, player);
    }

    return true;
}
=======
            MWBase::Environment::get().getWindowManager()->playSound(ESM::RefId::stringRefId("Enchant Success"));

            player.getClass().getContainerStore(player).restack(item);
        }
        else
        {
            MWBase::Environment::get().getWindowManager()->playSound(ESM::RefId::stringRefId("Enchant Fail"));
        }

        player.getClass().skillUsageSucceeded(player, ESM::Skill::Enchant, 0);
        gem.getContainerStore()->remove(gem, 1);

        if (gem.getRefData().getCount() == 0)
        {
            std::string message = MWBase::Environment::get()
                                      .getESMStore()
                                      ->get<ESM::GameSetting>()
                                      .find("sNotifyMessage51")
                                      ->mValue.getString();
            message = Misc::StringUtils::format(message, gem.getClass().getName(gem));

            MWBase::Environment::get().getWindowManager()->messageBox(message);

            const ESM::RefId soulGemAzura = ESM::RefId::stringRefId("Misc_SoulGem_Azura");
            // special case: readd Azura's Star
            if (gem.get<ESM::Miscellaneous>()->mBase->mId == soulGemAzura)
                player.getClass().getContainerStore(player).add(soulGemAzura, 1);
        }

        return true;
    }
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

}
