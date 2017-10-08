#ifndef OPENMW_PROCESSORPLAYERSHAPESHIFT_HPP
#define OPENMW_PROCESSORPLAYERSHAPESHIFT_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerShapeshift : public PlayerProcessor
    {
    public:
        ProcessorPlayerShapeshift()
        {
            BPP_INIT(ID_PLAYER_SHAPESHIFT)
        }

        void Do(PlayerPacket &packet, std::shared_ptr<Player> player) override
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Received %s from %s", strPacketID.c_str(), player->npc.mName.c_str());

            packet.Send(true);

            Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_PLAYER_SHAPESHIFT>(player);
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERSHAPESHIFT_HPP
