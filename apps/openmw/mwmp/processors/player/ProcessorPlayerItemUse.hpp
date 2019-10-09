#ifndef OPENMW_PROCESSORPLAYERITEMUSE_HPP
#define OPENMW_PROCESSORPLAYERITEMUSE_HPP

#include "apps/openmw/mwbase/environment.hpp"
#include "apps/openmw/mwgui/inventorywindow.hpp"
#include "apps/openmw/mwgui/windowmanagerimp.hpp"
#include "apps/openmw/mwworld/inventorystore.hpp"

#include "apps/openmw/mwmp/MechanicsHelper.hpp"

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerItemUse : public PlayerProcessor
    {
    public:
        ProcessorPlayerItemUse()
        {
            BPP_INIT(ID_PLAYER_ITEM_USE)
        }

        virtual void Do(PlayerPacket &packet, BasePlayer *player)
        {
            if (!isLocal()) return;

            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Received ID_PLAYER_ITEM_USE about LocalPlayer from server");

            if (!isRequest())
            {
                LOG_APPEND(MWMPLog::LOG_INFO, "- refId: %s, count: %i, charge: %i, enchantmentCharge: %f, soul: %s",
                    player->usedItem.refId.c_str(), player->usedItem.count, player->usedItem.charge,
                    player->usedItem.enchantmentCharge, player->usedItem.soul.c_str());

                MWWorld::Ptr playerPtr = MWBase::Environment::get().getWorld()->getPlayerPtr();
                MWWorld::InventoryStore &inventoryStore = playerPtr.getClass().getInventoryStore(playerPtr);

                MWWorld::Ptr itemPtr = MechanicsHelper::getItemPtrFromStore(player->usedItem, inventoryStore);

                if (itemPtr)
                    MWBase::Environment::get().getWindowManager()->getInventoryWindow()->useItem(itemPtr);
                else
                    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, "Cannot use non-existent item %s", player->usedItem.refId.c_str());
            }
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERITEMUSE_HPP
