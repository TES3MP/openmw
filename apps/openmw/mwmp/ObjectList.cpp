#include "ObjectList.hpp"
#include "Main.hpp"
#include "Networking.hpp"
#include "MechanicsHelper.hpp"
#include "LocalPlayer.hpp"
#include "DedicatedPlayer.hpp"
#include "PlayerList.hpp"
#include "CellController.hpp"
#include "RecordHelper.hpp"

#include <components/openmw-mp/MWMPLog.hpp>

#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/soundmanager.hpp"
#include "../mwbase/windowmanager.hpp"

#include "../mwgui/container.hpp"
#include "../mwgui/inventorywindow.hpp"
#include "../mwgui/windowmanagerimp.hpp"

#include "../mwmechanics/aifollow.hpp"
#include "../mwmechanics/spellcasting.hpp"
#include "../mwmechanics/summoning.hpp"

#include "../mwrender/animation.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/containerstore.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/manualref.hpp"

using namespace mwmp;
using namespace std;

ObjectList::ObjectList()
{

}

ObjectList::~ObjectList()
{

}

Networking *ObjectList::getNetworking()
{
    return mwmp::Main::get().getNetworking();
}

void ObjectList::reset()
{
    cell.blank();
    baseObjects.clear();
    guid = mwmp::Main::get().getNetworking()->getLocalPlayer()->guid;

    action = -1;
    containerSubAction = 0;
}

void ObjectList::addObject(BaseObject baseObject)
{
    baseObjects.push_back(baseObject);
}

BaseObject ObjectList::getBaseObject(const MWWorld::Ptr& ptr)
{
    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = ptr.getCellRef().getMpNum();
    return baseObject;
}

void ObjectList::addContainerItem(mwmp::BaseObject& baseObject, const MWWorld::Ptr& itemPtr, int actionCount)
{
    mwmp::ContainerItem containerItem;
    containerItem.refId = itemPtr.getCellRef().getRefId();
    containerItem.count = itemPtr.getRefData().getCount();
    containerItem.charge = itemPtr.getCellRef().getCharge();
    containerItem.enchantmentCharge = itemPtr.getCellRef().getEnchantmentCharge();
    containerItem.soul = itemPtr.getCellRef().getSoul();
    containerItem.actionCount = actionCount;

    LOG_APPEND(MWMPLog::LOG_VERBOSE, "--- Adding container item %s", containerItem.refId.c_str());

    baseObject.containerItems.push_back(containerItem);
}

void ObjectList::addEntireContainer(const MWWorld::Ptr& ptr)
{
    LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Adding entire container %s %i-%i", ptr.getCellRef().getRefId().c_str(),
        ptr.getCellRef().getRefNum().mIndex, ptr.getCellRef().getMpNum());

    MWWorld::ContainerStore& containerStore = ptr.getClass().getContainerStore(ptr);

    mwmp::BaseObject baseObject = getBaseObject(ptr);

    for (const auto itemPtr : containerStore)
    {
        addContainerItem(baseObject, itemPtr, itemPtr.getRefData().getCount());
    }

    addObject(baseObject);
}

