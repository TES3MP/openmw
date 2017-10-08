//
// Created by koncord on 08.08.17.
//

#pragma once

#include "sol.hpp"
#include "Utils.hpp"
#include <string>
#include <deque>

class Player;
class LuaState;

struct Command
{
    sol::function func;
    std::string helpMessage;
};

class CommandController
{
    typedef std::unordered_map<std::string, Command> Container;
    typedef Container::iterator Iter;
    typedef Container::const_iterator CIter;
public:
    static void Init(LuaState &lua);
public:

    enum class ExecResult : int
    {
        NOT_FOUND,
        SUCCESS,
        FAIL,
        NOT_CMD
    };

    /**
     * Register new command. Only unique commands are allowed.
     * @param command name of command. Case sensitive. No need in command prefix ('/' or '!').
     * @param helpMessage help message. Shows in the '/help' command also appears if exec() fails.
     * @param callback Will be called when command is called.
     * @return false if the command already registered.
     */
    bool registerCommand(const std::string &command, sol::function callback, const std::string &helpMessage);
    /**
     * Removes a registered command
     * @param command name of command.
     */
    void unregisterCommand(const std::string &command);

    /**
     * Check a command is exist.
     * @param command name of command
     * @return false if the command did not exist.
     */
    bool hasCommand(const std::string &command);

    std::pair<ExecResult, std::string> exec(std::shared_ptr<Player> player, const std::string &message);
private:
    std::deque<std::string> cmdParser(const std::string &message);
    Container commands;
};