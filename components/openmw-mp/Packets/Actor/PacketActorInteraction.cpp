#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/MWMPLog.hpp>
#include "PacketActorInteraction.hpp"

using namespace mwmp;

PacketActorInteraction::PacketActorInteraction(RakNet::RakPeerInterface *peer) : ActorPacket(peer)
{
    packetID = ID_ACTOR_INTERACTION;
}

void PacketActorInteraction::Actor(BaseActor &actor, bool send)
{
    // Placeholder
}
