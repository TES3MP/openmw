#ifndef OPENMW_ACTORLIST_HPP
#define OPENMW_ACTORLIST_HPP

#include <components/openmw-mp/Base/BaseActor.hpp>
#include "../mwworld/cellstore.hpp"
#include <RakNetTypes.h>

#include "LocalActor.hpp"

namespace mwmp
{
    class Networking;
    class ActorList : public BaseActorList
    {
    public:

        ActorList();
        virtual ~ActorList();

        void reset();
        void addActor(BaseActor *baseActor);
        void addActor(LocalActor *localActor);

        void addPositionActor(LocalActor *localActor);
        void addAnimFlagsActor(LocalActor *localActor);
        void addAnimPlayActor(LocalActor *localActor);
        void addSpeechActor(LocalActor *localActor);
        void addStatsDynamicActor(LocalActor *localActor);
        void addEquipmentActor(LocalActor *localActor);
        void addAttackActor(LocalActor *localActor);
        void addCellChangeActor(LocalActor *localActor);

        void sendPositionActors();
        void sendAnimFlagsActors();
        void sendAnimPlayActors();
        void sendSpeechActors();
        void sendStatsDynamicActors();
        void sendEquipmentActors();
        void sendAttackActors();
        void sendCellChangeActors();

        void sendActorsInCell(MWWorld::CellStore* cellStore);

    private:
        Networking *getNetworking();

        std::vector<std::shared_ptr<BaseActor>> positionActors;
        std::vector<std::shared_ptr<BaseActor>> animFlagsActors;
        std::vector<std::shared_ptr<BaseActor>> animPlayActors;
        std::vector<std::shared_ptr<BaseActor>> speechActors;
        std::vector<std::shared_ptr<BaseActor>> statsDynamicActors;
        std::vector<std::shared_ptr<BaseActor>> equipmentActors;
        std::vector<std::shared_ptr<BaseActor>> attackActors;
        std::vector<std::shared_ptr<BaseActor>> cellChangeActors;
    };
}

#endif //OPENMW_ACTORLIST_HPP