void ObjectList::editContainers(MWWorld::CellStore* cellStore)
{
    bool isLocalEvent = guid == Main::get().getLocalPlayer()->guid;

    LOG_APPEND(MWMPLog::LOG_VERBOSE, "- isLocalEvent? %s", isLocalEvent ? "true" : "false");

    BaseObject baseObject;

    for (unsigned int i = 0; i < baseObjectCount; i++)
    {
        baseObject = baseObjects.at(i);

        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- container cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            bool isCurrentContainer = false;
            bool hasActorEquipment = ptrFound.getClass().isActor() && ptrFound.getClass().hasInventoryStore(ptrFound);

            // If we are in a container, and it happens to be this container, keep track of that
            if (MWBase::Environment::get().getWindowManager()->containsMode(MWGui::GM_Container))
            {
                CurrentContainer *currentContainer = &mwmp::Main::get().getLocalPlayer()->currentContainer;

                if (currentContainer->refNum == ptrFound.getCellRef().getRefNum().mIndex &&
                    currentContainer->mpNum == ptrFound.getCellRef().getMpNum())
                {
                    isCurrentContainer = true;
                }
            }

            MWWorld::ContainerStore& containerStore = ptrFound.getClass().getContainerStore(ptrFound);

            // If we are setting the entire contents, clear the current ones
            if (action == BaseObjectList::SET)
                containerStore.clear();

            // bool isLocalDrop = isLocalEvent && containerSubAction == BaseObjectList::DROP;
            bool isLocalDrag = isLocalEvent && containerSubAction == BaseObjectList::DRAG;
            bool isLocalTakeAll = isLocalEvent && containerSubAction == BaseObjectList::TAKE_ALL;
            std::string takeAllSound = "";

            MWWorld::Ptr ownerPtr = ptrFound.getClass().isActor() ? ptrFound : MWBase::Environment::get().getWorld()->getPlayerPtr();

            for (const auto &containerItem : baseObject.containerItems)
            {
                //LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- containerItem cellRef: %s, count: %i, actionCount: %i",
                //    containerItem.refId.c_str(), containerItem.count, containerItem.actionCount);

                if (containerItem.refId.find("$dynamic") != string::npos)
                    continue;

                if (action == BaseObjectList::SET || action == BaseObjectList::ADD)
                {
                    // Create a ManualRef to be able to set item charge
                    MWWorld::ManualRef ref(MWBase::Environment::get().getWorld()->getStore(), containerItem.refId, 1);
                    MWWorld::Ptr newPtr = ref.getPtr();

                    if (containerItem.count > 1)
                        newPtr.getRefData().setCount(containerItem.count);

                    if (containerItem.charge > -1)
                        newPtr.getCellRef().setCharge(containerItem.charge);

                    if (containerItem.enchantmentCharge > -1)
                        newPtr.getCellRef().setEnchantmentCharge(containerItem.enchantmentCharge);

                    if (!containerItem.soul.empty())
                        newPtr.getCellRef().setSoul(containerItem.soul);

                    containerStore.add(newPtr, containerItem.count, ownerPtr, true);
                }

                else if (action == BaseObjectList::REMOVE && containerItem.actionCount > 0)
                {
                    // We have to find the right item ourselves because ContainerStore has no method
                    // accounting for charge
                    for (const auto itemPtr : containerStore)
                    {
                        if (Misc::StringUtils::ciEqual(itemPtr.getCellRef().getRefId(), containerItem.refId))
                        {
                            if (itemPtr.getCellRef().getCharge() == containerItem.charge &&
                                itemPtr.getCellRef().getEnchantmentCharge() == containerItem.enchantmentCharge &&
                                Misc::StringUtils::ciEqual(itemPtr.getCellRef().getSoul(), containerItem.soul))
                            {
                                // Store the sound of the first item in a TAKE_ALL
                                if (isLocalTakeAll && takeAllSound.empty())
                                    takeAllSound = itemPtr.getClass().getUpSoundId(itemPtr);

                                // Is this an actor's container? If so, unequip this item if it was equipped
                                if (hasActorEquipment)
                                {
                                    MWWorld::InventoryStore& invStore = ptrFound.getClass().getInventoryStore(ptrFound);

                                    if (invStore.isEquipped(itemPtr))
                                        invStore.unequipItemQuantity(itemPtr, ptrFound, containerItem.count);
                                }

                                bool isResolved = false;

                                if (isLocalDrag && isCurrentContainer)
                                {
                                    MWGui::ContainerWindow* containerWindow = MWBase::Environment::get().getWindowManager()->getContainerWindow();

                                    if (!containerWindow->isOnDragAndDrop())
                                    {
                                        isResolved = containerWindow->dragItemByPtr(itemPtr, containerItem.actionCount);
                                    }
                                }

                                if (!isResolved)
                                {
                                    containerStore.remove(itemPtr, containerItem.actionCount, ownerPtr);

                                    if (isLocalDrag || isLocalTakeAll)
                                    {
                                        MWWorld::Ptr ptrPlayer = MWBase::Environment::get().getWorld()->getPlayerPtr();
                                        MWWorld::ContainerStore &playerStore = ptrPlayer.getClass().getContainerStore(ptrPlayer);
                                        *playerStore.add(containerItem.refId, containerItem.actionCount, ptrPlayer);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Was this a SET or ADD action on an actor's container, and are we the authority
            // over the actor? If so, autoequip the actor
            if ((action == BaseObjectList::ADD || action == BaseObjectList::SET) && hasActorEquipment &&
                mwmp::Main::get().getCellController()->isLocalActor(ptrFound))
            {
                MWWorld::InventoryStore& invStore = ptrFound.getClass().getInventoryStore(ptrFound);

                if (ptrFound.getTypeName() == typeid(ESM::NPC).name())
                    invStore.autoEquip(ptrFound);
                // autoEquip only works on NPCs, so use the closest alternative for creatures
                else
                    invStore.autoEquip(ptrFound);
            }

            // If this container was open for us, update its view
            if (isCurrentContainer)
            {
                if (isLocalTakeAll)
                {
                    MWBase::Environment::get().getWindowManager()->removeGuiMode(MWGui::GM_Container);
                    MWBase::Environment::get().getWindowManager()->playSound(takeAllSound);
                }
                else
                {
                    MWGui::ContainerWindow* containerWindow = MWBase::Environment::get().getWindowManager()->getContainerWindow();
                    containerWindow->setPtr(ptrFound);
                }
            }
        }
    }
}

void ObjectList::activateObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        MWWorld::Ptr ptrFound;

        if (baseObject.isPlayer)
        {
            if (baseObject.guid == Main::get().getLocalPlayer()->guid)
            {
                ptrFound = Main::get().getLocalPlayer()->getPlayerPtr();
                LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Activated object is local player");
            }
            else
            {
                DedicatedPlayer *player = PlayerList::getPlayer(baseObject.guid);

                if (player != 0)
                {
                    ptrFound = player->getPtr();
                    LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Activated object is player %s", player->npc.mName.c_str());
                }
                else
                {
                    LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Could not find player to activatee!");
                }
            }
        }
        else
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Activated object is %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);
            ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);
        }

        if (ptrFound)
        {
            MWWorld::Ptr activatingActorPtr;

            if (baseObject.activatingActor.isPlayer)
            {
                activatingActorPtr = MechanicsHelper::getPlayerPtr(baseObject.activatingActor);
                LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Object has been activated by player %s",
                    activatingActorPtr.getClass().getName(activatingActorPtr).c_str());
            }
            else
            {
                activatingActorPtr = cellStore->searchExact(baseObject.activatingActor.refNum, baseObject.activatingActor.mpNum);
                LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Object has been activated by actor %s %i-%i", activatingActorPtr.getCellRef().getRefId().c_str(),
                    activatingActorPtr.getCellRef().getRefNum().mIndex, activatingActorPtr.getCellRef().getMpNum());
            }

            if (activatingActorPtr)
            {
                // Is an item that can be picked up being activated by the local player with their inventory open?
                if (activatingActorPtr == MWBase::Environment::get().getWorld()->getPlayerPtr() &&
                    (MWBase::Environment::get().getWindowManager()->getMode() == MWGui::GM_Container ||
                    MWBase::Environment::get().getWindowManager()->getMode() == MWGui::GM_Inventory))
                {
                    MWBase::Environment::get().getWindowManager()->getInventoryWindow()->pickUpObject(ptrFound);
                }
                else
                {
                    MWBase::Environment::get().getWorld()->activate(ptrFound, activatingActorPtr);
                }
            }
        }
    }
}

