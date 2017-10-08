//
// Created by koncord on 15.08.17.
//

#pragma once

#include <string>

class LuaState;
class Player;

class Dialogue
{
public:
    static void Init(LuaState &lua);
public:
    explicit Dialogue(Player *player);

    void addTopic(const std::string &topicId);
    std::string getTopicId(unsigned int i) const;
    unsigned int getChanges() const;

    void reset();
    void update();
private:
    Player *player;
    bool changed;
};


