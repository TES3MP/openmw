#ifndef OPENMW_PROCESSORPLAYERMAP_HPP
#define OPENMW_PROCESSORPLAYERMAP_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerMap : public PlayerProcessor
    {
    public:
        ProcessorPlayerMap()
        {
            BPP_INIT(ID_PLAYER_MAP)
        }

        void Do(PlayerPacket &packet, std::shared_ptr<Player> player) override
        {
            DEBUG_PRINTF(strPacketID.c_str());

            // Not currently implemented
            //
            // To be dealt with later to save explored areas on local maps
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERMAP_HPP