void ObjectList::placeObjects(MWWorld::CellStore* cellStore)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i, count: %i, charge: %i, enchantmentCharge: %.2f, soul: %s",
            baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum, baseObject.count, baseObject.charge,
            baseObject.enchantmentCharge, baseObject.soul.c_str());

        // Ignore generic dynamic refIds because they could be anything on other clients
        if (baseObject.refId.find("$dynamic") != string::npos)
            continue;

        MWWorld::Ptr ptrFound = cellStore->searchExact(0, baseObject.mpNum);

        // Only create this object if it doesn't already exist
        if (!ptrFound)
        {
            try
            {
                MWWorld::ManualRef ref(world->getStore(), baseObject.refId, 1);

                MWWorld::Ptr newPtr = ref.getPtr();

                if (baseObject.count > 1)
                    newPtr.getRefData().setCount(baseObject.count);

                if (baseObject.charge > -1)
                    newPtr.getCellRef().setCharge(baseObject.charge);

                if (baseObject.enchantmentCharge > -1)
                    newPtr.getCellRef().setEnchantmentCharge(baseObject.enchantmentCharge);

                if (!baseObject.soul.empty())
                    newPtr.getCellRef().setSoul(baseObject.soul);

                newPtr.getCellRef().setGoldValue(baseObject.goldValue);
                newPtr = world->placeObject(newPtr, cellStore, baseObject.position);

                // Because gold automatically gets replaced with a new object, make sure we set the mpNum at the end
                newPtr.getCellRef().setMpNum(baseObject.mpNum);

                if (guid == Main::get().getLocalPlayer()->guid && baseObject.droppedByPlayer)
                    world->PCDropped(newPtr);

            }
            catch (std::exception&)
            {
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, "Ignored placement of invalid object %s", baseObject.refId.c_str());
            }
        }
        else
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Object already existed!");
    }
}

