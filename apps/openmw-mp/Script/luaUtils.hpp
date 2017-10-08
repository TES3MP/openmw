//
// Created by koncord on 03.08.17.
//

#pragma once

#include "Utils.hpp"

#include "sol.hpp"

class LuaState;

class LuaUtils
{
    //Singleton(LuaUtils)
public:
    static void Init(LuaState &lua);
};
