//
// Created by koncord on 25.08.17.
//

#pragma once

#include <cstddef>
#include <string>
#include <components/openmw-mp/Base/BasePlayer.hpp>

class LuaState;
class NetActor;

class Cells
{
public:
    static void Init(LuaState &lua);
public:
    explicit Cells(NetActor *netActor);
    ~Cells();

    void update();

    std::string getCell() const;
    void setCell(const std::string &cellDescription);

    std::tuple<int, int> getExterior() const;
    void setExterior(int x, int y);

    bool isExterior() const;
    bool isChangingRegion() const;

    std::string getRegion() const;

    bool isChangedCell() const;
    void resetChangedCell();

private:
    NetActor *netActor;
    bool changedCell;
};


