#ifndef OPENMW_PROCESSORACTORCELLCHANGE_HPP
#define OPENMW_PROCESSORACTORCELLCHANGE_HPP

#include "../ActorProcessor.hpp"

namespace mwmp
{
    class ProcessorActorCellChange : public ActorProcessor
    {
    public:
        ProcessorActorCellChange()
        {
            BPP_INIT(ID_ACTOR_CELL_CHANGE)
        }

        void Do(ActorPacket &packet, std::shared_ptr<Player> player, BaseActorList &actorList) override
        {
            Cell *serverCell = CellController::get()->getCell(&actorList.cell);

            if (serverCell != nullptr && *serverCell->getAuthority() == actorList.guid)
            {
                serverCell->removeActors(&actorList);

                std::vector<std::shared_ptr<Actor>> actors;

                for (auto &baseActor : actorList.baseActors)
                {
                    Actor *actor = new Actor;
                    actor->actor = baseActor;
                    actors.emplace_back(actor);
                }

                Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_ACTOR_CELL_CHANGE>(player, actors);

                Networking::get().getState().getActorCtrl().sendActors(player, actors, actorList.cell, true);

                // Send this to everyone
                packet.Send(true);
            }
        }
    };
}

#endif //OPENMW_PROCESSORACTORCELLCHANGE_HPP
