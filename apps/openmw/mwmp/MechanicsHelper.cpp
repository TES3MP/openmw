#include <components/openmw-mp/MWMPLog.hpp>

#include <components/misc/rng.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"

#include "../mwmechanics/creaturestats.hpp"
#include "../mwmechanics/combat.hpp"
#include "../mwmechanics/levelledlist.hpp"
#include "../mwmechanics/spellcasting.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/inventorystore.hpp"

#include "MechanicsHelper.hpp"
#include "Main.hpp"
#include "Networking.hpp"
#include "LocalPlayer.hpp"
#include "DedicatedPlayer.hpp"
#include "PlayerList.hpp"
#include "ObjectList.hpp"
#include "CellController.hpp"

using namespace mwmp;

osg::Vec3f MechanicsHelper::getLinearInterpolation(osg::Vec3f start, osg::Vec3f end, float percent)
{
    osg::Vec3f position(percent, percent, percent);

    return (start + osg::componentMultiply(position, (end - start)));
}

ESM::Position MechanicsHelper::getPositionFromVector(osg::Vec3f vector)
{
    ESM::Position position;
    position.pos[0] = vector.x();
    position.pos[1] = vector.y();
    position.pos[2] = vector.z();

    return position;
}

// Inspired by similar code in mwclass\creaturelevlist.cpp
void MechanicsHelper::spawnLeveledCreatures(MWWorld::CellStore* cellStore)
{
    MWWorld::CellRefList<ESM::CreatureLevList> *creatureLevList = cellStore->getCreatureLists();
    mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
    objectList->reset();
    objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;

    int spawnCount = 0;

    for (auto &lref : creatureLevList->mList)
    {
        MWWorld::Ptr ptr(&lref, cellStore);

        std::string id = MWMechanics::getLevelledItem(ptr.get<ESM::CreatureLevList>()->mBase, true);

        if (!id.empty())
        {
            const MWWorld::ESMStore& store = MWBase::Environment::get().getWorld()->getStore();
            MWWorld::ManualRef manualRef(store, id);
            manualRef.getPtr().getCellRef().setPosition(ptr.getCellRef().getPosition());
            MWWorld::Ptr placed = MWBase::Environment::get().getWorld()->placeObject(manualRef.getPtr(), ptr.getCell(),
                                                                                     ptr.getCellRef().getPosition());
            objectList->addObjectSpawn(placed);
            MWBase::Environment::get().getWorld()->deleteObject(placed);

            spawnCount++;
        }
    }

    if (spawnCount > 0)
        objectList->sendObjectSpawn();
}

bool MechanicsHelper::isUsingRangedWeapon(const MWWorld::Ptr& ptr)
{
    if (ptr.getClass().hasInventoryStore(ptr))
    {
        MWWorld::InventoryStore &inventoryStore = ptr.getClass().getInventoryStore(ptr);
        MWWorld::ContainerStoreIterator weaponSlot = inventoryStore.getSlot(
            MWWorld::InventoryStore::Slot_CarriedRight);

        if (weaponSlot != inventoryStore.end() && weaponSlot->getTypeName() == typeid(ESM::Weapon).name())
        {
            const ESM::Weapon* weaponRecord = weaponSlot->get<ESM::Weapon>()->mBase;

            if (weaponRecord->mData.mType >= ESM::Weapon::MarksmanBow)
                return true;
        }
    }

    return false;
}

Attack *MechanicsHelper::getLocalAttack(const MWWorld::Ptr& ptr)
{
    if (ptr == MWBase::Environment::get().getWorld()->getPlayerPtr())
        return &mwmp::Main::get().getLocalPlayer()->attack;
    else if (mwmp::Main::get().getCellController()->isLocalActor(ptr))
        return &mwmp::Main::get().getCellController()->getLocalActor(ptr)->attack;

    return nullptr;
}

Attack *MechanicsHelper::getDedicatedAttack(const MWWorld::Ptr& ptr)
{
    if (mwmp::PlayerList::isDedicatedPlayer(ptr))
        return &mwmp::PlayerList::getPlayer(ptr)->attack;
    else if (mwmp::Main::get().getCellController()->isDedicatedActor(ptr))
        return &mwmp::Main::get().getCellController()->getDedicatedActor(ptr)->attack;

    return nullptr;
}

MWWorld::Ptr MechanicsHelper::getPlayerPtr(const Target& target)
{
    if (target.guid == mwmp::Main::get().getLocalPlayer()->guid)
        return MWBase::Environment::get().getWorld()->getPlayerPtr();
    else if (PlayerList::getPlayer(target.guid) != nullptr)
        return PlayerList::getPlayer(target.guid)->getPtr();

    return nullptr;
}

