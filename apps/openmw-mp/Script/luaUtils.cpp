//
// Created by koncord on 03.08.17.
//

#include <boost/tokenizer.hpp>
#include "luaUtils.hpp"
#include "LuaState.hpp"

sol::table str_split(const std::string &str, const sol::object &sepObj, sol::this_state state)
{
    sol::state_view lua(state);
    sol::table tbl = lua.create_table();

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(sepObj.valid() ? sepObj.as<const char *>() : ":");
    tokenizer tokens(str, sep);

    for (const auto &token : tokens)
        tbl.add(token);

    return tbl;
}

void LuaUtils::Init(LuaState &lua)
{
    sol::table strTable = (*lua.getState())["string"];
    if (!strTable.valid())
        throw std::runtime_error("string library not initialized");

    strTable.set_function("split", &str_split);
}