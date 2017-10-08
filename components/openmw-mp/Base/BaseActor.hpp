#ifndef OPENMW_BASEACTOR_HPP
#define OPENMW_BASEACTOR_HPP

#include <components/esm/loadcell.hpp>

#include <components/openmw-mp/Base/BaseStructs.hpp>
#include <components/openmw-mp/Base/BaseNetCreature.hpp>

#include <RakNetTypes.h>
#include <memory>

namespace mwmp
{
    class BaseActor : public BaseNetCreature
    {
    public:

        BaseActor()
        {
            hasPositionData = false;
            hasStatsDynamicData = false;
            refNumIndex = 0;
            mpNum = 0;
            drawState = 0;
            isFlying = false;
        }

        std::string refId;
        int refNumIndex;
        int mpNum;

        std::string response;
        std::string sound;

        Animation animation;

        bool hasPositionData;
        bool hasStatsDynamicData;
    };

    class BaseActorList
    {
    public:

        BaseActorList()
        {
            cell.blank();
        }

        enum ACTOR_ACTION
        {
            SET = 0,
            ADD = 1,
            REMOVE = 2,
            REQUEST = 3
        };

        RakNet::RakNetGUID guid;

        std::vector<std::shared_ptr<BaseActor>> baseActors;

        unsigned int count;

        ESM::Cell cell;

        unsigned char action; // 0 - Clear and set in entirety, 1 - Add item, 2 - Remove item, 3 - Request items

        bool isValid;
    };
}

#endif //OPENMW_BASEACTOR_HPP
