//
// Created by koncord on 30.07.17.
//

#pragma once

#include "sol.hpp"
#include "Utils.hpp"
#include "LuaState.hpp"

typedef unsigned Event;

namespace CoreEvent
{
    enum
    {
        ON_EXIT = 0,
        ON_POST_INIT,
        ON_REQUEST_PLUGIN_LIST,
        ON_PLAYER_CONNECT,
        ON_PLAYER_DISCONNECT,
        ON_PLAYER_DEATH,
        ON_PLAYER_RESURRECT,
        ON_PLAYER_CELLCHANGE,
        ON_PLAYER_KILLCOUNT,
        ON_PLAYER_ATTRIBUTE,
        ON_PLAYER_SKILL,
        ON_PLAYER_LEVEL,
        ON_PLAYER_BOUNTY,
        ON_PLAYER_EQUIPMENT,
        ON_PLAYER_INVENTORY,
        ON_PLAYER_JOURNAL,
        ON_PLAYER_FACTION,
        ON_PLAYER_SHAPESHIFT,
        ON_PLAYER_SPELLBOOK,
        ON_PLAYER_TOPIC,
        ON_PLAYER_DISPOSITION,
        ON_PLAYER_BOOK,
        ON_PLAYER_MAP,
        ON_PLAYER_REST,
        ON_PLAYER_SENDMESSAGE,
        ON_PLAYER_ENDCHARGEN,

        ON_GUI_ACTION,
        ON_MP_REFNUM,

        ON_ACTOR_EQUIPMENT,
        ON_ACTOR_CELL_CHANGE,
        ON_ACTOR_LIST,
        ON_ACTOR_TEST,

        ON_CELL_LOAD,
        ON_CELL_UNLOAD,
        ON_CELL_DELETION,

        ON_CONTAINER,
        ON_DOOR_STATE,
        ON_OBJECT_PLACE,
        ON_OBJECT_STATE,
        ON_OBJECT_SPAWN,
        ON_OBJECT_DELETE,
        ON_OBJECT_LOCK,
        ON_OBJECT_SCALE,
        ON_OBJECT_TRAP,

        LAST,
    };
    const int FIRST = ON_EXIT;
}

class CallbackCollection // todo: add sort by dependencies
{
public:
    typedef std::vector<std::pair<sol::environment, sol::function>> Container;
    typedef Container::iterator Iterator;
    typedef Container::const_iterator CIterator;

    template<typename> struct type_tag {};

    void push(sol::environment &env, sol::function &func) { functions.emplace_back(env, func); }
    CIterator begin() const { return functions.begin(); }
    CIterator end() const { return functions.end(); }

    void stop() {_stop = true;}
    bool isStoped() const {return _stop;}
    CIterator stopedAt() const { return lastCalled; }

    auto get(const std::string &modname)
    {
        for(auto iter = functions.cbegin(); iter != functions.cend(); ++iter)
        {
            if(iter->first["ModName"]["name"] == modname)
                return iter;
        }
        return functions.cend();
    }


    template<typename... Args>
    void call(type_tag<void>, Args&&... args)
    {
        lastCalled = functions.end();
        _stop = false;
        for(CIterator iter = functions.begin(); iter != functions.end(); ++iter)
        {
            if(!_stop)
                iter->second.call(std::forward<Args>(args)...);
            else
            {
                lastCalled = iter;
                break;
            }
        }

    }

    template<typename R, typename... Args>
    decltype(auto) call(type_tag<R>, Args&&... args)
    {
        R ret;

        lastCalled = functions.end();
        _stop = false;
        for(CIterator iter = functions.begin(); iter != functions.end(); ++iter)
        {
            if (!_stop)
                ret = iter->second.call(std::forward<Args>(args)...);
            else
            {
                lastCalled = iter;
                break;
            }
        }
        return ret;
    }

private:
    Container functions;
    CIterator lastCalled;
    bool _stop = false;
};

class EventController
{
public:
    static void Init(LuaState &lua);
public:
    explicit EventController(LuaState *luaCtrl);
    typedef std::unordered_map<int, CallbackCollection> Container;

    void registerEvent(int event, sol::environment &env, sol::function& func);
    CallbackCollection& GetEvents(Event event);
    Event createEvent();
    void raiseEvent(Event id, sol::table data, const std::string &modname = "");
    void stop(int event);

    template<Event event, typename R = void, typename... Args>
    R Call(Args&&... args)
    {
        return events.at(event).call(CallbackCollection::type_tag<R>{}, std::forward<Args>(args)...);
    }
private:
    Container events;
    Event lastEvent = CoreEvent::LAST;
    LuaState *luaCtrl;
};