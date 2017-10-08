//
// Created by koncord on 25.08.17.
//

#include "CellState.hpp"

#include "Script/LuaState.hpp"

void CellState::Init(LuaState &lua)
{
    lua.getState()->new_usertype<CellState>("CellState",
                                            "type", sol::property(&CellState::getStateType),
                                            "description", sol::property(&CellState::getDescription)
    );
}

CellState::CellState(mwmp::CellState state) : state(state)
{

}

int CellState::getStateType() const
{
    return state.type;
}

std::string CellState::getDescription() const
{
    return state.cell.getDescription();
}