#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketObjectTrap.hpp"

using namespace mwmp;

PacketObjectTrap::PacketObjectTrap(RakNet::RakPeerInterface *peer) : ObjectPacket(peer)
{
    packetID = ID_OBJECT_TRAP;
    hasCellData = true;
}

void PacketObjectTrap::Object(BaseObject &baseObject, bool send)
{
    ObjectPacket::Object(baseObject, send);
    RW(baseObject.trapSpellId, send, true);
    RW(baseObject.trapAction, send);

    if (baseObject.trapAction == mwmp::BaseObjectList::TRAP_ACTION::TRIGGER)
        RW(baseObject.trapTriggerPosition, send);
}
