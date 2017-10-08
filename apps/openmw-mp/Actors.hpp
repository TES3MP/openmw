//
// Created by koncord on 25.08.17.
//

#pragma once

#include <string>
#include <components/openmw-mp/Base/BaseActor.hpp>
#include "NetActor.hpp"

class LuaState;
class Player;

class Actor: public NetActor
{
    friend class ActorController;
public:
    static void Init(LuaState &lua);
public:
    Actor();
    std::string getRefId() const;
    void setRefId(const std::string &refId);
    
    int getRefNumIndex() const;
    void setRefNumIndex(int refNumIndex);
    int getMpNum() const;
    void setMpNum(int mpNum);
    bool doesHavePosition() const; // ????
    bool doesHaveStatsDynamic() const; // ????
    
    std::shared_ptr<mwmp::BaseActor> actor;
};

class ActorController
{
public:
    static void Init(LuaState &lua);
public:

    ActorController();
    ~ActorController();

    std::shared_ptr<Actor> createActor();
    void sendActors(std::shared_ptr<Player> player, std::vector<std::shared_ptr<Actor>> actors, const ESM::Cell &cell, bool sendToAll = false);
    void sendList(std::shared_ptr<Player> player, std::vector<std::shared_ptr<Actor>> actors, const ESM::Cell &cell, bool sendToAll = false);

    void requestList(std::shared_ptr<Player> player, const ESM::Cell &cell);

    std::vector<std::shared_ptr<Actor>> getActors(std::shared_ptr<Player> player, const ESM::Cell &cell);

private:
    mwmp::BaseActorList actorList;
};


