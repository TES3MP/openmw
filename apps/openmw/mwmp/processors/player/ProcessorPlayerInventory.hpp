#ifndef OPENMW_PROCESSORPLAYERUPDATEINVENTORY_HPP
#define OPENMW_PROCESSORPLAYERUPDATEINVENTORY_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerInventory : public PlayerProcessor
    {
    public:
        ProcessorPlayerInventory()
        {
            BPP_INIT(ID_PLAYER_INVENTORY)
        }

        virtual void Do(PlayerPacket &packet, BasePlayer *player)
        {
            if (!isLocal()) return;

            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Received ID_PLAYER_INVENTORY about LocalPlayer from server");

            if (isRequest())
                static_cast<LocalPlayer*>(player)->updateInventory(true);
            else
            {
                LocalPlayer &localPlayer = static_cast<LocalPlayer&>(*player);
                int inventoryAction = localPlayer.inventoryChanges.action;

                // Because we send PlayerInventory packets from the same OpenMW methods that we use to set the
                // items received, we need to set a boolean to prevent resending the items set here
                localPlayer.isReceivingInventory = true;

                if (inventoryAction == InventoryChanges::ADD)
                    localPlayer.addItems();
                else if (inventoryAction == InventoryChanges::REMOVE)
                    localPlayer.removeItems();
                else // InventoryChanges::SET
                    localPlayer.setInventory();

                localPlayer.isReceivingInventory = false;
            }
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERUPDATEINVENTORY_HPP
