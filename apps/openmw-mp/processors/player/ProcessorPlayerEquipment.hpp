//
// Created by koncord on 01.04.17.
//

#ifndef OPENMW_PROCESSORPLAYEREQUIPMENT_HPP
#define OPENMW_PROCESSORPLAYEREQUIPMENT_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerEquipment : public PlayerProcessor
    {
    public:
        ProcessorPlayerEquipment()
        {
            BPP_INIT(ID_PLAYER_EQUIPMENT)
        }

        void Do(PlayerPacket &packet, std::shared_ptr<Player> player) override
        {
            DEBUG_PRINTF(strPacketID.c_str());

            player->sendToLoaded(&packet);

            Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_PLAYER_EQUIPMENT>(player);
        }
    };
}

#endif //OPENMW_PROCESSORPLAYEREQUIPMENT_HPP
