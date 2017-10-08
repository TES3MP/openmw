#ifndef OPENMW_PROCESSORACTOREQUIPMENT_HPP
#define OPENMW_PROCESSORACTOREQUIPMENT_HPP

#include <apps/openmw-mp/Actors.hpp>
#include "../ActorProcessor.hpp"

namespace mwmp
{
    class ProcessorActorEquipment : public ActorProcessor
    {
    public:
        ProcessorActorEquipment()
        {
            BPP_INIT(ID_ACTOR_EQUIPMENT)
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

            Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_ACTOR_EQUIPMENT>(player, actors);

            Networking::get().getState().getActorCtrl().sendActors(player, actors, actorList.cell, true);
        }
    };
}

#endif //OPENMW_PROCESSORACTOREQUIPMENT_HPP
