#ifndef OPENMW_PROCESSORPLAYERBOUNTY_HPP
#define OPENMW_PROCESSORPLAYERBOUNTY_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerBounty : public PlayerProcessor
    {
    public:
        ProcessorPlayerBounty()
        {
            BPP_INIT(ID_PLAYER_BOUNTY)
        }

        void Do(PlayerPacket &packet, std::shared_ptr<Player> player) override
        {
            Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_PLAYER_BOUNTY>(player);
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERBOUNTY_HPP
