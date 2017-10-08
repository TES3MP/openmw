//
// Created by koncord on 30.07.17.
//

#include <iostream>
#include "EventController.hpp"

using namespace std;
#define ADD_CORE_EVENT(event) #event, CoreEvent::event
void EventController::Init(LuaState &lua)
{
    sol::table eventsTable = lua.getState()->create_named_table("Event");

    eventsTable["register"] = [&lua](int event, sol::function func, sol::this_environment te) {
        sol::environment& env = te;
        string modname = env["ModInfo"]["name"];
        lua.getEventCtrl().registerEvent(event, env, func);
    };
    eventsTable["stop"] = [&lua](int event) {
        lua.getEventCtrl().stop(event);
    };
    eventsTable["create"] = [&lua]() {
        return lua.getEventCtrl().createEvent();
    };
    eventsTable["raise"] = [&lua](unsigned event, sol::table data) {
        lua.getEventCtrl().raiseEvent(event, data);
    };
    eventsTable["raiseSpecified"] = [&lua](unsigned event, const std::string &modname, sol::table data) {
        lua.getEventCtrl().raiseEvent(event, data, modname);
    };
}

EventController::EventController(LuaState *luaCtrl)
{
    this->luaCtrl = luaCtrl;

#ifdef SERVER_DEBUG
    luaCtrl->getState()->new_enum<false>
#else
    luaCtrl->getState()->new_enum
#endif
            ("Events",
             ADD_CORE_EVENT(ON_POST_INIT),
             ADD_CORE_EVENT(ON_EXIT),
             ADD_CORE_EVENT(ON_PLAYER_CONNECT),
             ADD_CORE_EVENT(ON_PLAYER_DISCONNECT),
             ADD_CORE_EVENT(ON_PLAYER_DEATH),
             ADD_CORE_EVENT(ON_PLAYER_RESURRECT),
             ADD_CORE_EVENT(ON_PLAYER_CELLCHANGE),
             ADD_CORE_EVENT(ON_PLAYER_KILLCOUNT),
             ADD_CORE_EVENT(ON_PLAYER_ATTRIBUTE),
             ADD_CORE_EVENT(ON_PLAYER_SKILL),
             ADD_CORE_EVENT(ON_PLAYER_LEVEL),
             ADD_CORE_EVENT(ON_PLAYER_BOUNTY),
             ADD_CORE_EVENT(ON_PLAYER_EQUIPMENT),
             ADD_CORE_EVENT(ON_PLAYER_INVENTORY),
             ADD_CORE_EVENT(ON_PLAYER_JOURNAL),
             ADD_CORE_EVENT(ON_PLAYER_FACTION),
             ADD_CORE_EVENT(ON_PLAYER_SHAPESHIFT),
             ADD_CORE_EVENT(ON_PLAYER_SPELLBOOK),
             ADD_CORE_EVENT(ON_PLAYER_TOPIC),
             ADD_CORE_EVENT(ON_PLAYER_DISPOSITION),
             ADD_CORE_EVENT(ON_PLAYER_BOOK),
             ADD_CORE_EVENT(ON_PLAYER_MAP),
             ADD_CORE_EVENT(ON_PLAYER_REST),
             ADD_CORE_EVENT(ON_PLAYER_SENDMESSAGE),
             ADD_CORE_EVENT(ON_PLAYER_ENDCHARGEN),
             ADD_CORE_EVENT(ON_GUI_ACTION),
             ADD_CORE_EVENT(ON_REQUEST_PLUGIN_LIST),
             ADD_CORE_EVENT(ON_MP_REFNUM),
             ADD_CORE_EVENT(ON_ACTOR_EQUIPMENT),
             ADD_CORE_EVENT(ON_ACTOR_CELL_CHANGE),
             ADD_CORE_EVENT(ON_ACTOR_LIST),
             ADD_CORE_EVENT(ON_ACTOR_TEST),
             ADD_CORE_EVENT(ON_CELL_LOAD),
             ADD_CORE_EVENT(ON_CELL_UNLOAD),
             ADD_CORE_EVENT(ON_CELL_DELETION),
             ADD_CORE_EVENT(ON_CONTAINER),
             ADD_CORE_EVENT(ON_DOOR_STATE),
             ADD_CORE_EVENT(ON_OBJECT_PLACE),
             ADD_CORE_EVENT(ON_OBJECT_STATE),
             ADD_CORE_EVENT(ON_OBJECT_SPAWN),
             ADD_CORE_EVENT(ON_OBJECT_DELETE),
             ADD_CORE_EVENT(ON_OBJECT_LOCK),
             ADD_CORE_EVENT(ON_OBJECT_SCALE),
             ADD_CORE_EVENT(ON_OBJECT_TRAP)
            );

    sol::state &state = *luaCtrl->getState();
    sol::table eventsEnum = state["Events"];
    for (int i = CoreEvent::FIRST; i < CoreEvent::LAST; ++i)
    {
#ifdef SERVER_DEBUG
        bool found = false;
        eventsEnum.for_each([&found, &i](sol::object key, sol::object value){
            if (value.as<int>() == i)
            {
                found = true;
                return;
            }
        });

        if (!found)
            throw runtime_error("Event " + to_string(i) + " is not registered. Dear developer, please fix me :D");
#endif
        events[i]; // create core event
    }
}

void EventController::registerEvent(int event, sol::environment &env, sol::function& func)
{
    auto iter = events.find(event);
    if (iter != events.end())
        iter->second.push(env, func);
}

void EventController::stop(int event)
{
    printf("EventController::stop\n");
    auto iter = events.find(event);
    if (iter != events.end())
        iter->second.stop();
}

CallbackCollection &EventController::GetEvents(Event event)
{
    return events.at(event);
}

Event EventController::createEvent()
{
    events[lastEvent];
    return lastEvent++;
}

void EventController::raiseEvent(Event id, sol::table data, const string &modname)
{
    auto iter = events.find(id);
    if (iter != events.end())
    {
        if (!modname.empty())
        {
            auto f = std::find_if (iter->second.begin(), iter->second.end(), [&modname](const auto &item){
                return item.first["ModName"]["name"] == modname;
            });
            if (f != iter->second.end())
                f->second.call(data); // call only specified mod
        }
        iter->second.call(CallbackCollection::type_tag<void>(), data); // call all registered events with this id
    }
    else
        cerr << "Event with id: " << id << " is not registered" << endl;
}