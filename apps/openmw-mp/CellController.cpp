#include "CellController.hpp"

#include <iostream>
#include <apps/openmw-mp/Script/EventController.hpp>
#include "Cell.hpp"
#include "Player.hpp"
#include "Networking.hpp"

using namespace std;

CellController::CellController()
{

}

CellController::~CellController()
{
    for (auto cell : cells)
        delete cell;
}

CellController *CellController::sThis = nullptr;

void CellController::create()
{
    assert(!sThis);
    sThis = new CellController;
}

void CellController::destroy()
{
    assert(sThis);
    delete sThis;
    sThis = nullptr;
}

CellController *CellController::get()
{
    assert(sThis);
    return sThis;
}

Cell *CellController::getCell(ESM::Cell *esmCell)
{
    if (esmCell->isExterior())
        return getCellByXY(esmCell->mData.mX, esmCell->mData.mY);
    else
        return getCellByName(esmCell->mName);
}


Cell *CellController::getCellByXY(int x, int y)
{
    auto it = find_if (cells.begin(), cells.end(), [x, y](const Cell *c)
    {
        return c->cell.mData.mX == x && c->cell.mData.mY == y;
    });

    if (it == cells.end())
    {
        LOG_APPEND(Log::LOG_INFO, "- Attempt to get Cell at %i, %i failed!", x, y);
        return nullptr;
    }

    return *it;
}

Cell *CellController::getCellByName(std::string cellName)
{
    auto it = find_if (cells.begin(), cells.end(), [cellName](const Cell *c)
    {
        return c->cell.mName == cellName;
    });

    if (it == cells.end())
    {
        LOG_APPEND(Log::LOG_INFO, "- Attempt to get Cell at %s failed!", cellName.c_str());
        return nullptr;
    }

    return *it;
}

Cell *CellController::addCell(ESM::Cell cellData)
{
    LOG_APPEND(Log::LOG_INFO, "- Loaded cells: %d", cells.size());
    auto it = find_if (cells.begin(), cells.end(), [cellData](const Cell *c) {
        // Currently we cannot compare because plugin lists can be loaded in different order
        //return c->cell.sRecordId == cellData.sRecordId;
        if (c->cell.isExterior() && cellData.isExterior())
        {
            if (c->cell.mData.mX == cellData.mData.mX && c->cell.mData.mY == cellData.mData.mY)
                return true;
        }
        else if (c->cell.mName == cellData.mName)
            return true;

        return false;
    });

    Cell *cell;
    if (it == cells.end())
    {
        LOG_APPEND(Log::LOG_INFO, "- Adding %s to CellController", cellData.getDescription().c_str());

        cell = new Cell(cellData);
        cells.push_back(cell);
    }
    else
    {
        LOG_APPEND(Log::LOG_INFO, "- Found %s in CellController", cellData.getDescription().c_str());
        cell = *it;
    }

    return cell;
}

void CellController::removeCell(Cell *cell)
{
    if (cell == nullptr)
        return;

    for (auto it = cells.begin(); it != cells.end();)
    {
        if (*it != nullptr && *it == cell)
        {
            mwmp::Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_CELL_DELETION>(cell->getDescription());

            LOG_APPEND(Log::LOG_INFO, "- Removing %s from CellController", cell->getDescription().c_str());

            delete *it;
            it = cells.erase(it);
        }
        else
            ++it;
    }
}

void CellController::removePlayer(Cell *cell, Player *player)
{
    cell->removePlayer(player);

    if (cell->players.empty())
    {
        LOG_APPEND(Log::LOG_INFO, "- Cell %s has no players left", cell->getDescription().c_str());
        removeCell(cell);
    }
}

void CellController::deletePlayer(Player *player)
{
    LOG_APPEND(Log::LOG_INFO, "- Iterating through Cells from Player %s", player->npc.mName.c_str());

    for (auto it = player->getCells()->begin(); player->getCells()->size() != 0; ++it)
        removePlayer(*it, player);
}

void CellController::update(Player *player)
{
    for (auto cell : player->cellStateChanges.cellStates)
    {
        if (cell.type == mwmp::CellState::LOAD)
        {
            Cell *c = addCell(cell.cell);
            c->addPlayer(player);
        }
        else
        {
            Cell *c;
            if (!cell.cell.isExterior())
                c = getCellByName(cell.cell.mName);
            else
                c = getCellByXY(cell.cell.getGridX(), cell.cell.getGridY());

            if (c != nullptr)
                removePlayer(c, player);
        }
    }
}
