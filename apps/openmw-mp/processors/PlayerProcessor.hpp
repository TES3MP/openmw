//
// Created by koncord on 31.03.17.
//

#ifndef OPENMW_BASEPLAYERPROCESSOR_HPP
#define OPENMW_BASEPLAYERPROCESSOR_HPP

#include <components/openmw-mp/Base/BasePacketProcessor.hpp>
#include <components/openmw-mp/Packets/BasePacket.hpp>
#include <components/openmw-mp/NetworkMessages.hpp>
#include "Utils.hpp"
#include "Players.hpp"
#include "../Script/EventController.hpp"

namespace mwmp
{
    class PlayerProcessor : public BasePacketProcessor<PlayerProcessor>
    {
    public:

        virtual void Do(PlayerPacket &packet, std::shared_ptr<Player> player) = 0;

        static bool Process(RakNet::Packet &packet) noexcept;
    };
}

#endif //OPENMW_BASEPLAYERPROCESSOR_HPP
