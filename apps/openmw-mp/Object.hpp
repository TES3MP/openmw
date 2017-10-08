//
// Created by koncord on 26.08.17.
//

#pragma once

#include <tuple>
#include <components/openmw-mp/Base/BaseEvent.hpp>
#include <memory>

class LuaState;
class Player;

class BaseObject
{
public:
    BaseObject();
    std::string getRefId() const;
    void setRefId(const std::string &refId);

    int getRefNum() const;
    void setRefNum(int refNum);

    int getMpNum() const;
    void setMpNum(int mpNum);

    //void setEventCell(const std::string &cellDescription);


    mwmp::WorldObject object;
    bool changedBase;
    bool copied;
};

class Object : public BaseObject
{
public:
    static void Init(LuaState &lua);
public:
    Object();
    ~Object();

    void update();

    /**
     *
     * @return  x, y, z
     */

    std::tuple<float, float, float> getPosition() const;
    void setPosition(float x, float y, float z);

    /**
     *
     * @return  x, y, z
     */
    std::tuple<float, float, float> getRotation() const;
    void setRotation(float x, float y, float z);

    int getCount() const;
    void setCount(int count);

    int getCharge() const;
    void setCharge(int charge);

    int getGoldValue() const;
    void setGoldValue(int gold);

    float getScale() const;
    void setScale(float scale);

    bool getState() const;
    void setState(bool state);

    int getDoorState() const;
    void setDoorState(int state);

    int getLockLevel() const;
    void setLockLevel(int locklevel);

    void setDisarmState(bool state);
    void setMasterState(bool state);

    bool changedDoorState, changedObjectState, changedObjectScale, changedObjectTrap, changedObjectLock,
            changedObjectDelete, changedObjectSpawn, changedObjectPlace;
};

class Container : public BaseObject
{
public:
    static void Init(LuaState &lua);
public:
    Container();

    std::tuple<std::string, int, int> getItem(int i) const;
    void addItem(const std::string &refId, int count, int charge);

    void setItem(int i, const std::string &refId, int count, int charge);
    int getActionCount(int i) const;

    size_t size() const;
    bool changed;
};


class ObjectController
{
public:
    static void Init(LuaState &lua);
public:

    std::shared_ptr<std::vector<std::shared_ptr<Object>>> copyObjects(mwmp::BaseEvent &event);
    std::shared_ptr<std::vector<std::shared_ptr<Container>>> copyContainers(mwmp::BaseEvent &event);

    void sendObjects(std::shared_ptr<Player> player, std::shared_ptr<std::vector<std::shared_ptr<Object>>> objects, const ESM::Cell &cell);
    void sendContainers(std::shared_ptr<Player> player, std::shared_ptr<std::vector<std::shared_ptr<Container>>> objects, const ESM::Cell &cell);

    void requestContainers(std::shared_ptr<Player> player);
};