void ObjectList::spawnObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(),
            baseObject.refNum, baseObject.mpNum);

        // Ignore generic dynamic refIds because they could be anything on other clients
        if (baseObject.refId.find("$dynamic") != string::npos)
            continue;
        else if (!RecordHelper::doesCreatureRecordExist(baseObject.refId) && !RecordHelper::doesNpcRecordExist(baseObject.refId))
        {
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, "Ignored spawning of invalid object %s", baseObject.refId.c_str());
            continue;
        }

        MWWorld::Ptr ptrFound = cellStore->searchExact(0, baseObject.mpNum);

        // Only create this object if it doesn't already exist
        if (!ptrFound)
        {
            MWWorld::ManualRef ref(MWBase::Environment::get().getWorld()->getStore(), baseObject.refId, 1);
            MWWorld::Ptr newPtr = ref.getPtr();

            newPtr.getCellRef().setMpNum(baseObject.mpNum);

            newPtr = MWBase::Environment::get().getWorld()->placeObject(newPtr, cellStore, baseObject.position);
            MWMechanics::CreatureStats& creatureStats = newPtr.getClass().getCreatureStats(newPtr);

            if (baseObject.isSummon)
            {
                MWWorld::Ptr masterPtr;

                if (baseObject.master.isPlayer)
                    masterPtr = MechanicsHelper::getPlayerPtr(baseObject.master);
                else
                    masterPtr = cellStore->searchExact(baseObject.master.refNum, baseObject.master.mpNum);

                if (masterPtr)
                {
                    LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Actor has master: %s", masterPtr.getCellRef().getRefId().c_str());

                    MWMechanics::AiFollow package(masterPtr);
                    creatureStats.getAiSequence().stack(package, newPtr);

                    MWRender::Animation* anim = MWBase::Environment::get().getWorld()->getAnimation(newPtr);
                    if (anim)
                    {
                        const ESM::Static* fx = MWBase::Environment::get().getWorld()->getStore().get<ESM::Static>()
                            .search("VFX_Summon_Start");
                        if (fx)
                            anim->addEffect("meshes\\" + fx->mModel, -1, false);
                    }

                    int creatureActorId = newPtr.getClass().getCreatureStats(newPtr).getActorId();

                    MWMechanics::CreatureStats& masterCreatureStats = masterPtr.getClass().getCreatureStats(masterPtr);
                    masterCreatureStats.setSummonedCreatureActorId(baseObject.refId, creatureActorId);
                }
            }
        }
        else
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Actor already existed!");
    }
}

void ObjectList::deleteObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            // If we are in a container, and it happens to be this object, exit it
            if (MWBase::Environment::get().getWindowManager()->containsMode(MWGui::GM_Container))
            {
                CurrentContainer *currentContainer = &mwmp::Main::get().getLocalPlayer()->currentContainer;

                if (currentContainer->refNum == ptrFound.getCellRef().getRefNum().mIndex &&
                    currentContainer->mpNum == ptrFound.getCellRef().getMpNum())
                {
                    MWBase::Environment::get().getWindowManager()->removeGuiMode(MWGui::GM_Container);
                    MWBase::Environment::get().getWindowManager()->setDragDrop(false);
                }
            }

            MWBase::Environment::get().getWorld()->deleteObject(ptrFound);
        }
    }
}

void ObjectList::lockObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            if (baseObject.lockLevel > 0)
                ptrFound.getCellRef().lock(baseObject.lockLevel);
            else
                ptrFound.getCellRef().unlock();
        }
    }
}

void ObjectList::triggerTrapObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            if (!baseObject.isDisarmed)
            {
                MWMechanics::CastSpell cast(ptrFound, ptrFound);
                cast.mHitPosition = baseObject.position.asVec3();
                cast.cast(ptrFound.getCellRef().getTrap());
            }

            ptrFound.getCellRef().setTrap("");
        }
    }
}

void ObjectList::scaleObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i, scale: %f", baseObject.refId.c_str(), baseObject.refNum,
            baseObject.mpNum, baseObject.scale);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            MWBase::Environment::get().getWorld()->scaleObject(ptrFound, baseObject.scale);
        }
    }
}

