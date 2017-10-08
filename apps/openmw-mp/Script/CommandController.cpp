//
// Created by koncord on 08.08.17.
//
#include "CommandController.hpp"

#include <boost/spirit/home/x3.hpp>
#include <iostream>
#include "Player.hpp"
#include "LuaState.hpp"

using namespace std;

void CommandController::Init(LuaState &lua)
{
    sol::table cmdCtrlTable = lua.getState()->create_named_table("CommandController");

    cmdCtrlTable["registerCommand"] = [&lua](const std::string &command, sol::function func, const std::string &helpMessage) {
        return lua.getCmdCtrl().registerCommand(command, func, helpMessage);
    };

    cmdCtrlTable["unregisterCommand"] = [&lua](const std::string &command) {
        lua.getCmdCtrl().unregisterCommand(command);
    };

    cmdCtrlTable["hasCommand"] = [&lua](const std::string &command) {
        return lua.getCmdCtrl().hasCommand(command);
    };

    cmdCtrlTable["getHelpStrings"] = [&lua]() {
        auto &commands = lua.getCmdCtrl().commands;
        sol::table helpTable = lua.getState()->create_table(commands.size(), 0);

        for (const auto &cmd : commands)
            helpTable.add(cmd.first, cmd.second.helpMessage);

        return helpTable;
    };
}

bool CommandController::registerCommand(const std::string &command, sol::function func, const std::string &helpMessage)
{
    auto iter = commands.find(command);
    if (iter == commands.end())
    {
        commands.emplace(command, Command {std::move(func), helpMessage});
        return true;
    }
    return false;
}

void CommandController::unregisterCommand(const std::string &command)
{
    commands.erase(command);
}

bool CommandController::hasCommand(const std::string &command)
{
    return commands.find(command) != commands.end();
}

std::pair<CommandController::ExecResult, std::string> CommandController::exec(std::shared_ptr<Player> player, const std::string &message)
{
    char cmdChar = message[0];
    if (message.size() < 2 || (cmdChar != '/' && cmdChar != '!'))
        return make_pair(ExecResult::NOT_CMD, "");

    auto tokens = cmdParser(message);

    auto cmd = commands.find(&tokens[0][1]);
    if (cmd != commands.end())
    {
        tokens.pop_front();
        bool result = cmd->second.func(player, sol::as_table(tokens));
        if (result)
            return make_pair(ExecResult::SUCCESS, "");

        return make_pair(ExecResult::FAIL, cmd->second.helpMessage);
    }

    return make_pair(ExecResult::NOT_FOUND, "");
}

std::deque<std::string> CommandController::cmdParser(const std::string &message)
{
    deque<string> ret;
    namespace x3 = boost::spirit::x3;
    auto const sep = ' ';
    auto const quoted = '"' >> *~x3::char_('"') >> '"';
    auto const unquoted = *~x3::char_(sep);
    auto const arguments = (quoted | unquoted) % sep;
    /*auto const command = '/' >> *~x3::char_(sep) >> sep;

    if (!x3::phrase_parse(message.cbegin(), message.cend(), arguments, command, ret))
        throw runtime_error("failed to parse message: "+ message);*/

    if (!x3::parse(message.cbegin(), message.cend(), arguments, ret))
        throw runtime_error("failed to parse message: " + message);

    return ret;
}