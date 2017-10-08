#ifndef OPENMW_PROCESSORPLAYERKILLCOUNT_HPP
#define OPENMW_PROCESSORPLAYERKILLCOUNT_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerKillCount : public PlayerProcessor
    {
    public:
        ProcessorPlayerKillCount()
        {
            BPP_INIT(ID_PLAYER_KILL_COUNT)
        }

        void Do(PlayerPacket &packet, std::shared_ptr<Player> player) override
        {
            DEBUG_PRINTF(strPacketID.c_str());

            packet.Send(true);

            Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_PLAYER_KILLCOUNT>(player);
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERKILLCOUNT_HPP
