//
// Created by koncord on 12.08.17.
//

#include "Players.hpp"

using namespace std;

Players::Store Players::store;

void Players::Init(LuaState &lua)
{
    sol::table playersTable = lua.getState()->create_named_table("Players");
    playersTable.set_function("getByPID", &Players::getPlayerByPID);
    playersTable.set_function("getByGUID", &Players::getPlayerByGUID);
    playersTable.set_function("for_each", [](sol::function func)
    {
        for (shared_ptr<Player> player : store)
            func(player);
    });
}


std::shared_ptr<Player> Players::getPlayerByPID(int pid)
{
    const auto &ls = store.get<ByID>();
    auto it = ls.find(pid);
    if (it != ls.end())
        return *it;

    return nullptr;
}

std::shared_ptr<Player> Players::getPlayerByGUID(RakNet::RakNetGUID guid)
{
    const auto &ls = store.get<ByGUID>();
    auto it = ls.find(guid.g);
    if (it != ls.end())
        return *it;

    return nullptr;
}

std::shared_ptr<Player> Players::addPlayer(RakNet::RakNetGUID guid)
{
    const int maxConnections = 65535;
    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Creating new player with guid %lu", guid.g);

    auto player = make_shared<Player>(guid);

    unsigned short findPid = 0;
    const auto &ls = store.get<ByID>();
    for (; findPid < maxConnections; ++findPid) // find empty slot
    {
        auto it = ls.find(findPid);
        if (it == ls.end())
            break;
    }

    if (findPid >= maxConnections)
        return nullptr;

    LOG_APPEND(Log::LOG_INFO, "- Storing in slot %i", findPid);
    player->id = findPid;
    player->guid = guid;
    store.push_back(player);
    return player;
}

void Players::deletePlayerByPID(int pid)
{
    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Marking player (pid %lu) to deleting", pid);
    auto &ls = store.get<ByID>();
    auto it = ls.find(pid);
    if (it != ls.end())
    {
        ls.erase(it);
    }
}

void Players::deletePlayerByGUID(RakNet::RakNetGUID guid)
{
    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Marking player (guid %lu) to deleting", guid.g);
    auto &ls = store.get<ByGUID>();
    auto it = ls.find(guid.g);
    if (it != ls.end())
    {
        LOG_APPEND(Log::LOG_INFO, "- references: %d", it->use_count());
        ls.erase(it);
        LOG_APPEND(Log::LOG_INFO, "- references: %d", it->use_count());
    }
}

void Players::for_each(std::function<void (std::shared_ptr<Player>)> func)
{
    for (auto &player : store)
        func(player);
}

Players::Store::const_iterator Players::begin()
{
    return store.cbegin();
}

Players::Store::const_iterator Players::end()
{
    return store.cend();
}

size_t Players::size()
{
    return store.size();
}