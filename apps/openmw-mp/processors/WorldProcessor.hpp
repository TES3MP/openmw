//
// Created by koncord on 03.04.17.
//

#ifndef OPENMW_WORLDPROCESSOR_HPP
#define OPENMW_WORLDPROCESSOR_HPP


#include <components/openmw-mp/Base/BasePacketProcessor.hpp>
#include <components/openmw-mp/Packets/BasePacket.hpp>
#include <components/openmw-mp/Packets/World/WorldPacket.hpp>
#include <components/openmw-mp/NetworkMessages.hpp>
#include "Players.hpp"

namespace mwmp
{
    class WorldProcessor : public BasePacketProcessor<WorldProcessor>
    {
    public:

        virtual void Do(WorldPacket &packet, std::shared_ptr<Player> player, BaseEvent &event);

        static bool Process(RakNet::Packet &packet, BaseEvent &event) noexcept;
    };
}

#endif //OPENMW_WORLDPROCESSOR_HPP