void ObjectList::setObjectStates(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i, state: %s", baseObject.refId.c_str(), baseObject.refNum,
            baseObject.mpNum, baseObject.objectState ? "true" : "false");

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            if (baseObject.objectState)
                MWBase::Environment::get().getWorld()->enable(ptrFound);
            else
                MWBase::Environment::get().getWorld()->disable(ptrFound);
        }
    }
}

void ObjectList::moveObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            MWBase::Environment::get().getWorld()->moveObject(ptrFound, baseObject.position.pos[0], baseObject.position.pos[1],
                                                              baseObject.position.pos[2]);
        }
    }
}

void ObjectList::rotateObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            MWBase::Environment::get().getWorld()->rotateObject(ptrFound,
                baseObject.position.rot[0], baseObject.position.rot[1], baseObject.position.rot[2]);
        }
    }
}

void ObjectList::animateObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            MWBase::MechanicsManager * mechanicsManager = MWBase::Environment::get().getMechanicsManager();
            mechanicsManager->playAnimationGroup(ptrFound, baseObject.animGroup, baseObject.animMode,
                                                 std::numeric_limits<int>::max(), true);
        }
    }
}

void ObjectList::activateDoors(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            ptrFound.getClass().setDoorState(ptrFound, static_cast<MWWorld::DoorState>(baseObject.doorState));
            MWBase::Environment::get().getWorld()->saveDoorState(ptrFound, static_cast<MWWorld::DoorState>(baseObject.doorState));
        }
    }
}

void ObjectList::setDoorDestinations(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            ptrFound.getCellRef().setTeleport(baseObject.teleportState);

            if (baseObject.teleportState)
            {
                ptrFound.getCellRef().setDoorDest(baseObject.destinationPosition);

                if (baseObject.destinationCell.isExterior())
                    ptrFound.getCellRef().setDestCell("");
                else
                    ptrFound.getCellRef().setDestCell(baseObject.destinationCell.getDescription());
            }
        }
    }
}

void ObjectList::runConsoleCommands(MWWorld::CellStore* cellStore)
{
    MWBase::WindowManager *windowManager = MWBase::Environment::get().getWindowManager();

    LOG_APPEND(MWMPLog::LOG_VERBOSE, "- Console command: %s", consoleCommand.c_str());

    if (baseObjects.empty())
    {
        windowManager->clearConsolePtr();

        LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Running with no object reference");
        windowManager->executeCommandInConsole(consoleCommand);
    }
    else
    {
        for (const auto &baseObject : baseObjects)
        {
            windowManager->clearConsolePtr();

            if (baseObject.isPlayer)
            {
                if (baseObject.guid == Main::get().getLocalPlayer()->guid)
                {
                    LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Running on local player");
                    windowManager->setConsolePtr(Main::get().getLocalPlayer()->getPlayerPtr());
                    windowManager->executeCommandInConsole(consoleCommand);
                }
                else
                {
                    DedicatedPlayer *player = PlayerList::getPlayer(baseObject.guid);

                    if (player != 0)
                    {
                        LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Running on player %s", player->npc.mName.c_str());
                        windowManager->setConsolePtr(player->getPtr());
                        windowManager->executeCommandInConsole(consoleCommand);
                    }
                }
            }
            else
            {
                LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Running on object %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

                MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

                if (ptrFound)
                {
                    LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                        ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

                    windowManager->setConsolePtr(ptrFound);
                    windowManager->executeCommandInConsole(consoleCommand);
                }
            }
        }

        windowManager->clearConsolePtr();
    }
}

void ObjectList::setLocalShorts(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i, index: %i, shortVal: %i", baseObject.refId.c_str(),
                   baseObject.refNum, baseObject.mpNum, baseObject.index, baseObject.shortVal);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            ptrFound.getRefData().getLocals().mShorts.at(baseObject.index) = baseObject.shortVal;
        }
    }
}

void ObjectList::setLocalFloats(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i, index: %i, floatVal: %f", baseObject.refId.c_str(),
                   baseObject.refNum, baseObject.mpNum, baseObject.index, baseObject.floatVal);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            ptrFound.getRefData().getLocals().mFloats.at(baseObject.index) = baseObject.floatVal;
        }
    }
}

