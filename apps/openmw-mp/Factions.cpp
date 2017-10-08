//
// Created by koncord on 17.08.17.
//

#include <components/openmw-mp/NetworkMessages.hpp>
#include "Factions.hpp"

#include "Player.hpp"
#include "Script/LuaState.hpp"
#include "Networking.hpp"

void Factions::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Factions>("Factions",
                                           "addFaction", &Factions::addFaction,
                                           "changesAction", sol::property(&Factions::getFactionChangesAction, &Factions::setFactionChangesAction),
                                           "getFaction", &Factions::getFaction,
                                           "setFaction", &Factions::setFaction,
                                           "clear", &Factions::clear,
                                           "size", &Factions::size

    );
/*"InitializeFactionChanges", FactionFunctions::InitializeFactionChanges,
    "GetFactionChangesSize",    FactionFunctions::GetFactionChangesSize,
    "GetFactionChangesAction",  FactionFunctions::GetFactionChangesAction,
    "GetFactionId",             FactionFunctions::GetFactionId,
    "GetFactionRank",           FactionFunctions::GetFactionRank,
    "GetFactionExpulsionState", FactionFunctions::GetFactionExpulsionState,
    "GetFactionReputation",     FactionFunctions::GetFactionReputation,
    "SetFactionChangesAction",  FactionFunctions::SetFactionChangesAction,
    "SetFactionId",             FactionFunctions::SetFactionId,
    "SetFactionRank",           FactionFunctions::SetFactionRank,
    "SetFactionExpulsionState", FactionFunctions::SetFactionExpulsionState,
    "SetFactionReputation",     FactionFunctions::SetFactionReputation,
    "AddFaction",               FactionFunctions::AddFaction,
    "SendFactionChanges",       FactionFunctions::SendFactionChanges*/
}

Factions::Factions(Player *player): player(player), changed(false)
{

}

Factions::~Factions()
{

}

void Factions::update()
{
    if (!changed)
        return;
    changed = false;

    auto packet =mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_FACTION);
    packet->setPlayer(player);
    packet->Send(/*toOthers*/ false);
    clear();
}

int Factions::getFactionChangesAction() const
{
    return player->factionChanges.action;
}

void Factions::setFactionChangesAction(int action)
{
    player->factionChanges.action = action;
    changed = true;
}

void Factions::addFaction(Faction faction)
{
    player->factionChanges.factions.push_back(faction.faction);
    changed = true;
}


Faction Factions::getFaction(int id) const
{
    return Faction(player->factionChanges.factions.at(id));
}

void Factions::setFaction(int id, Faction faction)
{
    player->factionChanges.factions.at(id) = faction.faction;
    changed = true;
}

void Factions::clear()
{
    player->factionChanges.factions.clear();
    changed = true;
}

size_t Factions::size() const
{
    return player->factionChanges.factions.size();
}

void Faction::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Faction>("Faction",
                                          "factionId", sol::property(&Faction::getFactionId, &Faction::setFactionId),
                                          "rank", sol::property(&Faction::getFactionRank, &Faction::setFactionRank),
                                          "isExpelled", sol::property(&Faction::getFactionExpulsionState, &Faction::setFactionExpulsionState),
                                          "reputation", sol::property(&Faction::getFactionReputation, &Faction::setFactionReputation)
    );
}

Faction::Faction(mwmp::Faction &faction): faction(faction)
{

}

std::string Faction::getFactionId() const
{
    return faction.factionId;
}

void Faction::setFactionId(const std::string &factionId)
{
    faction.factionId = factionId;
}

int Faction::getFactionRank() const
{
    return faction.rank;
}

void Faction::setFactionRank(unsigned int rank)
{
    faction.rank = rank;
}

bool Faction::getFactionExpulsionState() const
{
    return faction.isExpelled;
}

void Faction::setFactionExpulsionState(bool expulsionState)
{
    faction.isExpelled = expulsionState;
}

int Faction::getFactionReputation() const
{
    return faction.reputation;
}

void Faction::setFactionReputation(int reputation)
{
    faction.reputation = reputation;
}