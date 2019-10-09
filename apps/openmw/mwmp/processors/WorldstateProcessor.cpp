#include "../Main.hpp"
#include "../Networking.hpp"

#include "WorldstateProcessor.hpp"

using namespace mwmp;

template<class T>
typename BasePacketProcessor<T>::processors_t BasePacketProcessor<T>::processors;

bool WorldstateProcessor::Process(RakNet::Packet &packet, Worldstate &worldstate)
{
    RakNet::BitStream bsIn(&packet.data[1], packet.length, false);
    bsIn.Read(guid);
    worldstate.guid = guid;

    WorldstatePacket *myPacket = Main::get().getNetworking()->getWorldstatePacket(packet.data[0]);

    myPacket->setWorldstate(&worldstate);
    myPacket->SetReadStream(&bsIn);

    for (auto &processor : processors)
    {
        if (processor.first == packet.data[0])
        {
            myGuid = Main::get().getLocalPlayer()->guid;
            request = packet.length == myPacket->headerSize();

            worldstate.isValid = true;

            if (!request && !processor.second->avoidReading)
                myPacket->Read();

            if (worldstate.isValid)
                processor.second->Do(*myPacket, worldstate);
            else
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, "Received %s that failed integrity check and was ignored!", processor.second->strPacketID.c_str());

            return true;
        }
    }
    return false;
}
