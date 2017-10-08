#ifndef OPENMW_PROCESSORCONTAINER_HPP
#define OPENMW_PROCESSORCONTAINER_HPP

#include <apps/openmw-mp/Object.hpp>
#include "../WorldProcessor.hpp"


namespace mwmp
{
    class ProcessorContainer : public WorldProcessor
    {
    public:
        ProcessorContainer()
        {
            BPP_INIT(ID_CONTAINER)
        }

        void Do(WorldPacket &packet, std::shared_ptr<Player> player, BaseEvent &event) override
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Received %s from %s", strPacketID.c_str(), player->npc.mName.c_str());
            LOG_APPEND(Log::LOG_INFO, "- action: %i", event.action);

            // Until we have a timestamp-based system, send packets pertaining to more
            // than one container (i.e. replies to server requests for container contents)
            // only to players who have the container's cell loaded
            if (event.action == BaseEvent::SET && event.worldObjectCount > 1)
            {
                Cell *serverCell = CellController::get()->getCell(&event.cell);

                if (serverCell != nullptr)
                    serverCell->sendToLoaded(&packet, &event);
            }
            else
                packet.Send(true);

            auto objCtrl = Networking::get().getState().getObjectCtrl();
            auto containers = objCtrl.copyContainers(event);

            Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_CONTAINER>(player, containers);

            Networking::get().getState().getObjectCtrl().sendContainers(player, containers, event.cell);

            LOG_APPEND(Log::LOG_INFO, "- Finished processing ID_CONTAINER");
        }
    };
}

#endif //OPENMW_PROCESSORCONTAINER_HPP
