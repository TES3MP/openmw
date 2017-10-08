#ifndef OPENMW_PACKETACTORLIST_HPP
#define OPENMW_PACKETACTORLIST_HPP

#include <components/openmw-mp/Packets/Actor/ActorPacket.hpp>

namespace mwmp
{
    class PacketActorList : public ActorPacket
    {
    public:
        PacketActorList(RakNet::RakPeerInterface *peer);

        void Packet(RakNet::BitStream *bs, bool send) final;
    };
}

#endif //OPENMW_PACKETACTORLIST_HPP