void ObjectList::setMemberShorts()
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s, index: %i, shortVal: %i", baseObject.refId.c_str(),
                   baseObject.index, baseObject.shortVal);

        // Mimic the way a Ptr is fetched in InterpreterContext for similar situations
        MWWorld::Ptr ptrFound = MWBase::Environment::get().getWorld()->searchPtr(baseObject.refId, false);

        if (!ptrFound.isEmpty())
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            std::string scriptId = ptrFound.getClass().getScript(ptrFound);

            ptrFound.getRefData().setLocals(
                *MWBase::Environment::get().getWorld()->getStore().get<ESM::Script>().find(scriptId));

            ptrFound.getRefData().getLocals().mShorts.at(baseObject.index) = baseObject.shortVal;;
        }
    }
}

void ObjectList::setGlobalShorts()
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- varName: %s, shortVal: %i", baseObject.varName.c_str(), baseObject.shortVal);

        MWBase::Environment::get().getWorld()->setGlobalInt(baseObject.varName, baseObject.shortVal);
    }
}

void ObjectList::playMusic()
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- filename: %s", baseObject.musicFilename.c_str());

        MWBase::Environment::get().getSoundManager()->streamMusic(baseObject.musicFilename);
    }
}

void ObjectList::playVideo()
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- filename: %s, allowSkipping: %s", baseObject.videoFilename.c_str(),
            baseObject.allowSkipping ? "true" : "false");

        MWBase::Environment::get().getWindowManager()->playVideo(baseObject.videoFilename, baseObject.allowSkipping);
    }
}

void ObjectList::addAllContainers(MWWorld::CellStore* cellStore)
{
    for (auto &ref : cellStore->getContainers()->mList)
    {
        MWWorld::Ptr ptr(&ref, 0);
        addEntireContainer(ptr);
    }

    for (auto &ref : cellStore->getNpcs()->mList)
    {
        MWWorld::Ptr ptr(&ref, 0);
        addEntireContainer(ptr);
    }

    for (auto &ref : cellStore->getCreatures()->mList)
    {
        MWWorld::Ptr ptr(&ref, 0);
        addEntireContainer(ptr);
    }
}

void ObjectList::addRequestedContainers(MWWorld::CellStore* cellStore, const std::vector<BaseObject>& requestObjects)
{
    for (const auto &baseObject : requestObjects)
    {
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(),
            baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum);

        if (ptrFound)
        {
            if (ptrFound.getClass().hasContainerStore(ptrFound))
                addEntireContainer(ptrFound);
            else
                LOG_APPEND(MWMPLog::LOG_VERBOSE, "-- Object lacks container store", ptrFound.getCellRef().getRefId().c_str());
        }
    }
}

void ObjectList::addObjectActivate(const MWWorld::Ptr& ptr, const MWWorld::Ptr& activatingActor)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;

    if (ptr == MWBase::Environment::get().getWorld()->getPlayerPtr())
    {
        baseObject.isPlayer = true;
        baseObject.guid = mwmp::Main::get().getLocalPlayer()->guid;
    }
    else if (mwmp::PlayerList::isDedicatedPlayer(ptr))
    {
        baseObject.isPlayer = true;
        baseObject.guid = mwmp::PlayerList::getPlayer(ptr)->guid;
    }
    else
    {
        baseObject.isPlayer = false;
        baseObject.refId = ptr.getCellRef().getRefId();
        baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
        baseObject.mpNum = ptr.getCellRef().getMpNum();
    }

    baseObject.activatingActor = MechanicsHelper::getTarget(activatingActor);

    addObject(baseObject);
}

void ObjectList::addObjectPlace(const MWWorld::Ptr& ptr, bool droppedByPlayer)
{
    if (ptr.getCellRef().getRefId().find("$dynamic") != string::npos)
    {
        MWBase::Environment::get().getWindowManager()->messageBox("You cannot place unsynchronized custom items in multiplayer.");
        return;
    }

    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = 0;
    baseObject.charge = ptr.getCellRef().getCharge();
    baseObject.enchantmentCharge = ptr.getCellRef().getEnchantmentCharge();
    baseObject.soul = ptr.getCellRef().getSoul();
    baseObject.droppedByPlayer = droppedByPlayer;
    baseObject.hasContainer = ptr.getClass().hasContainerStore(ptr);

    // Make sure we send the RefData position instead of the CellRef one, because that's what
    // we actually see on this client
    baseObject.position = ptr.getRefData().getPosition();

    // We have to get the count from the dropped object because it gets changed
    // automatically for stacks of gold
    baseObject.count = ptr.getRefData().getCount();

    // Get the real count of gold in a stack
    baseObject.goldValue = ptr.getCellRef().getGoldValue();

    addObject(baseObject);
}

