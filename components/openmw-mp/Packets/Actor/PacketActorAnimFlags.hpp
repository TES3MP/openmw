#ifndef OPENMW_PACKETACTORANIMFLAGS_HPP
#define OPENMW_PACKETACTORANIMFLAGS_HPP

#include <components/openmw-mp/Packets/Actor/ActorPacket.hpp>

namespace mwmp
{
    class PacketActorAnimFlags : public ActorPacket
    {
    public:
        PacketActorAnimFlags(RakNet::RakPeerInterface *peer);

        void Actor(BaseActor &actor, bool send) final;
    };
}

#endif //OPENMW_PACKETACTORANIMFLAGS_HPP
