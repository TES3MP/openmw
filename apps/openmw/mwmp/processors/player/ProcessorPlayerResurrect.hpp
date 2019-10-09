//
// Created by koncord on 16.04.17.
//

#ifndef OPENMW_PROCESSORPLAYERRESURRECT_HPP
#define OPENMW_PROCESSORPLAYERRESURRECT_HPP

#include "../PlayerProcessor.hpp"
#include "apps/openmw/mwmp/Main.hpp"
#include "apps/openmw/mwmp/Networking.hpp"

namespace mwmp
{
    class ProcessorPlayerResurrect : public PlayerProcessor
    {
    public:
        ProcessorPlayerResurrect()
        {
            BPP_INIT(ID_PLAYER_RESURRECT)
        }

        virtual void Do(PlayerPacket &packet, BasePlayer *player)
        {
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Received ID_PLAYER_RESURRECT from server");
            
            if (isLocal())
            {
                LOG_APPEND(MWMPLog::LOG_INFO, "- Packet was about me with resurrectType of %i", player->resurrectType);

                static_cast<LocalPlayer*>(player)->resurrect();
            }
            else if (player != 0)
            {
                LOG_APPEND(MWMPLog::LOG_INFO, "- Packet was about %s", player->npc.mName.c_str());

                player->creatureStats.mDead = false;
                if (player->creatureStats.mDynamic[0].mMod < 1)
                    player->creatureStats.mDynamic[0].mMod = 1;
                player->creatureStats.mDynamic[0].mCurrent = player->creatureStats.mDynamic[0].mMod;

                MWWorld::Ptr ptr = static_cast<DedicatedPlayer*>(player)->getPtr();

                ptr.getClass().getCreatureStats(ptr).resurrect();

                MWMechanics::DynamicStat<float> health;
                health.readState(player->creatureStats.mDynamic[0]);
                ptr.getClass().getCreatureStats(ptr).setHealth(health);
            }
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERRESURRECT_HPP
