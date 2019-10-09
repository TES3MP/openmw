#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/MWMPLog.hpp>
#include "PacketActorAI.hpp"

using namespace mwmp;

PacketActorAI::PacketActorAI(RakNet::RakPeerInterface *peer) : ActorPacket(peer)
{
    packetID = ID_ACTOR_AI;
}

void PacketActorAI::Actor(BaseActor &actor, bool send)
{
    RW(actor.aiAction, send);

    if (actor.aiAction != mwmp::BaseActorList::CANCEL)
    {
        if (actor.aiAction == mwmp::BaseActorList::WANDER)
        {
            RW(actor.aiDistance, send);
            RW(actor.aiShouldRepeat, send);
        }

        if (actor.aiAction == mwmp::BaseActorList::ESCORT || actor.aiAction == mwmp::BaseActorList::WANDER)
            RW(actor.aiDuration, send);

        if (actor.aiAction == mwmp::BaseActorList::ESCORT || actor.aiAction == mwmp::BaseActorList::TRAVEL)
            RW(actor.aiCoordinates, send);

        if (actor.aiAction == mwmp::BaseActorList::ACTIVATE || actor.aiAction == mwmp::BaseActorList::COMBAT ||
            actor.aiAction == mwmp::BaseActorList::ESCORT || actor.aiAction == mwmp::BaseActorList::FOLLOW)
        {
            RW(actor.hasAiTarget, send);

            if (actor.hasAiTarget)
            {
                RW(actor.aiTarget.isPlayer, send);

                if (actor.aiTarget.isPlayer)
                {
                    RW(actor.aiTarget.guid, send);
                }
                else
                {
                    RW(actor.aiTarget.refId, send, true);
                    RW(actor.aiTarget.refNum, send);
                    RW(actor.aiTarget.mpNum, send);
                }
            }
        }
    }
}