mwmp::Item MechanicsHelper::getItem(const MWWorld::Ptr& itemPtr, int count)
{
    mwmp::Item item;

    if (itemPtr.getClass().isGold(itemPtr))
        item.refId = MWWorld::ContainerStore::sGoldId;
    else
        item.refId = itemPtr.getCellRef().getRefId();

    item.count = count;
    item.charge = itemPtr.getCellRef().getCharge();
    item.enchantmentCharge = itemPtr.getCellRef().getEnchantmentCharge();
    item.soul = itemPtr.getCellRef().getSoul();

    return item;
}

mwmp::Target MechanicsHelper::getTarget(const MWWorld::Ptr& ptr)
{
    mwmp::Target target;
    clearTarget(target);

    if (ptr != nullptr)
    {
        if (ptr == MWBase::Environment::get().getWorld()->getPlayerPtr())
        {
            target.isPlayer = true;
            target.guid = mwmp::Main::get().getLocalPlayer()->guid;
        }
        else if (mwmp::PlayerList::isDedicatedPlayer(ptr))
        {
            target.isPlayer = true;
            target.guid = mwmp::PlayerList::getPlayer(ptr)->guid;
        }
        else
        {
            MWWorld::CellRef *ptrRef = &ptr.getCellRef();

            if (ptrRef)
            {
                target.isPlayer = false;
                target.refId = ptrRef->getRefId();
                target.refNum = ptrRef->getRefNum().mIndex;
                target.mpNum = ptrRef->getMpNum();
                target.name = ptr.getClass().getName(ptr);
            }
        }
    }

    return target;
}

void MechanicsHelper::clearTarget(mwmp::Target& target)
{
    target.isPlayer = false;
    target.refId.clear();
    target.refNum = -1;
    target.mpNum = -1;

    target.name.clear();
}

bool MechanicsHelper::isEmptyTarget(const mwmp::Target& target)
{
    if (target.isPlayer == false && target.refId.empty())
        return true;

    return false;
}

void MechanicsHelper::assignAttackTarget(Attack* attack, const MWWorld::Ptr& target)
{
    if (target == MWBase::Environment::get().getWorld()->getPlayerPtr())
    {
        attack->target.isPlayer = true;
        attack->target.guid = mwmp::Main::get().getLocalPlayer()->guid;
    }
    else if (mwmp::PlayerList::isDedicatedPlayer(target))
    {
        attack->target.isPlayer = true;
        attack->target.guid = mwmp::PlayerList::getPlayer(target)->guid;
    }
    else
    {
        MWWorld::CellRef *targetRef = &target.getCellRef();

        attack->target.isPlayer = false;
        attack->target.refId = targetRef->getRefId();
        attack->target.refNum = targetRef->getRefNum().mIndex;
        attack->target.mpNum = targetRef->getMpNum();
    }
}

void MechanicsHelper::resetAttack(Attack* attack)
{
    attack->isHit = false;
    attack->success = false;
    attack->knockdown = false;
    attack->block = false;
    attack->applyWeaponEnchantment = false;
    attack->applyAmmoEnchantment = false;
    attack->hitPosition.pos[0] = attack->hitPosition.pos[1] = attack->hitPosition.pos[2] = 0;
    attack->target.guid = RakNet::RakNetGUID();
    attack->target.refId.clear();
    attack->target.refNum = 0;
    attack->target.mpNum = 0;
}

bool MechanicsHelper::getSpellSuccess(std::string spellId, const MWWorld::Ptr& caster)
{
    return Misc::Rng::roll0to99() < MWMechanics::getSpellSuccessChance(spellId, caster);
}

