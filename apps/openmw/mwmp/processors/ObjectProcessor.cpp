#include "../Main.hpp"
#include "../Networking.hpp"

#include "ObjectProcessor.hpp"

using namespace mwmp;

template<class T>
typename BasePacketProcessor<T>::processors_t BasePacketProcessor<T>::processors;

bool ObjectProcessor::Process(RakNet::Packet &packet, ObjectList &objectList)
{
    RakNet::BitStream bsIn(&packet.data[1], packet.length, false);
    bsIn.Read(guid);
    objectList.guid = guid;

    ObjectPacket *myPacket = Main::get().getNetworking()->getObjectPacket(packet.data[0]);

    myPacket->setObjectList(&objectList);
    myPacket->SetReadStream(&bsIn);

    for (auto &processor: processors)
    {
        if (processor.first == packet.data[0])
        {
            myGuid = Main::get().getLocalPlayer()->guid;
            request = packet.length == myPacket->headerSize();

            objectList.isValid = true;

            if (!request && !processor.second->avoidReading)
                myPacket->Read();

            if (objectList.isValid)
                processor.second->Do(*myPacket, objectList);
            else
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, "Received %s that failed integrity check and was ignored!", processor.second->strPacketID.c_str());

            return true;
        }
    }
    return false;
}
