#include "Spells.hpp"

#include <components/misc/stringops.hpp>
#include <components/openmw-mp/NetworkMessages.hpp>

#include <apps/openmw-mp/Script/ScriptFunctions.hpp>
#include <apps/openmw-mp/Networking.hpp>

using namespace mwmp;

Player* storedPlayerCaster;
std::vector<ESM::ActiveEffect> storedActiveEffects;

void SpellFunctions::ClearSpellbookChanges(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->spellbookChanges.spells.clear();
}

void SpellFunctions::ClearSpellsActiveChanges(unsigned short pid) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, );

    player->spellsActiveChanges.activeSpells.clear();
}

void SpellFunctions::ClearCooldownChanges(unsigned short pid) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, );

    player->cooldownChanges.clear();
}

unsigned int SpellFunctions::GetSpellbookChangesSize(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, 0);

    return player->spellbookChanges.spells.size();
}

unsigned int SpellFunctions::GetSpellbookChangesAction(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, 0);

    return player->spellbookChanges.action;
}

unsigned int SpellFunctions::GetSpellsActiveChangesSize(unsigned short pid) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0);

    return player->spellsActiveChanges.activeSpells.size();
}

unsigned int SpellFunctions::GetSpellsActiveChangesAction(unsigned short pid) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0);

    return player->spellsActiveChanges.action;
}

unsigned int SpellFunctions::GetCooldownChangesSize(unsigned short pid) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0);

    return player->cooldownChanges.size();
}

void SpellFunctions::SetSpellbookChangesAction(unsigned short pid, unsigned char action) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->spellbookChanges.action = action;
}

void SpellFunctions::SetSpellsActiveChangesAction(unsigned short pid, unsigned char action) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, );

    player->spellsActiveChanges.action = action;
}

void SpellFunctions::AddSpell(unsigned short pid, const char* spellId) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    ESM::Spell spell;
    spell.mId = spellId;

    player->spellbookChanges.spells.push_back(spell);
}

void SpellFunctions::AddSpellActive(unsigned short pid, const char* spellId, const char* displayName, bool stackingState) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, );

    mwmp::ActiveSpell spell;
    spell.id = spellId;
    spell.isStackingSpell = stackingState;
    spell.params.mDisplayName = displayName;
    spell.params.mEffects = storedActiveEffects;

    if (storedPlayerCaster != nullptr)
    {
        spell.caster.isPlayer = true;
        spell.caster.guid = storedPlayerCaster->guid;
    }

    player->spellsActiveChanges.activeSpells.push_back(spell);

    storedPlayerCaster = nullptr;
    storedActiveEffects.clear();
}

void SpellFunctions::AddSpellActiveEffect(unsigned short pid, int effectId, double magnitude, double duration, double timeLeft, int arg) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, );

    ESM::ActiveEffect effect;
    effect.mEffectId = effectId;
    effect.mMagnitude = magnitude;
    effect.mDuration = duration;
    effect.mTimeLeft = timeLeft;
    effect.mArg = arg;

    storedActiveEffects.push_back(effect);
}

void SpellFunctions::AddCooldownSpell(unsigned short pid, const char* spellId, unsigned int startDay, double startHour) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, );

    mwmp::SpellCooldown spellCooldown;
    spellCooldown.id = spellId;
    spellCooldown.startTimestampDay = startDay;
    spellCooldown.startTimestampHour = startHour;

    player->cooldownChanges.push_back(spellCooldown);
}

const char *SpellFunctions::GetSpellId(unsigned short pid, unsigned int index) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, "");

    if (index >= player->spellbookChanges.spells.size())
        return "invalid";

    return player->spellbookChanges.spells.at(index).mId.c_str();
}

const char* SpellFunctions::GetSpellsActiveId(unsigned short pid, unsigned int index) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, "");

    if (index >= player->spellsActiveChanges.activeSpells.size())
        return "invalid";

    return player->spellsActiveChanges.activeSpells.at(index).id.c_str();
}

const char* SpellFunctions::GetSpellsActiveDisplayName(unsigned short pid, unsigned int index) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, "");

    if (index >= player->spellsActiveChanges.activeSpells.size())
        return "invalid";

    return player->spellsActiveChanges.activeSpells.at(index).params.mDisplayName.c_str();
}

bool SpellFunctions::GetSpellsActiveStackingState(unsigned short pid, unsigned int index) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, false);

    if (index >= player->spellsActiveChanges.activeSpells.size())
        return false;

    return player->spellsActiveChanges.activeSpells.at(index).isStackingSpell;
}

unsigned int SpellFunctions::GetSpellsActiveEffectCount(unsigned short pid, unsigned int index) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0);

    if (index >= player->spellsActiveChanges.activeSpells.size())
        return 0;

    return player->spellsActiveChanges.activeSpells.at(index).params.mEffects.size();
}

