#ifndef OPENMW_PROCESSORACTORTEST_HPP
#define OPENMW_PROCESSORACTORTEST_HPP

#include "../ActorProcessor.hpp"

namespace mwmp
{
    class ProcessorActorTest : public ActorProcessor
    {
    public:
        ProcessorActorTest()
        {
            BPP_INIT(ID_ACTOR_TEST)
        }

        void Do(ActorPacket &packet, std::shared_ptr<Player> player, BaseActorList &actorList) override
        {
            std::vector<std::shared_ptr<Actor>> actors;

            for (auto &baseActor : actorList.baseActors)
            {
                Actor *actor = new Actor;
                actor->actor = baseActor;
                actors.emplace_back(actor);
            }

            Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_ACTOR_TEST>(player, actors);

            Networking::get().getState().getActorCtrl().sendActors(player, actors, actorList.cell, true);
        }
    };
}

#endif //OPENMW_PROCESSORACTORTEST_HPP
