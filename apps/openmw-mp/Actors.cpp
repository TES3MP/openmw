//
// Created by koncord on 25.08.17.
//

#include <components/openmw-mp/NetworkMessages.hpp>
#include "Actors.hpp"
#include "Script/LuaState.hpp"
#include "Networking.hpp"
#include "Cell.hpp"
#include "CellController.hpp"
#include "Player.hpp"

using namespace std;

void Actor::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Actor>("Actor",
                                        "getPosition", &NetActor::getPosition,
                                        "setPosition", &NetActor::setPosition,
                                        "getRotation", &NetActor::getRotation,
                                        "setRotation", &NetActor::setRotation,

                                        "getHealth", &NetActor::getHealth,
                                        "setHealth", &NetActor::setHealth,
                                        "getMagicka", &NetActor::getMagicka,
                                        "setMagicka", &NetActor::setMagicka,
                                        "getFatigue", &NetActor::getFatigue,
                                        "setFatigue", &NetActor::setFatigue,

                                        "getCell", &NetActor::getCell,
                                        "getInventory", &NetActor::getInventory,

                                        "refId", sol::property(&Actor::getRefId, &Actor::setRefId),
                                        "refNumIndex", sol::property(&Actor::getRefNumIndex, &Actor::setRefNumIndex),
                                        "mpNum", sol::property(&Actor::getMpNum, &Actor::setMpNum)

    );
}

Actor::Actor() : NetActor()
{

}

std::string Actor::getRefId() const
{
    return actor->refId;
}

void Actor::setRefId(const std::string &refId)
{
    actor->refId = refId;
}

int Actor::getRefNumIndex() const
{
    return actor->refNumIndex;
}

void Actor::setRefNumIndex(int refNumIndex)
{
    actor->refNumIndex = refNumIndex;
}

int Actor::getMpNum() const
{
    return actor->mpNum;
}

void Actor::setMpNum(int mpNum)
{
    actor->mpNum = mpNum;
}

bool Actor::doesHavePosition() const
{
    return actor->hasPositionData;
}

bool Actor::doesHaveStatsDynamic() const
{
    return actor->hasStatsDynamicData;
}

void ActorController::Init(LuaState &lua)
{
    sol::table playersTable = lua.getState()->create_named_table("Actors");

    playersTable.set_function("createActor", [&lua](){
        return lua.getActorCtrl().createActor();
    });

    playersTable.set_function("sendActors", [&lua](shared_ptr<Player> player, vector<shared_ptr<Actor>> actors,
                                                   const std::string &cellDescription, bool sendToAll) {
        lua.getActorCtrl().sendActors(player, actors, Utils::getCellFromDescription(cellDescription), sendToAll);
    });

    playersTable.set_function("sendList", [&lua](shared_ptr<Player> player, vector<shared_ptr<Actor>> actors,
                                                 const std::string &cellDescription, bool sendToAll) {
        lua.getActorCtrl().sendList(player, actors, Utils::getCellFromDescription(cellDescription), sendToAll);
    });

    playersTable.set_function("requestList", [&lua](shared_ptr<Player> player, const std::string &cellDescription){
        lua.getActorCtrl().requestList(player, Utils::getCellFromDescription(cellDescription));
    });

    playersTable.set_function("getActors", [&lua](shared_ptr<Player> player, const std::string &cellDescription){
        lua.getActorCtrl().getActors(player, Utils::getCellFromDescription(cellDescription));
    });
}

ActorController::ActorController()
{

}

ActorController::~ActorController()
{

}

std::shared_ptr<Actor> ActorController::createActor()
{
    Actor *actor = new Actor();
    actor->actor.reset(new mwmp::BaseActor);

    return shared_ptr<Actor>(actor);
}

