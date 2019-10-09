#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/MWMPLog.hpp>
#include "PacketActorCellChange.hpp"

using namespace mwmp;

PacketActorCellChange::PacketActorCellChange(RakNet::RakPeerInterface *peer) : ActorPacket(peer)
{
    packetID = ID_ACTOR_CELL_CHANGE;
}

void PacketActorCellChange::Actor(BaseActor &actor, bool send)
{
    RW(actor.cell.mData, send, true);
    RW(actor.cell.mName, send, true);

    RW(actor.position, send, true);
    RW(actor.direction, send, true);

    RW(actor.isFollowerCellChange, send);
}
