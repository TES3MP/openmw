//
// Created by koncord on 15.08.17.
//

#include "Dialogue.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>
#include "Script/LuaState.hpp"
#include "Player.hpp"
#include "Networking.hpp"

void Dialogue::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Dialogue>("Dialogue",
    "addTopic", &Dialogue::addTopic,
    "getTopicId", &Dialogue::getTopicId,
    "getChanges", &Dialogue::getChanges,
    "reset", &Dialogue::reset);
}



Dialogue::Dialogue(Player *player) : player(player), changed(false)
{

}


void Dialogue::reset()
{
    player->topicChanges.topics.clear();
}

void Dialogue::update()
{
    if (!changed)
        return;
    changed = false;

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_TOPIC);

    packet->setPlayer(player);
    packet->Send(/*toOthers*/ false);
}

void Dialogue::addTopic(const std::string &topicId)
{
    if (!changed)
        reset();
    changed = true;
    player->topicChanges.topics.push_back({topicId});
}

std::string Dialogue::getTopicId(unsigned int i) const
{
    return player->topicChanges.topics.at(i).topicId;
}

unsigned int Dialogue::getChanges() const
{
    return player->topicChanges.count;
}
