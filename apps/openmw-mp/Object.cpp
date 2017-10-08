//
// Created by koncord on 26.08.17.
//

#include <components/openmw-mp/NetworkMessages.hpp>
#include "Object.hpp"
#include "Player.hpp"
#include "Networking.hpp"

using namespace std;

void Object::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Object>("Object",
                                         "refId", sol::property(&BaseObject::getRefId, &BaseObject::setRefId),
                                         "refNum", sol::property(&BaseObject::getRefNum, &BaseObject::setRefNum),
                                         "mpNum", sol::property(&BaseObject::getMpNum, &BaseObject::setMpNum),
                                         "setPosition", &Object::setPosition,
                                         "getPosition", &Object::getPosition,
                                         "setRotation", &Object::setRotation,
                                         "getRotation", &Object::getRotation,
                                         "count", sol::property(&Object::getCount, &Object::setCount),
                                         "goldValue", sol::property(&Object::getGoldValue, &Object::setGoldValue),
                                         "scale", sol::property(&Object::getScale, &Object::setScale),
                                         "state", sol::property(&Object::getState, &Object::setState),
                                         "doorState", sol::property(&Object::getDoorState, &Object::setDoorState),
                                         "lockLevel", sol::property(&Object::getLockLevel, &Object::setLockLevel),
                                         "setDisarmState", &Object::setDisarmState,
                                         "setMasterState", &Object::setMasterState
    );
}

Object::Object()
{

}

Object::~Object()
{

}

void Object::update()
{

}

tuple<float, float, float> Object::getPosition() const
{
    return make_tuple(object.position.pos[0], object.position.pos[1], object.position.pos[2]);
}

void Object::setPosition(float x, float y, float z)
{
    object.position.pos[0] = x;
    object.position.pos[1] = y;
    object.position.pos[2] = z;
    changedObjectPlace = true;
}

tuple<float, float, float> Object::getRotation() const
{
    return make_tuple(object.position.rot[0], object.position.rot[1], object.position.rot[2]);
}

void Object::setRotation(float x, float y, float z)
{
    object.position.rot[0] = x;
    object.position.rot[1] = y;
    object.position.rot[2] = z;
    changedObjectPlace = true;
}

BaseObject::BaseObject(): changedBase(false), copied(false)
{

}

string BaseObject::getRefId() const
{
    return object.refId;
}

void BaseObject::setRefId(const string &refId)
{
    changedBase = true;
    object.refId = refId;
}

int BaseObject::getRefNum() const
{
    return object.refNumIndex;
}

void BaseObject::setRefNum(int refNum)
{
    changedBase = true;
    object.refNumIndex = refNum;
}

int BaseObject::getMpNum() const
{
    return object.mpNum;
}

void BaseObject::setMpNum(int mpNum)
{
    changedBase = true;
    object.mpNum = mpNum;
}

int Object::getCount() const
{
    return object.count;
}

void Object::setCount(int count)
{
    changedObjectPlace = true;
    object.count = count;
}

int Object::getCharge() const
{
    return object.charge;
}

void Object::setCharge(int charge)
{
    changedObjectPlace = true;
    object.charge = charge;

}

int Object::getGoldValue() const
{
    return object.goldValue;
}

void Object::setGoldValue(int gold)
{
    changedObjectPlace = true;
    object.goldValue = gold;
}

float Object::getScale() const
{
    return object.scale;
}

void Object::setScale(float scale)
{
    changedObjectScale = true;
    object.scale = scale;
}

bool Object::getState() const
{
    return object.objectState;
}

void Object::setState(bool state)
{
    changedObjectState = true;
    object.objectState = state;
}

int Object::getDoorState() const
{
    return object.doorState;
}

void Object::setDoorState(int state)
{
    changedDoorState = true;
    object.doorState = state;
}

int Object::getLockLevel() const
{
    return object.lockLevel;
}

void Object::setLockLevel(int locklevel)
{
    changedObjectLock = true;
    object.lockLevel = locklevel;
}

void Object::setDisarmState(bool state)
{
    changedObjectTrap = true;
    object.isDisarmed = state;
}

void Object::setMasterState(bool state)
{
    changedObjectSpawn = true;
    object.hasMaster = state;
}


