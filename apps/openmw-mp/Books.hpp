//
// Created by koncord on 15.08.17.
//

#pragma once

#include <string>

class LuaState;
class Player;

class Books
{
public:
    static void Init(LuaState &lua);
public:

    explicit Books(Player *player);
    ~Books();

    void addBook(const std::string &bookId);
    std::string getBookId(unsigned i) const;
    unsigned getChanges() const;
    void reset();

    void update();
private:
    Player *player;
    bool changed;
};


