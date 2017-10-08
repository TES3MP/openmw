//
// Created by koncord on 01.04.17.
//

#ifndef OPENMW_PROCESSORPLAYERATTACK_HPP
#define OPENMW_PROCESSORPLAYERATTACK_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerAttack : public PlayerProcessor
    {
        PlayerPacketController *playerController;
    public:
        ProcessorPlayerAttack()
        {
            BPP_INIT(ID_PLAYER_ATTACK)
            playerController = Networking::get().getPlayerPacketController();
        }

        void Do(PlayerPacket &packet, std::shared_ptr<Player> player) override
        {
            DEBUG_PRINTF(strPacketID.c_str());

            if (!player->creatureStats.mDead)
            {
                player->sendToLoaded(&packet);
            }
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERATTACK_HPP
