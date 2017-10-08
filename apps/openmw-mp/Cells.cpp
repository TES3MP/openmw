//
// Created by koncord on 25.08.17.
//

#include "Cells.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>
#include "Script/LuaState.hpp"
#include "NetActor.hpp"
#include "Networking.hpp"

using namespace std;

void Cells::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Cells>("Cell",
                                        "cell", sol::property(&Cells::getCell, &Cells::setCell),
                                        "getExterior", &Cells::getExterior,
                                        "setExterior", &Cells::setExterior,
                                        "getRegion", &Cells::getRegion,

                                        "isExterior", &Cells::isExterior,
                                        "isChangingRegion", &Cells::isChangingRegion
    );
}

Cells::Cells(NetActor *netActor) : netActor(netActor), changedCell(false)
{

}

Cells::~Cells()
{

}

void Cells::update()
{

}

std::string Cells::getCell() const
{
    return netActor->getNetCreature()->cell.getDescription();
}

void Cells::setCell(const std::string &cellDescription)
{
    /*LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Script is moving %s from %s to %s", netActor->getNetCreature()->npc.mName.c_str(),
                       netActor->getNetCreature()->cell.getDescription().c_str(), cellDescription.c_str());*/

    netActor->getNetCreature()->cell = Utils::getCellFromDescription(cellDescription);
    changedCell = true;
}

std::tuple<int, int> Cells::getExterior() const
{
    return make_tuple(netActor->getNetCreature()->cell.mData.mX, netActor->getNetCreature()->cell.mData.mY);
}

void Cells::setExterior(int x, int y)
{
    /*LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Script is moving %s from %s to %i,%i", netActor->getNetCreature()->npc.mName.c_str(),
                       netActor->getNetCreature()->cell.getDescription().c_str(), x, y);*/

    // If the player is currently in an interior, turn off the interior flag from the cell
    if (!netActor->getNetCreature()->cell.isExterior())
        netActor->getNetCreature()->cell.mData.mFlags &= ~ESM::Cell::Interior;

    netActor->getNetCreature()->cell.mData.mX = x;
    netActor->getNetCreature()->cell.mData.mY = y;
    changedCell = true;
}

bool Cells::isExterior() const
{
    return netActor->getNetCreature()->cell.isExterior();
}

bool Cells::isChangingRegion() const
{
    return netActor->getNetCreature()->isChangingRegion;
}

std::string Cells::getRegion() const
{
    return netActor->getNetCreature()->cell.mRegion;
}

bool Cells::isChangedCell() const
{
    return changedCell;
}

void Cells::resetChangedCell()
{
    changedCell = false;
}

