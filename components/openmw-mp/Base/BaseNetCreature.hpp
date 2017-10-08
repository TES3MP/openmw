//
// Created by koncord on 25.08.17.
//

#ifndef OPENMW_BASENETCREATURE_HPP
#define OPENMW_BASENETCREATURE_HPP

#include <components/esm/loadcell.hpp>

#include <components/esm/creaturestats.hpp>

#include <components/openmw-mp/Base/BaseStructs.hpp>

#include <RakNetTypes.h>

namespace mwmp
{
    class BaseNetCreature
    {
    public:
        ESM::Position position;
        ESM::Position direction;

        ESM::CreatureStats creatureStats;
        ESM::Cell cell;
        bool isChangingRegion;

        Attack attack;
        Item equipedItems[19];
        InventoryChanges inventoryChanges;

        unsigned int movementFlags;

        char drawState;
        bool isFlying;

        inline void copyCreatureStats(ESM::CreatureStats &stats)
        {
            for(int i = 0; i < 3; i++)
                creatureStats.mDynamic[i] = stats.mDynamic[i];

            for(int i = 0; i < 8; i++)
                creatureStats.mAttributes[i] = stats.mAttributes[i];

            creatureStats.mLevel = stats.mLevel;
            creatureStats.mDead = stats.mDead;
        }

    };
}

#endif //OPENMW_BASENETCREATURE_HPP