void Container::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Container>("Container",
                                            "refId", sol::property(&BaseObject::getRefId, &BaseObject::setRefId),
                                            "refNum", sol::property(&BaseObject::getRefNum, &BaseObject::setRefNum),
                                            "mpNum", sol::property(&BaseObject::getMpNum, &BaseObject::setMpNum),
                                            "getItem", &Container::getItem,
                                            "addItem", &Container::addItem,
                                            "setItem", &Container::setItem,
                                            "getActionCount", &Container::getActionCount
    );
}

Container::Container()
{

}

tuple<string, int, int> Container::getItem(int i) const
{
    auto &item = object.containerItems.at(i);
    return make_tuple(item.refId, item.count, item.charge);
}

void Container::setItem(int i, const string &refId, int count, int charge)
{
    auto &item = object.containerItems.at(i);
    item.refId = refId;
    item.count = count;
    item.charge = charge;
    changed = true;
}

void Container::addItem(const string &refId, int count, int charge)
{
    mwmp::ContainerItem item;
    item.refId = refId;
    item.count = count;
    item.charge = charge;
    object.containerItems.push_back(item);
    changed = true;
}

int Container::getActionCount(int i) const
{
    return object.containerItems.at(i).actionCount;
}

size_t Container::size() const
{
    return object.containerItems.size();
}

void ObjectController::Init(LuaState &lua)
{
    sol::table objectCtrl = lua.getState()->create_table("ObjectCtrl");

    objectCtrl.set_function("sendObjects", [&lua](shared_ptr<Player> player, shared_ptr<vector<shared_ptr<Object>>> objects,
                                                  const std::string &cellDescription) {
        return lua.getObjectCtrl().sendObjects(player, objects, Utils::getCellFromDescription(cellDescription));
    });

    objectCtrl.set_function("sendContainers", [&lua](shared_ptr<Player> player, shared_ptr<vector<shared_ptr<Container>>> objects,
                                                     const std::string &cellDescription) {
        return lua.getObjectCtrl().sendContainers(player, objects, Utils::getCellFromDescription(cellDescription));
    });

    objectCtrl.set_function("requestContainers", [&lua](shared_ptr<Player> player) {
        lua.getObjectCtrl().requestContainers(player);
    });


}

shared_ptr<vector<shared_ptr<Object>>> ObjectController::copyObjects(mwmp::BaseEvent &event)
{
    auto objects = make_shared<vector<shared_ptr<Object>>>();

    for(auto &obj : event.worldObjects)
    {
        auto object = new Object;
        object->copied = true;
        object->object = obj;
        objects->emplace_back(object);
    }
    return objects;
}

shared_ptr<vector<shared_ptr<Container>>> ObjectController::copyContainers(mwmp::BaseEvent &event)
{
    auto containers = make_shared<vector<shared_ptr<Container>>>();

    for(auto &obj : event.worldObjects)
    {
        auto container = new Container;
        container->copied = true;
        container->object = obj;
        containers->emplace_back(container);
    }
    return containers;
}

