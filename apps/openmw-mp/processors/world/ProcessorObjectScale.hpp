#ifndef OPENMW_PROCESSOROBJECTSCALE_HPP
#define OPENMW_PROCESSOROBJECTSCALE_HPP

#include "../WorldProcessor.hpp"

namespace mwmp
{
    class ProcessorObjectScale : public WorldProcessor
    {
    public:
        ProcessorObjectScale()
        {
            BPP_INIT(ID_OBJECT_SCALE)
        }

        void Do(WorldPacket &packet, std::shared_ptr<Player> player, BaseEvent &event) override
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Received %s from %s", strPacketID.c_str(), player->npc.mName.c_str());
            
            packet.Send(true);

            auto objCtrl = Networking::get().getState().getObjectCtrl();
            auto objects = objCtrl.copyObjects(event);

            Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_OBJECT_SCALE>(player, objects);

            objCtrl.sendObjects(player, objects, event.cell);
        }
    };
}

#endif //OPENMW_PROCESSOROBJECTSCALE_HPP