void MechanicsHelper::processAttack(Attack attack, const MWWorld::Ptr& attacker)
{
    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Processing attack from %s of type %i",
        attacker.getClass().getName(attacker).c_str(), attack.type);

    if (!attack.pressed)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- success: %s", attack.success ? "true" : "false");

        if (attack.success)
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "- damage: %f", attack.damage);
    }

    LOG_APPEND(MWMPLog::LOG_VERBOSE, "- pressed: %s", attack.pressed ? "true" : "false");

    MWMechanics::CreatureStats &attackerStats = attacker.getClass().getCreatureStats(attacker);
    MWWorld::Ptr victim;

    if (attack.target.isPlayer)
    {
        if (attack.target.guid == mwmp::Main::get().getLocalPlayer()->guid)
            victim = MWBase::Environment::get().getWorld()->getPlayerPtr();
        else if (PlayerList::getPlayer(attack.target.guid) != nullptr)
            victim = PlayerList::getPlayer(attack.target.guid)->getPtr();
    }
    else
    {
        auto controller = mwmp::Main::get().getCellController();
        if (controller->isLocalActor(attack.target.refNum, attack.target.mpNum))
            victim = controller->getLocalActor(attack.target.refNum, attack.target.mpNum)->getPtr();
        else if (controller->isDedicatedActor(attack.target.refNum, attack.target.mpNum))
            victim = controller->getDedicatedActor(attack.target.refNum, attack.target.mpNum)->getPtr();
    }

    if (attack.isHit && (attack.type == attack.MELEE || attack.type == attack.RANGED))
    {
        bool isRanged = attack.type == attack.RANGED;

        MWWorld::Ptr weaponPtr;
        MWWorld::Ptr ammoPtr;

        bool usedTempRangedWeapon = false;
        bool usedTempRangedAmmo = false;

        // Get the attacker's current weapon
        //
        // Note: if using hand-to-hand, the weapon is equal to inv.end()
        if (attacker.getClass().hasInventoryStore(attacker))
        {
            MWWorld::InventoryStore &inventoryStore = attacker.getClass().getInventoryStore(attacker);
            MWWorld::ContainerStoreIterator weaponSlot = inventoryStore.getSlot(
                MWWorld::InventoryStore::Slot_CarriedRight);

            weaponPtr = weaponSlot != inventoryStore.end() ? *weaponSlot : MWWorld::Ptr();

            // Is the currently selected weapon different from the one recorded for this ranged attack?
            // If so, try to find the correct one in the attacker's inventory and use it here. If it
            // no longer exists, add it back temporarily.
            if (isRanged)
            {
                if (!weaponPtr || !Misc::StringUtils::ciEqual(weaponPtr.getCellRef().getRefId(), attack.rangedWeaponId))
                {
                    weaponPtr = inventoryStore.search(attack.rangedWeaponId);

                    if (!weaponPtr)
                    {
                        weaponPtr = *attacker.getClass().getContainerStore(attacker).add(attack.rangedWeaponId, 1, attacker);
                        usedTempRangedWeapon = true;
                    }
                }

                if (!attack.rangedAmmoId.empty())
                {
                    MWWorld::ContainerStoreIterator ammoSlot = inventoryStore.getSlot(
                        MWWorld::InventoryStore::Slot_Ammunition);
                    ammoPtr = ammoSlot != inventoryStore.end() ? *ammoSlot : MWWorld::Ptr();

                    if (!ammoPtr || !Misc::StringUtils::ciEqual(ammoPtr.getCellRef().getRefId(), attack.rangedAmmoId))
                    {
                        ammoPtr = inventoryStore.search(attack.rangedAmmoId);

                        if (!ammoPtr)
                        {
                            ammoPtr = *attacker.getClass().getContainerStore(attacker).add(attack.rangedAmmoId, 1, attacker);
                            usedTempRangedAmmo = true;
                        }
                    }
                }
            }

            if (!weaponPtr.isEmpty() && weaponPtr.getTypeName() != typeid(ESM::Weapon).name())
                weaponPtr = MWWorld::Ptr();
        }

        if (!weaponPtr.isEmpty())
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "- weapon: %s\n- isRanged: %s\n- applyWeaponEnchantment: %s\n- applyAmmoEnchantment: %s",
                weaponPtr.getCellRef().getRefId().c_str(), isRanged ? "true" : "false", attack.applyWeaponEnchantment ? "true" : "false",
                attack.applyAmmoEnchantment ? "true" : "false");

            if (attack.applyWeaponEnchantment)
            {
                MWMechanics::CastSpell cast(attacker, victim, isRanged);
                cast.mHitPosition = attack.hitPosition.asVec3();
                cast.cast(weaponPtr, false);
            }

            if (isRanged && !ammoPtr.isEmpty() && attack.applyAmmoEnchantment)
            {
                MWMechanics::CastSpell cast(attacker, victim, isRanged);
                cast.mHitPosition = attack.hitPosition.asVec3();
                cast.cast(ammoPtr, false);
            }
        }

        if (victim.mRef != nullptr)
        {
            bool isHealthDamage = true;

            if (weaponPtr.isEmpty())
            {
                if (attacker.getClass().isBipedal(attacker))
                {
                    MWMechanics::CreatureStats &victimStats = victim.getClass().getCreatureStats(victim);
                    isHealthDamage = victimStats.isParalyzed() || victimStats.getKnockedDown();
                }
            }

            if (!isRanged)
                MWMechanics::blockMeleeAttack(attacker, victim, weaponPtr, attack.damage, 1);

            victim.getClass().onHit(victim, attack.damage, isHealthDamage, weaponPtr, attacker, attack.hitPosition.asVec3(),
                attack.success);
        }

        // Remove temporary items that may have been added above for ranged attacks
        if (isRanged && attacker.getClass().hasInventoryStore(attacker))
        {
            MWWorld::InventoryStore &inventoryStore = attacker.getClass().getInventoryStore(attacker);

            if (usedTempRangedWeapon)
                inventoryStore.remove(weaponPtr, 1, attacker);
            
            if (usedTempRangedAmmo)
                inventoryStore.remove(ammoPtr, 1, attacker);
        }
    }
    else if (attack.type == attack.MAGIC)
    {
        attackerStats.getSpells().setSelectedSpell(attack.spellId);

        if (attack.instant)
        {
            MWBase::Environment::get().getWorld()->castSpell(attacker);
            attack.instant = false;
        }

        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- spellId: %s", attack.spellId.c_str());
    }
    else if (attack.type == attack.ITEM_MAGIC)
    {
        attackerStats.getSpells().setSelectedSpell("");

        MWWorld::InventoryStore& inventoryStore = attacker.getClass().getInventoryStore(attacker);

        MWWorld::ContainerStoreIterator it = inventoryStore.begin();
        for (; it != inventoryStore.end(); ++it)
        {
            if (Misc::StringUtils::ciEqual(it->getCellRef().getRefId(), attack.itemId))
                break;
        }

        // Add the item if it's missing
        if (it == inventoryStore.end())
            it = attacker.getClass().getContainerStore(attacker).add(attack.itemId, 1, attacker);

        inventoryStore.setSelectedEnchantItem(it);
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- itemId: %s", attack.itemId.c_str());
        MWBase::Environment::get().getWorld()->castSpell(attacker);
        inventoryStore.setSelectedEnchantItem(inventoryStore.end());
    }
}

