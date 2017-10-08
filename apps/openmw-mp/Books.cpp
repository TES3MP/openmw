//
// Created by koncord on 15.08.17.
//

#include "Books.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>
#include "Script/LuaState.hpp"
#include "Player.hpp"
#include "Networking.hpp"

void Books::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Books>("Books",
                                        "addBook", &Books::addBook,
                                        "getBookId", &Books::getBookId,
                                        "getChanges", &Books::getChanges,
                                        "reset", &Books::reset
    );
}

Books::Books(Player *player) : player(player), changed(false)
{

}

Books::~Books()
{

}

void Books::addBook(const std::string &bookId)
{
    if (!changed)
        reset();
    player->bookChanges.books.push_back({bookId});
    changed = true;
}

std::string Books::getBookId(unsigned i) const
{
    if (i >= player->bookChanges.count)
        return "invalid";

    return player->bookChanges.books.at(i).bookId;
}

unsigned Books::getChanges() const
{
    return player->bookChanges.count;
}

void Books::reset()
{
    player->bookChanges.books.clear();
}

void Books::update()
{
    if (!changed)
        return;
    changed = false;

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_BOOK);

    packet->setPlayer(player);
    packet->Send(/*toOthers*/ false);
}