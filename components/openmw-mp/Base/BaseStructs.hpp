#ifndef OPENMW_BASESTRUCTS_HPP
#define OPENMW_BASESTRUCTS_HPP

#include <components/esm/statstate.hpp>

#include <RakNetTypes.h>

namespace mwmp
{
    struct Item
    {
        std::string refId;
        int count;
        int charge;

        inline bool operator==(const Item& rhs)
        {
            return refId == rhs.refId && count == rhs.count && charge == rhs.charge;
        }
    };

    struct InventoryChanges
    {
        std::vector<Item> items;
        unsigned int count;
        enum ACTION_TYPE
        {
            SET = 0,
            ADD,
            REMOVE
        };
        int action; // 0 - Clear and set in entirety, 1 - Add item, 2 - Remove item
    };
    
    struct Target
    {
        std::string refId;
        int refNumIndex;
        int mpNum;

        RakNet::RakNetGUID guid;
    };

    class Attack
    {
    public:

        Target target;

        char type; // 0 - melee, 1 - magic, 2 - throwable
        enum TYPE
        {
            MELEE = 0,
            MAGIC,
            THROWABLE
        };

        std::string spellId; // id of spell (e.g. "fireball")

        float damage;

        bool success;
        bool block;
        
        bool pressed;
        bool instant;
        bool knockdown;

        bool shouldSend;
    };

    struct Animation
    {
        std::string groupname;
        int mode;
        int count;
        bool persist;
    };
}

#endif //OPENMW_BASESTRUCTS_HPP
