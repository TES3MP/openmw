#ifndef OPENMW_PROCESSORPLAYERSPEECH_HPP
#define OPENMW_PROCESSORPLAYERSPEECH_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerSpeech : public PlayerProcessor
    {
    public:
        ProcessorPlayerSpeech()
        {
            BPP_INIT(ID_PLAYER_SPEECH)
        }

        virtual void Do(PlayerPacket &packet, BasePlayer *player)
        {
            if (isLocal())
            {
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Received ID_PLAYER_SPEECH about LocalPlayer from server");
                static_cast<LocalPlayer*>(player)->playSpeech();
            }
            else if (player != 0)
                static_cast<DedicatedPlayer*>(player)->playSpeech();
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERSPEECH_HPP
