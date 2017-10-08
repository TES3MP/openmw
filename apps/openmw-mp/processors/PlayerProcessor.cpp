//
// Created by koncord on 31.03.17.
//

#include "PlayerProcessor.hpp"
#include "Networking.hpp"

using namespace mwmp;

template<class T>
typename BasePacketProcessor<T>::processors_t BasePacketProcessor<T>::processors;

bool PlayerProcessor::Process(RakNet::Packet &packet) noexcept
{
    for (auto &processor : processors)
    {
        if (processor.first == packet.data[0])
        {
            auto player = Players::getPlayerByGUID(packet.guid);
            PlayerPacket *myPacket = Networking::get().getPlayerPacketController()->GetPacket(packet.data[0]);
            myPacket->setPlayer(player.get());

            if (!processor.second->avoidReading)
                myPacket->Read();

            processor.second->Do(*myPacket, player);
            return true;
        }
    }
    return false;
}