void ObjectController::sendObjects(shared_ptr<Player> player, shared_ptr<vector<shared_ptr<Object>>> objects, const ESM::Cell &cell)
{
    enum Type
    {
        DOOR_STATE = 0,
        OBJECT_STATE,
        OBJECT_SCALE,
        OBJECT_TRAP,
        OBJECT_LOCK,
        OBJECT_DELETE,
        OBJECT_SPAWN,
        OBJECT_PLACE,
        LAST
    };
    mwmp::BaseEvent events[Type::LAST];
    bool changed[Type::LAST];

    for(auto &e : events)
    {
        e.action = mwmp::BaseEvent::SET;
        e.guid = player->guid;
        e.cell = cell;
    }


    for(auto &object : *objects)
    {
        //sendObject(player.get(), object.get());

        bool validNewObjOrCopy = (!object->copied && object->changedBase) || object->copied;

        if(object->changedDoorState && validNewObjOrCopy)
        {
            changed[Type::DOOR_STATE] = true;
            events[Type::DOOR_STATE].worldObjects.push_back(object->object);
        }
        if(object->changedObjectState && validNewObjOrCopy)
        {
            changed[Type::OBJECT_STATE] = true;
            events[Type::OBJECT_STATE].worldObjects.push_back(object->object);
        }
        if(object->changedObjectScale && validNewObjOrCopy)
        {
            changed[Type::OBJECT_SCALE] = true;
            events[Type::OBJECT_SCALE].worldObjects.push_back(object->object);
        }
        if(object->changedObjectTrap && validNewObjOrCopy)
        {
            changed[Type::OBJECT_TRAP] = true;
            events[Type::OBJECT_TRAP].worldObjects.push_back(object->object);
        }
        if(object->changedObjectLock && validNewObjOrCopy)
        {
            changed[Type::OBJECT_LOCK] = true;
            events[Type::OBJECT_LOCK].worldObjects.push_back(object->object);
        }
        if(object->changedObjectDelete && validNewObjOrCopy)
        {
            changed[Type::OBJECT_DELETE] = true;
            events[Type::OBJECT_DELETE].worldObjects.push_back(object->object);
        }
        if(object->changedObjectSpawn && validNewObjOrCopy)
        {
            changed[Type::OBJECT_SPAWN] = true;
            events[Type::OBJECT_SPAWN].worldObjects.push_back(object->object);
        }
        if(object->changedObjectPlace && validNewObjOrCopy)
        {
            changed[Type::OBJECT_PLACE] = true;
            events[Type::OBJECT_PLACE].worldObjects.push_back(object->object);
        }
    }

    auto worldCtrl = mwmp::Networking::get().getWorldPacketController();

    if(changed[Type::DOOR_STATE])
    {
        auto packet = worldCtrl->GetPacket(ID_DOOR_STATE);
        auto &event = events[Type::DOOR_STATE];
        packet->setEvent(&event);
        packet->Send(event.guid);
    }
    if(changed[Type::OBJECT_STATE])
    {
        auto packet = worldCtrl->GetPacket(ID_OBJECT_STATE);
        auto &event = events[Type::OBJECT_STATE];
        packet->setEvent(&event);
        packet->Send(event.guid);
    }
    if(changed[Type::OBJECT_SCALE])
    {
        auto packet = worldCtrl->GetPacket(ID_OBJECT_SCALE);
        auto &event = events[Type::OBJECT_SCALE];
        packet->setEvent(&event);
        packet->Send(event.guid);
    }
    if(changed[Type::OBJECT_TRAP])
    {
        auto packet = worldCtrl->GetPacket(ID_OBJECT_TRAP);
        auto &event = events[Type::OBJECT_TRAP];
        packet->setEvent(&event);
        packet->Send(event.guid);
    }
    if(changed[Type::OBJECT_LOCK])
    {
        auto packet = worldCtrl->GetPacket(ID_OBJECT_LOCK);
        auto &event = events[Type::OBJECT_LOCK];
        packet->setEvent(&event);
        packet->Send(event.guid);
    }
    if(changed[Type::OBJECT_DELETE])
    {
        auto packet = worldCtrl->GetPacket(ID_OBJECT_DELETE);
        auto &event = events[Type::OBJECT_DELETE];
        packet->setEvent(&event);
        packet->Send(event.guid);
    }
    if(changed[Type::OBJECT_SCALE])
    {
        auto packet = worldCtrl->GetPacket(ID_OBJECT_SPAWN);
        auto &event = events[Type::OBJECT_SCALE];
        packet->setEvent(&event);
        packet->Send(event.guid);
    }
    if(changed[Type::OBJECT_PLACE])
    {
        auto packet = worldCtrl->GetPacket(ID_OBJECT_PLACE);
        auto &event = events[Type::OBJECT_PLACE];
        packet->setEvent(&event);
        packet->Send(event.guid);
    }
}

void ObjectController::sendContainers(shared_ptr<Player> player, shared_ptr<vector<shared_ptr<Container>>> objects, const ESM::Cell &cell)
{

    mwmp::BaseEvent event;
    event.cell = cell;
    event.action = mwmp::BaseEvent::SET;
    event.guid = player->guid;

    for(auto &object : *objects)
    {
        bool validNewObjOrCopy = (!object->copied && object->changedBase) || object->copied;
        if(object->changed && validNewObjOrCopy)
            event.worldObjects.push_back(object->object);
    }

    auto packet = mwmp::Networking::get().getWorldPacketController()->GetPacket(ID_CONTAINER);
    packet->setEvent(&event);
    packet->Send(event.guid);
}

void ObjectController::requestContainers(shared_ptr<Player> player)
{
    mwmp::BaseEvent event;
    event.action = mwmp::BaseEvent::REQUEST;
    event.guid = player->guid;
    event.cell = player->cell;

    auto packet = mwmp::Networking::get().getWorldPacketController()->GetPacket(ID_CONTAINER);
    packet->setEvent(&event);
    packet->Send(event.guid);
}
