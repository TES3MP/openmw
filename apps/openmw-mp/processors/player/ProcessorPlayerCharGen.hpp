//
// Created by koncord on 01.04.17.
//

#ifndef OPENMW_PROCESSORPLAYERCHARGEN_HPP
#define OPENMW_PROCESSORPLAYERCHARGEN_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerCharGen : public PlayerProcessor
    {
    public:
        ProcessorPlayerCharGen()
        {
            BPP_INIT(ID_PLAYER_CHARGEN)
        }

        void Do(PlayerPacket &packet, std::shared_ptr<Player> player) override
        {
            DEBUG_PRINTF(strPacketID.c_str());

            if (player->charGenStage.current == player->charGenStage.end && player->charGenStage.current != 0)
                Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_PLAYER_ENDCHARGEN>(player);
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERCHARGEN_HPP