void ObjectList::addObjectSpawn(const MWWorld::Ptr& ptr)
{
    if (ptr.getCellRef().getRefId().find("$dynamic") != string::npos)
    {
        MWBase::Environment::get().getWindowManager()->messageBox("You're trying to spawn a custom object lacking a server-given refId, "
            "and those cannot be synchronized in multiplayer.");
        return;
    }

    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = 0;
    baseObject.isSummon = false;
    baseObject.summonDuration = -1;

    // Make sure we send the RefData position instead of the CellRef one, because that's what
    // we actually see on this client
    baseObject.position = ptr.getRefData().getPosition();

    addObject(baseObject);
}

void ObjectList::addObjectSpawn(const MWWorld::Ptr& ptr, const MWWorld::Ptr& master, float duration)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = 0;
    baseObject.isSummon = true;
    baseObject.summonDuration = duration;
    baseObject.master = MechanicsHelper::getTarget(master);

    // Make sure we send the RefData position instead of the CellRef one, because that's what
    // we actually see on this client
    baseObject.position = ptr.getRefData().getPosition();

    addObject(baseObject);
}

void ObjectList::addObjectDelete(const MWWorld::Ptr& ptr)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = ptr.getCellRef().getMpNum();
    addObject(baseObject);
}

void ObjectList::addObjectLock(const MWWorld::Ptr& ptr, int lockLevel)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = ptr.getCellRef().getMpNum();
    baseObject.lockLevel = lockLevel;
    addObject(baseObject);
}

void ObjectList::addObjectTrap(const MWWorld::Ptr& ptr, const ESM::Position& pos, bool isDisarmed)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = ptr.getCellRef().getMpNum();
    baseObject.isDisarmed = isDisarmed;
    baseObject.position = pos;
    addObject(baseObject);
}

void ObjectList::addObjectScale(const MWWorld::Ptr& ptr, float scale)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = ptr.getCellRef().getMpNum();
    baseObject.scale = scale;
    addObject(baseObject);
}

void ObjectList::addObjectState(const MWWorld::Ptr& ptr, bool objectState)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = ptr.getCellRef().getMpNum();
    baseObject.objectState = objectState;
    addObject(baseObject);
}

void ObjectList::addObjectAnimPlay(const MWWorld::Ptr& ptr, std::string group, int mode)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = ptr.getCellRef().getMpNum();
    baseObject.animGroup = group;
    baseObject.animMode = mode;
    addObject(baseObject);
}

void ObjectList::addDoorState(const MWWorld::Ptr& ptr, int state)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = ptr.getCellRef().getMpNum();
    baseObject.doorState = state;
    addObject(baseObject);
}

void ObjectList::addMusicPlay(std::string filename)
{
    mwmp::BaseObject baseObject;
    baseObject.musicFilename = filename;
    addObject(baseObject);
}

void ObjectList::addVideoPlay(std::string filename, bool allowSkipping)
{
    mwmp::BaseObject baseObject;
    baseObject.videoFilename = filename;
    baseObject.allowSkipping = allowSkipping;
    addObject(baseObject);
}

void ObjectList::addScriptLocalShort(const MWWorld::Ptr& ptr, int index, int shortVal)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = ptr.getCellRef().getMpNum();
    baseObject.index = index;
    baseObject.shortVal = shortVal;
    addObject(baseObject);
}

void ObjectList::addScriptLocalFloat(const MWWorld::Ptr& ptr, int index, float floatVal)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject;
    baseObject.refId = ptr.getCellRef().getRefId();
    baseObject.refNum = ptr.getCellRef().getRefNum().mIndex;
    baseObject.mpNum = ptr.getCellRef().getMpNum();
    baseObject.index = index;
    baseObject.floatVal = floatVal;
    addObject(baseObject);
}

void ObjectList::addScriptMemberShort(std::string refId, int index, int shortVal)
{
    mwmp::BaseObject baseObject;
    baseObject.refId = refId;
    baseObject.index = index;
    baseObject.shortVal = shortVal;
    addObject(baseObject);
}

void ObjectList::addScriptGlobalShort(std::string varName, int shortVal)
{
    mwmp::BaseObject baseObject;
    baseObject.varName = varName;
    baseObject.shortVal = shortVal;
    addObject(baseObject);
}

