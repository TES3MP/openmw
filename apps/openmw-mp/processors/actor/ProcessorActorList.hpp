#ifndef OPENMW_PROCESSORACTORLIST_HPP
#define OPENMW_PROCESSORACTORLIST_HPP

#include "../ActorProcessor.hpp"

namespace mwmp
{
    class ProcessorActorList : public ActorProcessor
    {
    public:
        ProcessorActorList()
        {
            BPP_INIT(ID_ACTOR_LIST)
        }

        void Do(ActorPacket &packet, std::shared_ptr<Player> player, BaseActorList &actorList) override
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Received %s from %s", strPacketID.c_str(), player->npc.mName.c_str());

            std::vector<std::shared_ptr<Actor>> actors;

            for (auto &baseActor : actorList.baseActors)
            {
                Actor *actor = new Actor;
                actor->actor = baseActor;
                actors.emplace_back(actor);
            }

            Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_ACTOR_LIST>(player, actors);

            Networking::get().getState().getActorCtrl().sendActors(player, actors, actorList.cell, true);
        }
    };
}

#endif //OPENMW_PROCESSORACTORLIST_HPP
