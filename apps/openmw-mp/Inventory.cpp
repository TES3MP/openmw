//
// Created by koncord on 12.08.17.
//

#include "Inventory.hpp"
#include <components/openmw-mp/NetworkMessages.hpp>
#include <apps/openmw-mp/Script/LuaState.hpp>
#include <apps/openmw/mwworld/inventorystore.hpp>
#include <components/misc/stringops.hpp>
#include "NetActor.hpp"
#include "Networking.hpp"

using namespace std;

void Inventory::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Inventory>("Inventory",
                                            "getInventoryChangesSize", &Inventory::getChangesSize,
                                            "addItem", &Inventory::addItem,
                                            "removeItem", &Inventory::removeItem,
                                            "getInventoryItem", &Inventory::getInventoryItem,

                                            "equipItem", &Inventory::equipItem,
                                            "unequipItem", &Inventory::unequipItem,
                                            "hasItemEquipped", &Inventory::hasItemEquipped,
                                            "getEquipmentItem", &Inventory::getEquipmentItem


    );
}

Inventory::Inventory(NetActor *actor) : netActor(actor), equipmentChanged(false), inventoryChanged(0)
{
    printf("Inventory::Inventory()\n");
}

Inventory::~Inventory()
{
    printf("Inventory::~Inventory()\n");
}

void Inventory::update()
{
    printf("Inventory::update()");
    /*if (equipmentChanged)
    {
        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_EQUIPMENT);
        packet->setPlayer(netActor->getNetCreature());
        packet->Send(false);
        packet->Send(true);
    }

    if (inventoryChanged != 0)
    {
        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_EQUIPMENT);
        packet->setPlayer(netActor->getNetCreature());
        packet->Send(false);
    }

    equipmentChanged = false;
    inventoryChanged = 0;*/
}


void Inventory::InitializeInventoryChanges()
{
    netActor->getNetCreature()->inventoryChanges.items.clear();
    netActor->getNetCreature()->inventoryChanges.action = mwmp::InventoryChanges::SET;
}

int Inventory::getChangesSize() const
{
    return netActor->getNetCreature()->inventoryChanges.count;
}

void Inventory::equipItem(unsigned short slot, const std::string& refId, unsigned int count, int charge)
{
    netActor->getNetCreature()->equipedItems[slot].refId = refId;
    netActor->getNetCreature()->equipedItems[slot].count = count;
    netActor->getNetCreature()->equipedItems[slot].charge = charge;
    equipmentChanged = true;
}

void Inventory::unequipItem( unsigned short slot)
{
    equipItem(slot, "", 0, -1);
}


void Inventory::addItem(const std::string &refId, unsigned int count, int charge)
{
    if (inventoryChanged == mwmp::InventoryChanges::REMOVE)
        return;
    if (inventoryChanged == 0)
        InitializeInventoryChanges();

    mwmp::Item item;
    item.refId = refId;
    item.count = count;
    item.charge = charge;

    netActor->getNetCreature()->inventoryChanges.items.push_back(item);
    netActor->getNetCreature()->inventoryChanges.action = mwmp::InventoryChanges::ADD;
    inventoryChanged = netActor->getNetCreature()->inventoryChanges.action;
}

void Inventory::removeItem(const std::string &refId, unsigned short count)
{
    if (inventoryChanged == mwmp::InventoryChanges::ADD)
        return;
    if (inventoryChanged == 0)
        InitializeInventoryChanges();

    mwmp::Item item;
    item.refId = refId;
    item.count = count;

    netActor->getNetCreature()->inventoryChanges.items.push_back(item);
    netActor->getNetCreature()->inventoryChanges.action = mwmp::InventoryChanges::REMOVE;
    inventoryChanged = netActor->getNetCreature()->inventoryChanges.action;
}

bool Inventory::hasItemEquipped(const std::string &refId) const
{
    for (int slot = 0; slot < MWWorld::InventoryStore::Slots; slot++)
        if (Misc::StringUtils::ciEqual(netActor->getNetCreature()->equipedItems[slot].refId, refId))
            return true;
    return false;
}

std::tuple<std::string, int, int> Inventory::getEquipmentItem(unsigned short slot) const
{
    const auto &item = netActor->getNetCreature()->equipedItems[slot];
    return make_tuple(item.refId, item.count, item.charge);
}

std::tuple<std::string, int, int> Inventory::getInventoryItem(unsigned int slot) const
{
    const auto &item = netActor->getNetCreature()->inventoryChanges.items.at(slot);
    return make_tuple(item.refId, item.count, item.charge);
}

void Inventory::resetEquipmentFlag()
{
    equipmentChanged = false;
}

bool Inventory::isEquipmentChanged()
{
    return equipmentChanged;
}

void Inventory::resetInventoryFlag()
{
    inventoryChanged = 0;
}

int Inventory::inventoryChangeType()
{
    return inventoryChanged;
}