void ActorController::sendActors(std::shared_ptr<Player> player, std::vector<std::shared_ptr<Actor>> actors,
                                 const ESM::Cell &cell, bool sendToAll)
{
    actorList.cell = cell;
    actorList.guid = player->guid;

    bool positionChanged = false;
    bool statsChanged = false;
    /*bool attributesChanged = false;
    bool skillsChanged = false;
    bool baseInfoChanged = false;*/
    bool equipmentChanged = false;
    bool changedCell = false;

    actorList.baseActors.clear();
    for (auto &actor : actors)
    {
        actorList.baseActors.push_back(actor->actor);

        if (actor->positionChanged)
            positionChanged = true;
        if (actor->statsChanged)
            statsChanged = true;
        /*if (actor->attributesChanged)
            attributesChanged = true;
        if (actor->skillsChanged)
            skillsChanged = true;
        if (actor->baseInfoChanged)
            baseInfoChanged = true;*/
        if (actor->inventory.isEquipmentChanged())
        {
            equipmentChanged = true;
            actor->inventory.resetEquipmentFlag();
        }
        if (actor->cellAPI.isChangedCell())
        {
            changedCell = true;
            actor->cellAPI.resetChangedCell();
        }
        actor->resetUpdateFlags();
    }

    auto actorCtrl = mwmp::Networking::get().getActorPacketController();
    Cell *serverCell = nullptr;

    if (sendToAll)
        serverCell = CellController::get()->getCell(&actorList.cell);

    if (positionChanged)
    {
        auto packet = actorCtrl->GetPacket(ID_ACTOR_POSITION);

        packet->setActorList(&actorList);
        packet->Send(actorList.guid);

        if (sendToAll)
            serverCell->sendToLoaded(packet, &actorList);
    }
    if (statsChanged)
    {
        auto packet = actorCtrl->GetPacket(ID_ACTOR_STATS_DYNAMIC);

        packet->setActorList(&actorList);
        packet->Send(actorList.guid);

        if (sendToAll)
            serverCell->sendToLoaded(packet, &actorList);

    }
    /*if (attributesChanged)
    {
        auto packet = actorCtrl->GetPacket(ID_ACTOR_POSITION);

    }
    if (skillsChanged)
    {
        auto packet = actorCtrl->GetPacket(ID_ACTOR_POSITION);

    }
    if (baseInfoChanged)
    {
        auto packet = actorCtrl->GetPacket(ID_ACTOR_POSITION);

    }*/

    if (equipmentChanged)
    {
        auto packet = actorCtrl->GetPacket(ID_ACTOR_EQUIPMENT);
        packet->setActorList(&actorList);
        packet->Send(actorList.guid);

        if (sendToAll)
            serverCell->sendToLoaded(packet, &actorList);
    }
    if (changedCell)
    {
        auto packet = actorCtrl->GetPacket(ID_ACTOR_CELL_CHANGE);
        packet->setActorList(&actorList);
        packet->Send(actorList.guid);

        if (sendToAll)
            serverCell->sendToLoaded(packet, &actorList);
    }
}

void ActorController::sendList(std::shared_ptr<Player> player, std::vector<std::shared_ptr<Actor>> actors,
                               const ESM::Cell &cell, bool sendToAll)
{
    actorList.cell = player->cell;
    actorList.guid = player->guid;
    actorList.action = mwmp::BaseActorList::SET;

    for(auto &actor : actors)
    {
        actorList.baseActors.push_back(actor->actor);
    }

    auto packet = mwmp::Networking::get().getActorPacketController()->GetPacket(ID_ACTOR_LIST);
    packet->setActorList(&actorList);
    packet->Send(actorList.guid);
    if (sendToAll)
        CellController::get()->getCell(&actorList.cell)->sendToLoaded(packet, &actorList);
}

void ActorController::requestList(std::shared_ptr<Player> player, const ESM::Cell &cell)
{
    actorList.cell = player->cell;
    actorList.guid = player->guid;
    actorList.action = mwmp::BaseActorList::REQUEST;

    auto packet = mwmp::Networking::get().getActorPacketController()->GetPacket(ID_ACTOR_LIST);
    packet->setActorList(&actorList);
    packet->Send(actorList.guid);
}

std::vector<std::shared_ptr<Actor>> ActorController::getActors(std::shared_ptr<Player> player, const ESM::Cell &cell)
{
    Cell *serverCell = CellController::get()->getCell(&player->cell);

    std::vector<std::shared_ptr<Actor>> actorList;

    for (auto actor : serverCell->getActorList()->baseActors)
    {
        Actor *a = new Actor;
        a->actor = actor;
        actorList.emplace_back(a);
    }

    return actorList;
}