void ObjectList::sendObjectActivate()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_ACTIVATE)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_ACTIVATE)->Send();
}

void ObjectList::sendObjectPlace()
{
    if (baseObjects.size() == 0)
        return;

    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Sending ID_OBJECT_PLACE about %s", cell.getDescription().c_str());

    for (const auto &baseObject : baseObjects)
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s, count: %i", baseObject.refId.c_str(), baseObject.count);

    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_PLACE)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_PLACE)->Send();
}

void ObjectList::sendObjectSpawn()
{
    if (baseObjects.size() == 0)
        return;

    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Sending ID_OBJECT_SPAWN about %s", cell.getDescription().c_str());

    for (const auto &baseObject : baseObjects)
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s-%i", baseObject.refId.c_str(), baseObject.refNum);

    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_SPAWN)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_SPAWN)->Send();
}

void ObjectList::sendObjectDelete()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_DELETE)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_DELETE)->Send();
}

void ObjectList::sendObjectLock()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_LOCK)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_LOCK)->Send();
}

void ObjectList::sendObjectTrap()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_TRAP)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_TRAP)->Send();
}

void ObjectList::sendObjectScale()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_SCALE)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_SCALE)->Send();
}

void ObjectList::sendObjectState()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_STATE)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_STATE)->Send();
}

void ObjectList::sendObjectAnimPlay()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_ANIM_PLAY)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_ANIM_PLAY)->Send();
}

void ObjectList::sendDoorState()
{
    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Sending ID_DOOR_STATE about %s", cell.getDescription().c_str());

    for (const auto &baseObject : baseObjects)
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s-%i, state: %s", baseObject.refId.c_str(), baseObject.refNum,
                   baseObject.doorState ? "true" : "false");

    mwmp::Main::get().getNetworking()->getObjectPacket(ID_DOOR_STATE)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_DOOR_STATE)->Send();
}

void ObjectList::sendMusicPlay()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_MUSIC_PLAY)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_MUSIC_PLAY)->Send();
}

void ObjectList::sendVideoPlay()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_VIDEO_PLAY)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_VIDEO_PLAY)->Send();
}

void ObjectList::sendScriptLocalShort()
{
    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Sending ID_SCRIPT_LOCAL_SHORT about %s", cell.getDescription().c_str());

    for (const auto &baseObject : baseObjects)
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s-%i, index: %i, shortVal: %i", baseObject.refId.c_str(),
                   baseObject.refNum, baseObject.index, baseObject.shortVal);

    mwmp::Main::get().getNetworking()->getObjectPacket(ID_SCRIPT_LOCAL_SHORT)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_SCRIPT_LOCAL_SHORT)->Send();
}

void ObjectList::sendScriptLocalFloat()
{
    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Sending ID_SCRIPT_LOCAL_FLOAT about %s", cell.getDescription().c_str());

    for (const auto &baseObject : baseObjects)
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s-%i, index: %i, floatVal: %f", baseObject.refId.c_str(), 
                   baseObject.refNum, baseObject.index, baseObject.floatVal);

    mwmp::Main::get().getNetworking()->getObjectPacket(ID_SCRIPT_LOCAL_FLOAT)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_SCRIPT_LOCAL_FLOAT)->Send();
}

void ObjectList::sendScriptMemberShort()
{
    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Sending ID_SCRIPT_MEMBER_SHORT");

    for (const auto &baseObject : baseObjects)
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- cellRef: %s, index: %i, shortVal: %i", baseObject.refId.c_str(),
                   baseObject.index, baseObject.shortVal);

    mwmp::Main::get().getNetworking()->getObjectPacket(ID_SCRIPT_MEMBER_SHORT)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_SCRIPT_MEMBER_SHORT)->Send();
}

void ObjectList::sendScriptGlobalShort()
{
    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Sending ID_SCRIPT_GLOBAL_SHORT");

    for (const auto &baseObject : baseObjects)
        LOG_APPEND(MWMPLog::LOG_VERBOSE, "- varName: %s, shortVal: %i", baseObject.varName.c_str(), baseObject.shortVal);

    mwmp::Main::get().getNetworking()->getObjectPacket(ID_SCRIPT_GLOBAL_SHORT)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_SCRIPT_GLOBAL_SHORT)->Send();
}

void ObjectList::sendContainer()
{
    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Sending ID_CONTAINER");

    mwmp::Main::get().getNetworking()->getObjectPacket(ID_CONTAINER)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_CONTAINER)->Send();
}
