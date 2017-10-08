#ifndef OPENMW_PROCESSORACTORAI_HPP
#define OPENMW_PROCESSORACTORAI_HPP

#include "../ActorProcessor.hpp"

namespace mwmp
{
    class ProcessorActorAI : public ActorProcessor
    {
    public:
        ProcessorActorAI()
        {
            BPP_INIT(ID_ACTOR_AI)
        }

        void Do(ActorPacket &packet, std::shared_ptr<Player> player, BaseActorList &actorList) override
        {
            // Send only to players who have the cell loaded
            Cell *serverCell = CellController::get()->getCell(&actorList.cell);

            if (serverCell != nullptr && *serverCell->getAuthority() == actorList.guid)
                serverCell->sendToLoaded(&packet, &actorList);
        }
    };
}

#endif //OPENMW_PROCESSORACTORAI_HPP
