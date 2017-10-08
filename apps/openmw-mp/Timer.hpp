//
// Created by koncord on 12.08.17.
//

#pragma once

#include <sol.hpp>

class LuaState;

class Timer
{
    friend class TimerController;
public:
    static void Init(LuaState &lua);
public:
    bool isElapsed();
    void stop();
    void start();
    void restart(long msec);

    Timer() = delete;
    ~Timer();
protected:
    Timer(sol::environment &env, sol::function &callback, long msec, sol::table &args);
    void tick();
private:
    double startTime, targetMsec;
    sol::function callback;
    sol::table data;
    sol::environment env;
    bool end;
};

class TimerController
{
public:
    static void Init(LuaState &lua);
public:
    std::shared_ptr<Timer> create(sol::environment env, sol::function callback, long msec, sol::table args);
    void kill(std::shared_ptr<Timer> timer);

    void terminate();
    void tick();
private:
    std::vector<std::shared_ptr<Timer>> timers;
};

