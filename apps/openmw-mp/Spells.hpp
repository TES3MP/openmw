//
// Created by koncord on 25.08.17.
//

#pragma once

#include <components/esm/records.hpp>

class LuaState;
class Player;

class Effect
{
public:
    static void Init(LuaState &lua);
public:
    explicit Effect(ESM::ENAMstruct effect);

    short getId() const;
    void setId(short id);

    signed char getSkill() const;
    void setSkill(signed char skill);

    signed char getAttribute() const;
    void setAttribute(signed char attr);

    int getRange() const;
    void setRange(int range);

    int getArea() const;
    void setArea(int area);

    int getDuration() const;
    void setDuration(int dur);

    int getMinMag() const;
    int getMaxMag() const;

    void setMinMag(int mag);
    void setMaxMag(int mag);
    
    ESM::ENAMstruct effect;
};

class Spell
{
public:
    static void Init(LuaState &lua);
public:
    explicit Spell(const ESM::Spell &spell);

    std::string getId() const;
    void setId(const std::string &id);

    std::string getName() const;
    void setName(const std::string &name);

    int getType() const;
    void setType(int type);

    int getCost() const;
    void setCost(int cost);

    int getFlags() const;
    void setFlags(int flags);

    size_t getEffectCount() const;
    void addEffect(Effect effect);
    Effect getEffect(int i);
    
    ESM::Spell spell;
};

class Spells
{
public:
    static void Init(LuaState &lua);
public:
    explicit Spells(Player *player);
    ~Spells();
    
    void update();

    void addCustomSpell(Spell spell);
    Spell getCustomSpell(unsigned int i);
    void addDefaultSpell(const std::string &spell);

    size_t size() const;
    
private:
    void clear();

    Player *player;
    bool changed;
};


