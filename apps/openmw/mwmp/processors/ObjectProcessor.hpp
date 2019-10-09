#ifndef OPENMW_OBJECTPROCESSSOR_HPP
#define OPENMW_OBJECTPROCESSSOR_HPP

#include <components/openmw-mp/MWMPLog.hpp>
#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/Packets/Object/ObjectPacket.hpp>
#include "../ObjectList.hpp"
#include "../LocalPlayer.hpp"
#include "../DedicatedPlayer.hpp"
#include "BaseClientPacketProcessor.hpp"

namespace mwmp
{
    class ObjectProcessor : public BasePacketProcessor<ObjectProcessor>, public BaseClientPacketProcessor
    {
    public:
        virtual void Do(ObjectPacket &packet, ObjectList &objectList) = 0;

        static bool Process(RakNet::Packet &packet, ObjectList &objectList);
    };
}


#endif //OPENMW_OBJECTPROCESSSOR_HPP