unsigned int SpellFunctions::GetSpellsActiveEffectId(unsigned short pid, unsigned int spellIndex, unsigned int effectIndex) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0);

    if (spellIndex >= player->spellsActiveChanges.activeSpells.size())
        return 0;

    return player->spellsActiveChanges.activeSpells.at(spellIndex).params.mEffects.at(effectIndex).mEffectId;
}

int SpellFunctions::GetSpellsActiveEffectArg(unsigned short pid, unsigned int spellIndex, unsigned int effectIndex) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0);

    if (spellIndex >= player->spellsActiveChanges.activeSpells.size())
        return 0;

    return player->spellsActiveChanges.activeSpells.at(spellIndex).params.mEffects.at(effectIndex).mArg;
}

double SpellFunctions::GetSpellsActiveEffectMagnitude(unsigned short pid, unsigned int spellIndex, unsigned int effectIndex) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0);

    if (spellIndex >= player->spellsActiveChanges.activeSpells.size())
        return 0;

    return player->spellsActiveChanges.activeSpells.at(spellIndex).params.mEffects.at(effectIndex).mMagnitude;
}

double SpellFunctions::GetSpellsActiveEffectDuration(unsigned short pid, unsigned int spellIndex, unsigned int effectIndex) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0);

    if (spellIndex >= player->spellsActiveChanges.activeSpells.size())
        return 0;

    return player->spellsActiveChanges.activeSpells.at(spellIndex).params.mEffects.at(effectIndex).mDuration;
}

double SpellFunctions::GetSpellsActiveEffectTimeLeft(unsigned short pid, unsigned int spellIndex, unsigned int effectIndex) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0);

    if (spellIndex >= player->spellsActiveChanges.activeSpells.size())
        return 0;

    return player->spellsActiveChanges.activeSpells.at(spellIndex).params.mEffects.at(effectIndex).mTimeLeft;
}

bool SpellFunctions::DoesSpellsActiveHavePlayerCaster(unsigned short pid, unsigned int index) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, false);

    if (index >= player->spellsActiveChanges.activeSpells.size())
        return false;

    return player->spellsActiveChanges.activeSpells.at(index).caster.isPlayer;
}

int SpellFunctions::GetSpellsActiveCasterPid(unsigned short pid, unsigned int index) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, -1);

    if (index >= player->spellsActiveChanges.activeSpells.size())
        return -1;

    Player* caster = Players::getPlayer(player->spellsActiveChanges.activeSpells.at(index).caster.guid);

    if (caster != nullptr)
        return caster->getId();

    return -1;
}

const char* SpellFunctions::GetSpellsActiveCasterRefId(unsigned short pid, unsigned int index) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, "");

    if (index >= player->spellsActiveChanges.activeSpells.size())
        return "";

    return player->spellsActiveChanges.activeSpells.at(index).caster.refId.c_str();
}

unsigned int SpellFunctions::GetSpellsActiveCasterRefNum(unsigned short pid, unsigned int index) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0);

    if (index >= player->spellsActiveChanges.activeSpells.size())
        return 0;

    return player->spellsActiveChanges.activeSpells.at(index).caster.refNum;
}

unsigned int SpellFunctions::GetSpellsActiveCasterMpNum(unsigned short pid, unsigned int index) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0);

    if (index >= player->spellsActiveChanges.activeSpells.size())
        return 0;

    return player->spellsActiveChanges.activeSpells.at(index).caster.mpNum;
}

const char* SpellFunctions::GetCooldownSpellId(unsigned short pid, unsigned int index) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, "");

    if (index >= player->cooldownChanges.size())
        return "invalid";

    return player->cooldownChanges.at(index).id.c_str();
}

unsigned int SpellFunctions::GetCooldownStartDay(unsigned short pid, unsigned int index) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0);

    if (index >= player->cooldownChanges.size())
        return 0;

    return player->cooldownChanges.at(index).startTimestampDay;
}

double SpellFunctions::GetCooldownStartHour(unsigned short pid, unsigned int index) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0.0);

    if (index >= player->cooldownChanges.size())
        return 0.0;

    return player->cooldownChanges.at(index).startTimestampHour;
}

void SpellFunctions::SetSpellsActiveCasterPid(unsigned short casterPid) noexcept
{
    storedPlayerCaster = Players::getPlayer(casterPid);
}

void SpellFunctions::SendSpellbookChanges(unsigned short pid, bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    mwmp::PlayerPacket *packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_SPELLBOOK);
    packet->setPlayer(player);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void SpellFunctions::SendSpellsActiveChanges(unsigned short pid, bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, );

    mwmp::PlayerPacket* packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_SPELLS_ACTIVE);
    packet->setPlayer(player);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void SpellFunctions::SendCooldownChanges(unsigned short pid) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, );

    mwmp::PlayerPacket* packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_COOLDOWNS);
    packet->setPlayer(player);
    packet->Send(false);
}

// All methods below are deprecated versions of methods from above

void SpellFunctions::InitializeSpellbookChanges(unsigned short pid) noexcept
{
    ClearSpellbookChanges(pid);
}
