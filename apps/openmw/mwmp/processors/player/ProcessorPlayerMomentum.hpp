#ifndef OPENMW_PROCESSORPLAYERMOMENTUM_HPP
#define OPENMW_PROCESSORPLAYERMOMENTUM_HPP


#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerMomentum : public PlayerProcessor
    {
    public:
        ProcessorPlayerMomentum()
        {
            BPP_INIT(ID_PLAYER_MOMENTUM)
        }

        virtual void Do(PlayerPacket &packet, BasePlayer *player)
        {
            if (!isLocal()) return;

            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Received ID_PLAYER_MOMENTUM about LocalPlayer from server");

            if (!isRequest())
            {
                LocalPlayer &localPlayer = static_cast<LocalPlayer&>(*player);
                localPlayer.setMomentum();
            }
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERMOMENTUM_HPP
