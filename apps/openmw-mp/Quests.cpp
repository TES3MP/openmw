//
// Created by koncord on 25.08.17.
//

#include "Quests.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>
#include "Script/LuaState.hpp"
#include "Player.hpp"
#include "Networking.hpp"

void JournalItem::Init(LuaState &lua)
{
    lua.getState()->new_usertype<JournalItem>("JournalItem",
                                              "quest", sol::property(&JournalItem::getQuest, &JournalItem::setQuest),
                                              "index", sol::property(&JournalItem::getIndex, &JournalItem::setIndex),
                                              "actorRefId", sol::property(&JournalItem::getActorRefId, &JournalItem::setActorRefId),
                                              "type", sol::property(&JournalItem::getType, &JournalItem::setType)
    );
}

JournalItem::JournalItem(mwmp::JournalItem item) : item(item)
{

}

JournalItem::~JournalItem()
{

}

std::string JournalItem::getQuest() const
{
    return item.quest;
}

void JournalItem::setQuest(const std::string &quest)
{
    item.quest = quest;
}

int JournalItem::getIndex() const
{
    return item.index;
}

void JournalItem::setIndex(int index)
{
    item.index = index;
}

int JournalItem::getType() const
{
    return item.type;
}

void JournalItem::setType(int type)
{
    item.type = type;
}

std::string JournalItem::getActorRefId() const
{
    return item.actorRefId;
}

void JournalItem::setActorRefId(const std::string &refid)
{
    item.actorRefId = refid;
}

void Quests::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Quests>("Quests",
                                         "getJournalChangesSize", &Quests::getJournalChangesSize,
                                         "getKillChangesSize", &Quests::getKillChangesSize,
                                         
                                         "addJournalItem", &Quests::addJournalItem,
                                         "setJournalItem", &Quests::setJournalItem,
                                         "getJournalItem", &Quests::getJournalItem,
                                         
                                         "addKill", &Quests::addKill,
                                         "getKill", &Quests::getKill
    );
}

Quests::Quests(Player *player) : player(player), changedKills(false), changedJournal(false)
{

}

Quests::~Quests()
{

}

void Quests::update()
{
    if (changedJournal)
    {
        changedJournal = false;
        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_JOURNAL);

        packet->setPlayer(player);
        packet->Send(/*toOthers*/ false);
    }
    
    if (changedKills)
    {
        changedKills = false;
        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_KILL_COUNT);

        packet->setPlayer(player);
        packet->Send(/*toOthers*/ false);
    }
}

size_t Quests::getJournalChangesSize() const
{
    return player->journalChanges.journalItems.size();
}

size_t Quests::getKillChangesSize() const
{
    return player->killChanges.kills.size();
}

void Quests::addJournalItem(JournalItem item)
{
    player->journalChanges.journalItems.push_back(item.item);
    changedJournal = true;
    
}

void Quests::setJournalItem(unsigned int id, JournalItem item)
{
    player->journalChanges.journalItems.at(id) = item.item;
    changedJournal = true;
}

JournalItem Quests::getJournalItem(unsigned int id)
{
    return JournalItem(player->journalChanges.journalItems.at(id));
}

void Quests::addKill(const std::string &refId, int number)
{
    player->killChanges.kills.push_back({refId, number});
    changedKills = true;
}

std::tuple<std::string, int> Quests::getKill(unsigned int i) const
{
    auto & kill = player->killChanges.kills.at(i);
    return std::make_tuple(kill.refId, kill.number);
}
