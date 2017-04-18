//
// Created by koncord on 16.04.17.
//

#ifndef OPENMW_PROCESSORPLAYERRESURRECT_HPP
#define OPENMW_PROCESSORPLAYERRESURRECT_HPP


#include "apps/openmw/mwmp/PlayerProcessor.hpp"
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
            avoidReading = true;
        }

        virtual void Do(PlayerPacket &packet, BasePlayer *player)
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Received ID_PLAYER_RESURRECT from server");
            
            if (isLocal())
            {
                LOG_APPEND(Log::LOG_INFO, "- Packet was about me");

                MWWorld::Ptr playerPtr = MWBase::Environment::get().getWorld()->getPlayerPtr();
                playerPtr.getClass().getCreatureStats(playerPtr).resurrect();

                // If this player had a weapon or spell readied when dying, they will
                // still have it readied but be unable to use it unless we clear it here
                playerPtr.getClass().getNpcStats(playerPtr).setDrawState(MWMechanics::DrawState_Nothing);

                packet.setPlayer(player);
                packet.Send(serverAddr);

                static_cast<LocalPlayer*>(player)->updateStatsDynamic(true);
                Main::get().getNetworking()->getPlayerPacket(ID_PLAYER_STATS_DYNAMIC)->setPlayer(player);
                Main::get().getNetworking()->getPlayerPacket(ID_PLAYER_STATS_DYNAMIC)->Send(serverAddr);
            }
            else
            {
                LOG_APPEND(Log::LOG_INFO, "- Packet was about %s", player->npc.mName.c_str());

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