bool MechanicsHelper::doesEffectListContainEffect(const ESM::EffectList& effectList, short effectId, short attributeId, short skillId)
{
    for (const auto &effect : effectList.mList)
    {
        if (effect.mEffectID == effectId)
        {
            if (attributeId == -1 || effect.mAttribute == attributeId)
            {
                if (skillId == -1 || effect.mSkill == skillId)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void MechanicsHelper::unequipItemsByEffect(const MWWorld::Ptr& ptr, short enchantmentType, short effectId, short attributeId, short skillId)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();
    MWWorld::InventoryStore &ptrInventory = ptr.getClass().getInventoryStore(ptr);

    for (int slot = 0; slot < MWWorld::InventoryStore::Slots; slot++)
    {
        if (ptrInventory.getSlot(slot) != ptrInventory.end())
        {
            MWWorld::ConstContainerStoreIterator itemIterator = ptrInventory.getSlot(slot);
            std::string enchantmentName = itemIterator->getClass().getEnchantment(*itemIterator);

            if (!enchantmentName.empty())
            {
                const ESM::Enchantment* enchantment = world->getStore().get<ESM::Enchantment>().find(enchantmentName);

                if (enchantment->mData.mType == enchantmentType && doesEffectListContainEffect(enchantment->mEffects, effectId, attributeId, skillId))
                    ptrInventory.unequipSlot(slot, ptr);
            }
        }
    }
}

MWWorld::Ptr MechanicsHelper::getItemPtrFromStore(const mwmp::Item& item, MWWorld::ContainerStore& store)
{
    MWWorld::Ptr closestPtr;

    for (MWWorld::ContainerStoreIterator storeIterator = store.begin(); storeIterator != store.end(); ++storeIterator)
    {
        // Enchantment charges are often in the process of refilling themselves, so don't check for them here
        if (Misc::StringUtils::ciEqual(item.refId, storeIterator->getCellRef().getRefId()) &&
            item.count == storeIterator->getRefData().getCount() &&
            item.charge == storeIterator->getCellRef().getCharge() &&
            Misc::StringUtils::ciEqual(item.soul, storeIterator->getCellRef().getSoul()))
        {
            // If we have no closestPtr, set it to the Ptr corresponding to this storeIterator; otherwise, make
            // sure the storeIterator's enchantmentCharge is closer to our goal than that of the previous closestPtr
            if (!closestPtr || abs(storeIterator->getCellRef().getEnchantmentCharge() - item.enchantmentCharge) <
                abs(closestPtr.getCellRef().getEnchantmentCharge() - item.enchantmentCharge))
            {
                closestPtr = *storeIterator;
            }
        }
    }

    return closestPtr;
}
