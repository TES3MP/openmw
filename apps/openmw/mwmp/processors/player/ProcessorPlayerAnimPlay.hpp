#ifndef OPENMW_PROCESSORPLAYERANIMPLAY_HPP
#define OPENMW_PROCESSORPLAYERANIMPLAY_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerAnimPlay : public PlayerProcessor
    {
    public:
        ProcessorPlayerAnimPlay()
        {
            BPP_INIT(ID_PLAYER_ANIM_PLAY)
        }

        virtual void Do(PlayerPacket &packet, BasePlayer *player)
        {
            if (isLocal())
            {
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Received ID_PLAYER_ANIM_PLAY about LocalPlayer from server");
                static_cast<LocalPlayer*>(player)->playAnimation();
            }
            else if (player != 0)
                static_cast<DedicatedPlayer*>(player)->playAnimation();
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERANIMPLAY_HPP
