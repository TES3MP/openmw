//
// Created by koncord on 15.08.17.
//

#pragma once

class LuaState;
class Player;

class GUI
{
public:
    static void Init(LuaState &lua);
public:
    explicit GUI(Player *player);
    ~GUI();

    void update();

    void messageBox(int id, const char *label);

    void customMessageBox(int id, const char *label, const char *buttons);
    void inputDialog(int id, const char *label);
    void passwordDialog(int id, const char *label, const char *note);

    void listBox(int id, const char *label, const char *items);

    //state 0 - disallow, 1 - allow
    void setMapVisibility(unsigned short targetPID, unsigned short affectedPID, unsigned short state);
    void setMapVisibilityAll(unsigned short targetPID, unsigned short state);
private:
    Player *player;
    bool changed;
};


