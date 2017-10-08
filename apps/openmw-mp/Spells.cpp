//
// Created by koncord on 25.08.17.
//

#include "Spells.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>
#include "Script/LuaState.hpp"
#include "Player.hpp"
#include "Networking.hpp"

void Effect::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Effect>("Effect",
                                         "id", sol::property(&Effect::getId, &Effect::setId),
                                         "skill", sol::property(&Effect::getSkill, &Effect::setSkill),
                                         "attribute", sol::property(&Effect::getAttribute, &Effect::setAttribute),
                                         "range", sol::property(&Effect::getRange, &Effect::setRange),
                                         "area", sol::property(&Effect::getArea, &Effect::setArea),
                                         "duration", sol::property(&Effect::getDuration, &Effect::setDuration),
                                         "magMin", sol::property(&Effect::getMinMag, &Effect::setMinMag),
                                         "magMax", sol::property(&Effect::getMaxMag, &Effect::setMaxMag)

    );
}

Effect::Effect(ESM::ENAMstruct effect) : effect(effect)
{
}


short Effect::getId() const
{
    return effect.mEffectID;
}

void Effect::setId(short id)
{
    effect.mEffectID = id;

}

signed char Effect::getSkill() const
{
    return effect.mSkill;
}

void Effect::setSkill(signed char skill)
{
    effect.mSkill = skill;

}

signed char Effect::getAttribute() const
{
    return effect.mAttribute;
}

void Effect::setAttribute(signed char attr)
{
    effect.mAttribute = attr;

}

int Effect::getRange() const
{
    return effect.mRange;
}

void Effect::setRange(int range)
{
    effect.mRange = range;

}

int Effect::getArea() const
{
    return effect.mArea;
}

void Effect::setArea(int area)
{
    effect.mArea = area;

}

int Effect::getDuration() const
{
    return effect.mDuration;
}

void Effect::setDuration(int dur)
{
    effect.mDuration = dur;

}

int Effect::getMinMag() const
{
    return effect.mMagnMin;
}

void Effect::setMinMag(int mag)
{
    effect.mMagnMin = mag;

}

int Effect::getMaxMag() const
{
    return effect.mMagnMax;
}

void Effect::setMaxMag(int mag)
{
    effect.mMagnMax = mag;

}

void Spell::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Spell>("Spell",

                                        "id", sol::property(&Spell::getId, &Spell::setId),
                                        "name", sol::property(&Spell::getName, &Spell::setName),
                                        "type", sol::property(&Spell::getType, &Spell::setType),
                                        "cost", sol::property(&Spell::getCost, &Spell::setCost),
                                        "flags", sol::property(&Spell::getFlags, &Spell::setFlags),

                                        "getEffectCount", &Spell::getEffectCount,

                                        "addEffect", &Spell::addEffect,
                                        "getEffect", &Spell::getEffect
    );
}

Spell::Spell(const ESM::Spell &spell) : spell(spell)
{

}

std::string Spell::getId() const
{
    return spell.mId;
}

void Spell::setId(const std::string &id)
{
    spell.mId = id;
}

std::string Spell::getName() const
{
    return spell.mName;
}

void Spell::setName(const std::string &name)
{
    spell.mName = name;
}

int Spell::getType() const
{
    return spell.mData.mType;
}

void Spell::setType(int type)
{
    spell.mData.mType = type;
}

int Spell::getCost() const
{
    return spell.mData.mCost;
}

void Spell::setCost(int cost)
{
    spell.mData.mCost = cost;
}

int Spell::getFlags() const
{
    return spell.mData.mFlags;
}

void Spell::setFlags(int flags)
{
    spell.mData.mFlags = flags;
}

size_t Spell::getEffectCount() const
{
    return spell.mEffects.mList.size();
}

void Spell::addEffect(Effect effect)
{
    spell.mEffects.mList.push_back(effect.effect);
}

Effect Spell::getEffect(int i)
{
    return Effect(spell.mEffects.mList.at(i));
}

void Spells::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Spells>("Spells",

                                         "addCustomSpell", &Spells::addCustomSpell,
                                         "getCustomSpell", &Spells::getCustomSpell,
                                         "addDefaultSpell", &Spells::addDefaultSpell,
                                         "size", &Spells::size

    );
}

Spells::Spells(Player *player) : player(player), changed(false)
{

}

Spells::~Spells()
{

}

void Spells::update()
{
    if (!changed)
        return;
    changed = false;

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_SPELLBOOK);
    packet->setPlayer(player);
    packet->Send(false);
}

void Spells::addCustomSpell(Spell spell)
{
    if (!changed)
        clear();
    changed = true;

    player->spellbookChanges.spells.push_back(spell.spell);
}

void Spells::addDefaultSpell(const std::string &spell)
{
    if (!changed)
        clear();
    changed = true;

    ESM::Spell s;
    s.mId = spell;
    player->spellbookChanges.spells.push_back(s);
}

Spell Spells::getCustomSpell(unsigned int i)
{
    return Spell(player->spellbookChanges.spells.at(i));
}

void Spells::clear()
{
    player->spellbookChanges.spells.clear();
}

size_t Spells::size() const
{
    return player->spellbookChanges.spells.size();
}
