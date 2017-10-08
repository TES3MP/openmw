//
// Created by koncord on 25.08.17.
//

#include "NetActor.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>
#include "Script/LuaState.hpp"
//#include "Player.hpp"
#include "Networking.hpp"
#include <components/openmw-mp/Base/BaseNetCreature.hpp>

using namespace std;

NetActor::NetActor() : inventory(this), cellAPI(this)
{

}

void NetActor::resetUpdateFlags()
{
    statsChanged = false;
    skillsChanged = false;
    attributesChanged = false;
    baseInfoChanged = false;
    positionChanged = false;
}

std::tuple<float, float, float> NetActor::getPosition() const
{
    return make_tuple(netCreature->position.pos[0], netCreature->position.pos[1], netCreature->position.pos[2]);
}

void NetActor::setPosition(float x, float y, float z)
{
    netCreature->position.pos[0] = x;
    netCreature->position.pos[1] = y;
    netCreature->position.pos[2] = z;
    positionChanged = true;
}

std::tuple<float, float> NetActor::getRotation() const
{
    return make_tuple(netCreature->position.rot[0], netCreature->position.rot[2]);
}

void NetActor::setRotation(float x, float z)
{
    netCreature->position.rot[0] = x;
    netCreature->position.rot[2] = z;
    positionChanged = true;
}

std::tuple<float, float> NetActor::getHealth() const
{
    return make_tuple(netCreature->creatureStats.mDynamic[0].mBase, netCreature->creatureStats.mDynamic[0].mCurrent);
}

void NetActor::setHealth(float base, float current)
{
    netCreature->creatureStats.mDynamic[0].mBase = base;
    netCreature->creatureStats.mDynamic[0].mCurrent = current;
    statsChanged = true;
}

std::tuple<float, float> NetActor::getMagicka() const
{
    return make_tuple(netCreature->creatureStats.mDynamic[1].mBase, netCreature->creatureStats.mDynamic[1].mCurrent);
}

void NetActor::setMagicka(float base, float current)
{
    netCreature->creatureStats.mDynamic[1].mBase = base;
    netCreature->creatureStats.mDynamic[1].mCurrent = current;
    statsChanged = true;
}

std::tuple<float, float> NetActor::getFatigue() const
{
    return make_tuple(netCreature->creatureStats.mDynamic[2].mBase, netCreature->creatureStats.mDynamic[2].mCurrent);
}

void NetActor::setFatigue(float base, float current)
{
    netCreature->creatureStats.mDynamic[2].mBase = base;
    netCreature->creatureStats.mDynamic[2].mCurrent = current;
    statsChanged = true;
}

Inventory &NetActor::getInventory()
{
    return inventory;
}

Cells &NetActor::getCell()
{
    return cellAPI;
}