//
// Created by koncord on 01.08.17.
//

#pragma once

#include "sol.hpp"
#include "CommandController.hpp"
#include "Timer.hpp"
#include <unordered_map>
#include <memory>
#include <apps/openmw-mp/Actors.hpp>
#include <apps/openmw-mp/Object.hpp>

//class CommandController;
class EventController;
//class TimerController;

class LuaState
{
public:
    LuaState();
    ~LuaState() = default;
    std::shared_ptr<sol::state> getState() { return lua; }
    void addGlobalPackagePath(const std::string &path);
    void addGlobalCPath(const std::string &path);
    sol::table getCoreTable() { return dataEnv["Core"]; }
    sol::environment &getDataEnv(){ return dataEnv; }
    void loadMods();

    CommandController &getCmdCtrl();
    EventController &getEventCtrl();
    TimerController &getTimerCtrl();
    ObjectController &getObjectCtrl();

    ActorController &getActorCtrl();

private:
    sol::environment openScript(std::string path, std::string modname);
private:
    std::shared_ptr<sol::state> lua;
    sol::environment dataEnv;
    sol::environment configEnv;
    std::unique_ptr<CommandController> cmdCtrl;
    std::unique_ptr<EventController> eventCtrl;
    std::unique_ptr<TimerController> timerCtrl;
    std::unique_ptr<ActorController> actorCtrl;
    std::unique_ptr<ObjectController> objectCtrl;
    std::unordered_map<std::string, sol::environment> mods;
};
