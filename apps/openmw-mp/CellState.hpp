//
// Created by koncord on 25.08.17.
//

#pragma once

#include <string>
#include <components/openmw-mp/Base/BasePlayer.hpp>

class LuaState;

class CellState
{
public:
    static void Init(LuaState &lua);

    explicit CellState(mwmp::CellState state);
public:
    int getStateType() const;
    std::string getDescription() const;

private:
    mwmp::CellState state;
};
