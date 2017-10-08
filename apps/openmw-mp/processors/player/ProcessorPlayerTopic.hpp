#ifndef OPENMW_PROCESSORPLAYERTOPIC_HPP
#define OPENMW_PROCESSORPLAYERTOPIC_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerTopic : public PlayerProcessor
    {
    public:
        ProcessorPlayerTopic()
        {
            BPP_INIT(ID_PLAYER_TOPIC)
        }

        void Do(PlayerPacket &packet, std::shared_ptr<Player> player) override
        {
            DEBUG_PRINTF(strPacketID.c_str());

            packet.Send(true);

            //Todo: Script::Call<Script::CallbackIdentity("OnPlayerTopic")>(player.getId());
            Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_PLAYER_TOPIC>(player);
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERTOPIC_HPP
