#include "repair.hpp"

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

namespace MWMechanics
{

    void Repair::repair(const MWWorld::Ptr& itemToRepair)
    {
        MWWorld::Ptr player = getPlayer();
        MWWorld::LiveCellRef<ESM::Repair>* ref = mTool.get<ESM::Repair>();

<<<<<<< HEAD
        // repair by 'y' points
        int charge = itemToRepair.getClass().getItemHealth(itemToRepair);
        charge = std::min(charge + y, itemToRepair.getClass().getItemMaxHealth(itemToRepair));

        /*
            Start of tes3mp change (minor)

            Send PlayerInventory packets that replace the original item with the new one
        */
        mwmp::LocalPlayer *localPlayer = mwmp::Main::get().getLocalPlayer();
        mwmp::Item removedItem = MechanicsHelper::getItem(itemToRepair, 1);

        itemToRepair.getCellRef().setCharge(charge);

        mwmp::Item addedItem = MechanicsHelper::getItem(itemToRepair, 1);

        localPlayer->sendItemChange(addedItem, mwmp::InventoryChanges::ADD);
        localPlayer->sendItemChange(removedItem, mwmp::InventoryChanges::REMOVE);
        /*
            End of tes3mp change (minor)
        */

        // attempt to re-stack item, in case it was fully repaired
        MWWorld::ContainerStoreIterator stacked = player.getClass().getContainerStore(player).restack(itemToRepair);
=======
        // unstack tool if required
        player.getClass().getContainerStore(player).unstack(mTool);

        // reduce number of uses left
        int uses = mTool.getClass().getItemHealth(mTool);
        uses -= std::min(uses, 1);
        mTool.getCellRef().setCharge(uses);
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        MWMechanics::CreatureStats& stats = player.getClass().getCreatureStats(player);

        float fatigueTerm = stats.getFatigueTerm();
        float pcStrength = stats.getAttribute(ESM::Attribute::Strength).getModified();
        float pcLuck = stats.getAttribute(ESM::Attribute::Luck).getModified();
        float armorerSkill = player.getClass().getSkill(player, ESM::Skill::Armorer);

<<<<<<< HEAD
        MWBase::Environment::get().getWindowManager()->playSound("Repair");
        MWBase::Environment::get().getWindowManager()->messageBox("#{sRepairSuccess}");

        /*
            Start of tes3mp addition

            Send an ID_OBJECT_SOUND packet every time the player makes a sound here
        */
        mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
        objectList->reset();
        objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
        objectList->addObjectSound(MWMechanics::getPlayer(), "Repair", 1.0, 1.0);
        objectList->sendObjectSound();
        /*
            End of tes3mp addition
        */
    }
    else
    {
        MWBase::Environment::get().getWindowManager()->playSound("Repair Fail");
        MWBase::Environment::get().getWindowManager()->messageBox("#{sRepairFailed}");

        /*
            Start of tes3mp addition

            Send an ID_OBJECT_SOUND packet every time the player makes a sound here
        */
        mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
        objectList->reset();
        objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
        objectList->addObjectSound(MWMechanics::getPlayer(), "Repair Fail", 1.0, 1.0);
        objectList->sendObjectSound();
        /*
            End of tes3mp addition
        */
    }
=======
        float fRepairAmountMult = MWBase::Environment::get()
                                      .getESMStore()
                                      ->get<ESM::GameSetting>()
                                      .find("fRepairAmountMult")
                                      ->mValue.getFloat();
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        float toolQuality = ref->mBase->mData.mQuality;

        float x = (0.1f * pcStrength + 0.1f * pcLuck + armorerSkill) * fatigueTerm;

        auto& prng = MWBase::Environment::get().getWorld()->getPrng();
        int roll = Misc::Rng::roll0to99(prng);
        if (roll <= x)
        {
            int y = static_cast<int>(fRepairAmountMult * toolQuality * roll);
            y = std::max(1, y);

            // repair by 'y' points
            int charge = itemToRepair.getClass().getItemHealth(itemToRepair);
            charge = std::min(charge + y, itemToRepair.getClass().getItemMaxHealth(itemToRepair));
            itemToRepair.getCellRef().setCharge(charge);

            // attempt to re-stack item, in case it was fully repaired
            MWWorld::ContainerStoreIterator stacked = player.getClass().getContainerStore(player).restack(itemToRepair);

            // set the OnPCRepair variable on the item's script
            const ESM::RefId& script = stacked->getClass().getScript(itemToRepair);
            if (!script.empty())
                stacked->getRefData().getLocals().setVarByInt(script, "onpcrepair", 1);

            // increase skill
            player.getClass().skillUsageSucceeded(player, ESM::Skill::Armorer, 0);

            MWBase::Environment::get().getWindowManager()->playSound(ESM::RefId::stringRefId("Repair"));
            MWBase::Environment::get().getWindowManager()->messageBox("#{sRepairSuccess}");
        }
        else
        {
            MWBase::Environment::get().getWindowManager()->playSound(ESM::RefId::stringRefId("Repair Fail"));
            MWBase::Environment::get().getWindowManager()->messageBox("#{sRepairFailed}");
        }

        // tool used up?
        if (mTool.getCellRef().getCharge() == 0)
        {
            MWWorld::ContainerStore& store = player.getClass().getContainerStore(player);

            store.remove(mTool, 1);

            std::string message = MWBase::Environment::get()
                                      .getESMStore()
                                      ->get<ESM::GameSetting>()
                                      .find("sNotifyMessage51")
                                      ->mValue.getString();
            message = Misc::StringUtils::format(message, mTool.getClass().getName(mTool));

            MWBase::Environment::get().getWindowManager()->messageBox(message);

            // try to find a new tool of the same ID
            for (MWWorld::ContainerStoreIterator iter(store.begin()); iter != store.end(); ++iter)
            {
                if (iter->getCellRef().getRefId() == mTool.getCellRef().getRefId())
                {
                    mTool = *iter;

                    MWBase::Environment::get().getWindowManager()->playSound(ESM::RefId::stringRefId("Item Repair Up"));

                    break;
                }
            }
        }
    }

}
