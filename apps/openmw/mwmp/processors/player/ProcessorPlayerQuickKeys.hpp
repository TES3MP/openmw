#ifndef OPENMW_PROCESSORPLAYERQUICKKEYS_HPP
#define OPENMW_PROCESSORPLAYERQUICKKEYS_HPP


#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerQuickKeys : public PlayerProcessor
    {
    public:
        ProcessorPlayerQuickKeys()
        {
            BPP_INIT(ID_PLAYER_QUICKKEYS)
        }

        virtual void Do(PlayerPacket &packet, BasePlayer *player)
        {
            if (!isLocal()) return;

            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Received ID_PLAYER_QUICKKEYS about LocalPlayer from server");

            if (!isRequest())
            {
                LocalPlayer &localPlayer = static_cast<LocalPlayer&>(*player);

                // Because we send PlayerQuickKeys packets from the same OpenMW methods that we use to set the
                // quick keys received, we need to set a boolean to prevent resending the keys set here
                localPlayer.isReceivingQuickKeys = true;

                localPlayer.setQuickKeys();

                localPlayer.isReceivingQuickKeys = false;
            }
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERQUICKKEYS_HPP
