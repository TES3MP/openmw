//
// Created by koncord on 25.08.17.
//

#pragma once

#include <RakNetTypes.h>
#include <tuple>
#include "Inventory.hpp"
#include "Cells.hpp"

namespace mwmp
{
    class BasePlayer;
    class BaseNetCreature;
    class BaseActor;
}

class NetActor
{
public:
    NetActor();

    void resetUpdateFlags();

    /**
     *
     * @return  x, y, z
     */

    std::tuple<float, float, float> getPosition() const;
    void setPosition(float x, float y, float z);

    /**
     *
     * @return  x, y
     */
    std::tuple<float, float> getRotation() const;
    void setRotation(float x, float z);

    /**
     *
     * @return base, current
     */
    std::tuple<float, float> getHealth() const;
    void setHealth(float base, float current);

    /**
     *
     * @return base, current
     */
    std::tuple<float, float> getMagicka() const;
    void setMagicka(float base, float current);

    /**
     *
     * @return base, current
     */
    std::tuple<float, float> getFatigue() const;
    void setFatigue(float base, float current);

    Inventory &getInventory();
    Cells &getCell();

    mwmp::BaseNetCreature *getNetCreature() { return netCreature; }
protected:
    bool statsChanged, attributesChanged, skillsChanged, baseInfoChanged, positionChanged;
    mwmp::BasePlayer *basePlayer;
    mwmp::BaseNetCreature *netCreature;

    Inventory inventory;
    Cells cellAPI;
};


