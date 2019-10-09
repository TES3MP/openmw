#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/MWMPLog.hpp>
#include "PacketActorEquipment.hpp"

using namespace mwmp;

PacketActorEquipment::PacketActorEquipment(RakNet::RakPeerInterface *peer) : ActorPacket(peer)
{
    packetID = ID_ACTOR_EQUIPMENT;
}

void PacketActorEquipment::Actor(BaseActor &actor, bool send)
{
    for (auto &&equipmentItem : actor.equipmentItems)
    {
        RW(equipmentItem.refId, send);
        RW(equipmentItem.count, send);
        RW(equipmentItem.charge, send);
        RW(equipmentItem.enchantmentCharge, send);
    }
}
