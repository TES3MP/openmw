//
// Created by koncord on 12.08.17.
//

#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/global_fun.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include "Player.hpp"

class LuaState;


class Players
{
public:
    static void Init(LuaState &lua);
public:
    Players() = delete; // static class
protected:

    struct ByID {};
    struct ByGUID {};

public:
    typedef boost::multi_index_container<std::shared_ptr<Player>,
    boost::multi_index::indexed_by<
            boost::multi_index::random_access<>,
            boost::multi_index::ordered_unique<boost::multi_index::tag<ByGUID>, BOOST_MULTI_INDEX_CONST_MEM_FUN(Player, uint64_t, getGUID)>,
    boost::multi_index::ordered_unique<boost::multi_index::tag<ByID>, boost::multi_index::member<Player, unsigned short, &Player::id> >
    > > Store;


    static std::shared_ptr<Player> getPlayerByPID(int pid);
    static std::shared_ptr<Player> getPlayerByGUID(RakNet::RakNetGUID guid);
    static std::shared_ptr<Player> addPlayer(RakNet::RakNetGUID guid);
    static void deletePlayerByPID(int pid);
    static void deletePlayerByGUID(RakNet::RakNetGUID guid);
    static Store::const_iterator begin();
    static Store::const_iterator end();
    static size_t size();

    static void for_each(std::function<void(std::shared_ptr<Player>)> func);

private:
    static Store store;
};

