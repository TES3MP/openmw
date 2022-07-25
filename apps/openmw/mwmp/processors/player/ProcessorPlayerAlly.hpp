#ifndef OPENMW_PROCESSORPLAYERALLY_HPP
#define OPENMW_PROCESSORPLAYERALLY_HPP

#include "../PlayerProcessor.hpp"
#include "apps/openmw/mwmp/Main.hpp"
#include "apps/openmw/mwmp/LocalPlayer.hpp"
#include "apps/openmw/mwmp/MechanicsHelper.hpp"

namespace mwmp
{
    class ProcessorPlayerAlly final: public PlayerProcessor
    {
    public:
        ProcessorPlayerAlly()
        {
            BPP_INIT(ID_PLAYER_ALLY)
        }

        virtual void Do(PlayerPacket &packet, BasePlayer *player)
        {
            mwmp::LocalPlayer *localPlayer = mwmp::Main::get().getLocalPlayer();

            if (isLocal())
            {
                LOG_MESSAGE_SIMPLE(TimedLog::LOG_INFO, "Received ID_PLAYER_ALLY about LocalPlayer %s from server", localPlayer->npc.mName.c_str());

                for (std::vector<RakNet::RakNetGUID>::iterator iter = localPlayer->alliedPlayers.begin(); iter != localPlayer->alliedPlayers.end(); )
                {
                    DedicatedPlayer *dedicatedPlayer = PlayerList::getPlayer(*iter);

                    if (dedicatedPlayer)
                    {
                        LOG_APPEND(TimedLog::LOG_INFO, "- Adding DedicatedPlayer %s to our allied players", dedicatedPlayer->npc.mName.c_str());
                        MechanicsHelper::makePeace(localPlayer->getPlayerPtr(), dedicatedPlayer->getPtr());
                    }

                    ++iter;
                }
            }
            else if (player != 0)
            {
                LOG_MESSAGE_SIMPLE(TimedLog::LOG_INFO, "Received ID_PLAYER_ALLY about DedicatedPlayer %s from server", player->npc.mName.c_str());

                for (std::vector<RakNet::RakNetGUID>::iterator iter = player->alliedPlayers.begin(); iter != player->alliedPlayers.end(); )
                {
                    if (*iter == localPlayer->guid)
                    {
                        LOG_APPEND(TimedLog::LOG_INFO, "- Adding LocalPlayer %s to their allied players", localPlayer->npc.mName.c_str());
                    }
                    else
                    {
                        DedicatedPlayer *otherDedicatedPlayer = PlayerList::getPlayer(*iter);

                        if (otherDedicatedPlayer)
                        {
                            LOG_APPEND(TimedLog::LOG_INFO, "- Adding DedicatedPlayer %s to their allied players", otherDedicatedPlayer->npc.mName.c_str());
                            MechanicsHelper::makePeace(static_cast<DedicatedPlayer*>(player)->getPtr(), otherDedicatedPlayer->getPtr());
                        }
                    }

                    ++iter;
                }
            }
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERALLY_HPP
