#include "ObjectList.hpp"
#include "Main.hpp"
#include "Networking.hpp"
#include "MechanicsHelper.hpp"
#include "LocalPlayer.hpp"
#include "DedicatedPlayer.hpp"
#include "PlayerList.hpp"
#include "CellController.hpp"
#include "RecordHelper.hpp"

#include <components/translation/translation.hpp>
#include <components/openmw-mp/TimedLog.hpp>

#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/scriptmanager.hpp"
#include "../mwbase/soundmanager.hpp"
#include "../mwbase/windowmanager.hpp"

#include "../mwgui/container.hpp"
#include "../mwgui/dialogue.hpp"
#include "../mwgui/inventorywindow.hpp"
#include "../mwgui/windowmanagerimp.hpp"

#include "../mwmechanics/aifollow.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwmechanics/spellcasting.hpp"
#include "../mwmechanics/summoning.hpp"

#include "../mwrender/animation.hpp"

#include "../mwscript/interpretercontext.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/containerstore.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/manualref.hpp"
#include "../mwworld/timestamp.hpp"

using namespace mwmp;

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

void ObjectList::addBaseObject(BaseObject baseObject)
{
    baseObjects.push_back(baseObject);
}

mwmp::BaseObject ObjectList::getBaseObjectFromPtr(const MWWorld::Ptr& ptr)
{
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

    return baseObject;
}

void ObjectList::addContainerItem(mwmp::BaseObject& baseObject, const MWWorld::Ptr& itemPtr, int itemCount, int actionCount)
{
    mwmp::ContainerItem containerItem;
    containerItem.refId = itemPtr.getCellRef().getRefId();
    containerItem.count = itemCount;
    containerItem.charge = itemPtr.getCellRef().getCharge();
    containerItem.enchantmentCharge = itemPtr.getCellRef().getEnchantmentCharge();
    containerItem.soul = itemPtr.getCellRef().getSoul();
    containerItem.actionCount = actionCount;

    LOG_APPEND(TimedLog::LOG_VERBOSE, "--- Adding container item %s to packet with count %i and actionCount %i",
        containerItem.refId.c_str(), itemCount, actionCount);

    baseObject.containerItems.push_back(containerItem);
}

void ObjectList::addContainerItem(mwmp::BaseObject& baseObject, const MWGui::ItemStack& itemStack, int itemCount, int actionCount)
{
    mwmp::ContainerItem containerItem;
    containerItem.refId = itemStack.mBase.getCellRef().getRefId();
    containerItem.count = itemCount;
    containerItem.charge = itemStack.mBase.getCellRef().getCharge();
    containerItem.enchantmentCharge = itemStack.mBase.getCellRef().getEnchantmentCharge();
    containerItem.soul = itemStack.mBase.getCellRef().getSoul();
    containerItem.actionCount = actionCount;

    LOG_APPEND(TimedLog::LOG_VERBOSE, "--- Adding container item %s to packet with count %i and actionCount %i",
        containerItem.refId.c_str(), itemCount, actionCount);

    baseObject.containerItems.push_back(containerItem);
}

void ObjectList::addContainerItem(mwmp::BaseObject& baseObject, const std::string itemId, int itemCount, int actionCount)
{
    mwmp::ContainerItem containerItem;
    containerItem.refId = itemId;
    containerItem.count = itemCount;
    containerItem.charge = -1;
    containerItem.enchantmentCharge = -1;
    containerItem.soul = "";
    containerItem.actionCount = actionCount;

    LOG_APPEND(TimedLog::LOG_VERBOSE, "--- Adding container item %s to packet with count %i and actionCount %i",
        containerItem.refId.c_str(), itemCount, actionCount);

    baseObject.containerItems.push_back(containerItem);
}

void ObjectList::addEntireContainer(const MWWorld::Ptr& ptr)
{
    LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Adding entire container %s %i-%i", ptr.getCellRef().getRefId().c_str(),
        ptr.getCellRef().getRefNum().mIndex, ptr.getCellRef().getMpNum());

    MWWorld::ContainerStore& containerStore = ptr.getClass().getContainerStore(ptr);

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);

    // If the container store has not been populated with items yet, handle that now
    if (!containerStore.isResolved())
        containerStore.resolve();

    for (const auto itemPtr : containerStore)
    {
        addContainerItem(baseObject, itemPtr, itemPtr.getRefData().getCount(), itemPtr.getRefData().getCount());
    }

    addBaseObject(baseObject);
}

void ObjectList::editContainers(MWWorld::CellStore* cellStore)
{
    bool isLocalEvent = guid == Main::get().getLocalPlayer()->guid;

    LOG_APPEND(TimedLog::LOG_VERBOSE, "- isLocalEvent? %s", isLocalEvent ? "true" : "false");

    BaseObject baseObject;

    for (unsigned int i = 0; i < baseObjectCount; i++)
    {
        baseObject = baseObjects.at(i);

        LOG_APPEND(TimedLog::LOG_VERBOSE, "- container %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
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
            {
                containerStore.setResolved(true);
                containerStore.clear();
            }

            bool isLocalDrag = isLocalEvent && containerSubAction == BaseObjectList::DRAG;
            bool isLocalTakeAll = isLocalEvent && containerSubAction == BaseObjectList::TAKE_ALL;
            std::string takeAllSound = "";

            MWWorld::Ptr ownerPtr = ptrFound.getClass().isActor() ? ptrFound : MWBase::Environment::get().getWorld()->getPlayerPtr();

            for (const auto &containerItem : baseObject.containerItems)
            {
                //LOG_APPEND(TimedLog::LOG_VERBOSE, "-- containerItem %s, count: %i, actionCount: %i",
                //    containerItem.refId.c_str(), containerItem.count, containerItem.actionCount);

                if (containerItem.refId.find("$dynamic") != std::string::npos)
                    continue;

                if (action == BaseObjectList::SET || action == BaseObjectList::ADD)
                {
                    try
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

                        containerStore.add(newPtr, containerItem.count, ownerPtr);
                    }
                    catch (std::exception&)
                    {
                        LOG_APPEND(TimedLog::LOG_ERROR, "-- Ignored invalid container item %s", containerItem.refId.c_str());
                    }
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

                                bool isDragResolved = false;

                                if (isLocalDrag && isCurrentContainer)
                                {
                                    MWGui::ContainerWindow* containerWindow = MWBase::Environment::get().getWindowManager()->getContainerWindow();

                                    if (!containerWindow->isOnDragAndDrop())
                                    {
                                        isDragResolved = containerWindow->dragItemByPtr(itemPtr, containerItem.actionCount);
                                    }
                                }

                                if (!isLocalDrag || !isDragResolved)
                                {
                                    containerStore.remove(itemPtr, containerItem.actionCount, ownerPtr);

                                    if (isLocalDrag || isLocalTakeAll)
                                    {
                                        MWWorld::Ptr ptrPlayer = MWBase::Environment::get().getWorld()->getPlayerPtr();
                                        MWWorld::ContainerStore &playerStore = ptrPlayer.getClass().getContainerStore(ptrPlayer);
                                        *playerStore.add(itemPtr, containerItem.actionCount, ownerPtr, false);
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
                invStore.autoEquip(ptrFound);
                mwmp::Main::get().getCellController()->getLocalActor(ptrFound)->updateEquipment(true, true);
            }

            // If this container can be harvested, disable and then enable it again to refresh its animation
            if (ptrFound.getClass().canBeHarvested(ptrFound))
            {
                MWBase::Environment::get().getWorld()->disable(ptrFound);
                MWBase::Environment::get().getWorld()->enable(ptrFound);
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
                LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Activated object is local player");
            }
            else
            {
                DedicatedPlayer *player = PlayerList::getPlayer(baseObject.guid);

                if (player != 0)
                {
                    ptrFound = player->getPtr();
                    LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Activated object is player %s", player->npc.mName.c_str());
                }
                else
                {
                    LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Could not find player to activate!");
                }
            }
        }
        else
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Activated object is %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);
            ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);
        }

        if (ptrFound)
        {
            MWWorld::Ptr activatingActorPtr;

            if (baseObject.activatingActor.isPlayer)
            {
                activatingActorPtr = MechanicsHelper::getPlayerPtr(baseObject.activatingActor);
                LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Object has been activated by player %s",
                    activatingActorPtr.getClass().getName(activatingActorPtr).c_str());
            }
            else
            {
                activatingActorPtr = cellStore->searchExact(baseObject.activatingActor.refNum, baseObject.activatingActor.mpNum, baseObject.activatingActor.refId);
                LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Object has been activated by actor %s %i-%i", activatingActorPtr.getCellRef().getRefId().c_str(),
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
    
    static mwmp::ObjectList soundObjectList;
    soundObjectList.reset();
    soundObjectList.packetOrigin = mwmp::CLIENT_GAMEPLAY;
    soundObjectList.cell = cell;

    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i, count: %i, charge: %i, enchantmentCharge: %.2f, soul: %s",
            baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum, baseObject.count, baseObject.charge,
            baseObject.enchantmentCharge, baseObject.soul.c_str());

        // Ignore generic dynamic refIds because they could be anything on other clients
        if (baseObject.refId.find("$dynamic") != std::string::npos)
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

                if (baseObject.droppedByPlayer)
                {
                    if (guid == Main::get().getLocalPlayer()->guid)
                    {
                        world->PCDropped(newPtr);

                        // If this item was dropped locally, queue item drop sound
                        soundObjectList.addObjectSound(newPtr, newPtr.getClass().getDownSoundId(newPtr), 1.0, 1.0);
                    }
                }
            }
            catch (std::exception&)
            {
                LOG_MESSAGE_SIMPLE(TimedLog::LOG_ERROR, "Ignored placement of invalid object %s", baseObject.refId.c_str());
            }
        }
        else
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Object already existed!");
    }

    if (soundObjectList.baseObjects.size() > 0)
    {
        soundObjectList.sendObjectSound();
    }
}

void ObjectList::spawnObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(),
            baseObject.refNum, baseObject.mpNum);

        // Ignore generic dynamic refIds because they could be anything on other clients
        if (baseObject.refId.find("$dynamic") != std::string::npos)
            continue;
        else if (!RecordHelper::doesRecordIdExist<ESM::Creature>(baseObject.refId) && !RecordHelper::doesRecordIdExist<ESM::NPC>(baseObject.refId))
        {
            LOG_MESSAGE_SIMPLE(TimedLog::LOG_ERROR, "Ignored spawning of invalid object %s", baseObject.refId.c_str());
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
                    masterPtr = cellStore->searchExact(baseObject.master.refNum, baseObject.master.mpNum, baseObject.master.refId);

                if (masterPtr)
                {
                    LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Actor has master: %s", masterPtr.getCellRef().getRefId().c_str());

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

                    std::vector<ESM::ActiveEffect> activeEffects;
                    ESM::ActiveEffect activeEffect;
                    activeEffect.mEffectId = baseObject.summonEffectId;
                    activeEffect.mDuration = baseObject.summonDuration;
                    activeEffect.mMagnitude = 1;
                    activeEffects.push_back(activeEffect);

                    LOG_APPEND(TimedLog::LOG_INFO, "-- adding active spell to master with id %s, effect %i, duration %f",
                        baseObject.summonSpellId.c_str(), baseObject.summonEffectId, baseObject.summonDuration);

                    auto activeSpells = masterCreatureStats.getActiveSpells();
                    if (!activeSpells.isSpellActive(baseObject.summonSpellId))
                        activeSpells.addSpell(baseObject.summonSpellId, false, activeEffects, "", masterCreatureStats.getActorId());

                    LOG_APPEND(TimedLog::LOG_INFO, "-- setting summoned creatureActorId for %i-%i to %i",
                        newPtr.getCellRef().getRefNum(), newPtr.getCellRef().getMpNum(), creatureActorId);

                    // Check if this creature is present in the summoner's summoned creature map
                    std::map<ESM::SummonKey, int>& creatureMap = masterCreatureStats.getSummonedCreatureMap();

                    bool foundSummonedCreature = false;

                    for (std::map<ESM::SummonKey, int>::iterator it = creatureMap.begin(); it != creatureMap.end(); )
                    {
                        if (it->first.mEffectId == baseObject.summonEffectId && it->first.mSourceId == baseObject.summonSpellId)
                        {
                            foundSummonedCreature = true;
                            break;
                        }
                        ++it;
                    }

                    // If it is, update its creatureActorId
                    if (foundSummonedCreature)
                    {
                        masterCreatureStats.setSummonedCreatureActorId(baseObject.refId, creatureActorId);
                    }
                    // If not, add it to the summoned creature map
                    else
                    {
                        ESM::SummonKey summonKey(baseObject.summonEffectId, baseObject.summonSpellId, -1);
                        creatureMap.emplace(summonKey, creatureActorId);
                    }

                    creatureStats.setFriendlyHits(0);
                }
            }
        }
        else
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Actor already existed!");
    }
}

void ObjectList::deleteObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
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

            // Is this a dying actor being deleted before its death animation has finished? If so,
            // increase the death count for the actor if applicable and run the actor's script,
            // which is the same as what happens in OpenMW's ContainerWindow::onDisposeCorpseButtonClicked()
            // if an actor's corpse is disposed of before its death animation is finished
            if (ptrFound.getClass().isActor())
            {
                MWMechanics::CreatureStats& creatureStats = ptrFound.getClass().getCreatureStats(ptrFound);

                if (creatureStats.isDead() && !creatureStats.isDeathAnimationFinished())
                {
                    creatureStats.setDeathAnimationFinished(true);
                    MWBase::Environment::get().getMechanicsManager()->notifyDied(ptrFound);

                    const std::string script = ptrFound.getClass().getScript(ptrFound);
                    if (!script.empty() && MWBase::Environment::get().getWorld()->getScriptsEnabled())
                    {
                        MWScript::InterpreterContext interpreterContext(&ptrFound.getRefData().getLocals(), ptrFound);
                        MWBase::Environment::get().getScriptManager()->run(script, interpreterContext);
                    }
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
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
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
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i, trapSpellId: %s, trapAction: %i",
            baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum, baseObject.trapSpellId.c_str(), baseObject.trapAction);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            if (baseObject.trapAction == mwmp::BaseObjectList::TRAP_ACTION::SET_TRAP)
            {
                if (!baseObject.trapSpellId.empty() && !RecordHelper::doesRecordIdExist<ESM::Spell>(baseObject.trapSpellId))
                {
                    LOG_MESSAGE_SIMPLE(TimedLog::LOG_ERROR, "-- Ignored attempt to set invalid spell %s as trap", baseObject.refId.c_str());
                    ptrFound.getCellRef().setTrap("");
                }
                else
                {
                    ptrFound.getCellRef().setTrap(baseObject.trapSpellId);
                }
            }
            else
            {
                if (baseObject.trapAction == mwmp::BaseObjectList::TRAP_ACTION::DISARM)
                {
                    MWBase::Environment::get().getSoundManager()->playSound3D(ptrFound, "Disarm Trap", 1.0f, 1.0f);
                }
                else if (baseObject.trapAction == mwmp::BaseObjectList::TRAP_ACTION::TRIGGER)
                {
                    MWMechanics::CastSpell cast(ptrFound, ptrFound);
                    cast.mHitPosition = baseObject.trapTriggerPosition.asVec3();
                    cast.cast(baseObject.trapSpellId);
                }

                ptrFound.getCellRef().setTrap("");
            }
        }
    }
}

void ObjectList::scaleObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i, scale: %f", baseObject.refId.c_str(), baseObject.refNum,
            baseObject.mpNum, baseObject.scale);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            MWBase::Environment::get().getWorld()->scaleObject(ptrFound, baseObject.scale);
        }
    }
}

void ObjectList::setObjectStates(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i, state: %s", baseObject.refId.c_str(), baseObject.refNum,
            baseObject.mpNum, baseObject.objectState ? "true" : "false");

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            if (baseObject.objectState)
            {
                MWBase::Environment::get().getWorld()->enable(ptrFound);

                // Is this an actor in a cell where we're the authority? If so, initialize it as
                // a LocalActor
                if (ptrFound.getClass().isActor() && mwmp::Main::get().getCellController()->hasLocalAuthority(*cellStore->getCell()))
                {
                    mwmp::Main::get().getCellController()->getCell(*cellStore->getCell())->initializeLocalActor(ptrFound);
                }
            }
            else
                MWBase::Environment::get().getWorld()->disable(ptrFound);
        }
    }
}

void ObjectList::moveObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            MWBase::Environment::get().getWorld()->moveObject(ptrFound, baseObject.position.pos[0], baseObject.position.pos[1],
                                                              baseObject.position.pos[2]);
        }
    }
}

void ObjectList::restockObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            //ptrFound.getClass().restock(ptrFound);

            reset();
            packetOrigin = mwmp::PACKET_ORIGIN::CLIENT_GAMEPLAY;
            cell = *ptrFound.getCell()->getCell();
            action = mwmp::BaseObjectList::SET;
            containerSubAction = mwmp::BaseObjectList::RESTOCK_RESULT;
            addEntireContainer(ptrFound);
            sendContainer();
        }
    }
}

void ObjectList::rotateObjects(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
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
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            MWBase::MechanicsManager * mechanicsManager = MWBase::Environment::get().getMechanicsManager();
            mechanicsManager->playAnimationGroup(ptrFound, baseObject.animGroup, baseObject.animMode,
                                                 std::numeric_limits<int>::max(), true);
        }
    }
}

void ObjectList::playObjectSounds(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        MWWorld::Ptr ptrFound;
        std::string objectDescription;

        if (baseObject.isPlayer)
        {
            if (baseObject.guid == Main::get().getLocalPlayer()->guid)
            {
                objectDescription = "LocalPlayer " + Main::get().getLocalPlayer()->npc.mName;
                ptrFound = Main::get().getLocalPlayer()->getPlayerPtr();
            }
            else
            {
                DedicatedPlayer *player = PlayerList::getPlayer(baseObject.guid);

                if (player != 0)
                {
                    objectDescription = "DedicatedPlayer " + player->npc.mName;
                    ptrFound = player->getPtr();
                }
            }
        }
        else
        {
            objectDescription = baseObject.refId + " " + std::to_string(baseObject.refNum) + "-" + std::to_string(baseObject.mpNum);
            ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);
        }

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "- Playing sound %s on %s", baseObject.soundId.c_str(), objectDescription.c_str());
            bool playAtPosition = false;
            if (ptrFound.isInCell()) {
                ESM::CellId localCell = Main::get().getLocalPlayer()->cell.getCellId();
                ESM::CellId soundCell = ptrFound.getCell()->getCell()->getCellId();
                playAtPosition = localCell == soundCell;
            }

            if (playAtPosition) {
                MWBase::Environment::get().getSoundManager()->playSound3D(ptrFound.getRefData().getPosition().asVec3(),
                    baseObject.soundId, baseObject.volume, baseObject.pitch, MWSound::Type::Sfx, MWSound::PlayMode::Normal, 0);
            }
            else {
                MWBase::Environment::get().getSoundManager()->playSound3D(ptrFound,
                    baseObject.soundId, baseObject.volume, baseObject.pitch, MWSound::Type::Sfx, MWSound::PlayMode::Normal, 0);
            }
        }
    }
}

void ObjectList::setGoldPoolsForObjects(MWWorld::CellStore* cellStore)
{
    for (const auto& baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            if (ptrFound.getClass().isActor())
            {
                LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Setting gold pool to %u", baseObject.goldPool);
                ptrFound.getClass().getCreatureStats(ptrFound).setGoldPool(baseObject.goldPool);

                LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Setting last gold restock time to %f hours and %i days passed",
                    baseObject.lastGoldRestockHour, baseObject.lastGoldRestockDay);
                ptrFound.getClass().getCreatureStats(ptrFound).setLastRestockTime(MWWorld::TimeStamp(baseObject.lastGoldRestockHour,
                    baseObject.lastGoldRestockDay));
            }
            else
            {
                LOG_MESSAGE_SIMPLE(TimedLog::LOG_WARN, "Failed to set gold pool on %s %i-%i because it is not an actor!",
                    ptrFound.getCellRef().getRefId().c_str(), ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());
            }
        }
    }
}

void ObjectList::activateDoors(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            MWWorld::DoorState doorState = static_cast<MWWorld::DoorState>(baseObject.doorState);

            ptrFound.getClass().setDoorState(ptrFound, doorState);
            MWBase::Environment::get().getWorld()->saveDoorState(ptrFound, doorState);
        }
    }
}

void ObjectList::setDoorDestinations(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            ptrFound.getCellRef().setTeleport(baseObject.teleportState);

            if (baseObject.teleportState)
            {
                ptrFound.getCellRef().setDoorDest(baseObject.destinationPosition);

                if (baseObject.destinationCell.isExterior())
                    ptrFound.getCellRef().setDestCell("");
                else
                    ptrFound.getCellRef().setDestCell(baseObject.destinationCell.getShortDescription());
            }
        }
    }
}

void ObjectList::runConsoleCommands(MWWorld::CellStore* cellStore)
{
    MWBase::WindowManager *windowManager = MWBase::Environment::get().getWindowManager();

    LOG_APPEND(TimedLog::LOG_VERBOSE, "- Console command: %s", consoleCommand.c_str());

    if (baseObjects.empty())
    {
        windowManager->clearConsolePtr();

        LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Running with no object reference");
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
                    LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Running on local player");
                    windowManager->setConsolePtr(Main::get().getLocalPlayer()->getPlayerPtr());
                    windowManager->executeCommandInConsole(consoleCommand);
                }
                else
                {
                    DedicatedPlayer *player = PlayerList::getPlayer(baseObject.guid);

                    if (player != 0)
                    {
                        LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Running on player %s", player->npc.mName.c_str());
                        windowManager->setConsolePtr(player->getPtr());
                        windowManager->executeCommandInConsole(consoleCommand);
                    }
                }
            }
            // Only require a valid cellStore if running on cell objects
            else if (cellStore)
            {
                LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Running on object %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

                MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

                if (ptrFound)
                {
                    LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                        ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

                    windowManager->setConsolePtr(ptrFound);
                    windowManager->executeCommandInConsole(consoleCommand);
                }
            }
        }

        windowManager->clearConsolePtr();
    }
}

void ObjectList::makeDialogueChoices(MWWorld::CellStore* cellStore)
{
    for (const auto& baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);
        
        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            if (ptrFound.getClass().isActor())
            {
                // Ensure the dialogue window has the correct Ptr set for it
                if (MWBase::Environment::get().getWindowManager()->containsMode(MWGui::GM_Dialogue))
                {
                    if (MWBase::Environment::get().getWindowManager()->getDialogueWindow()->getPtr() != ptrFound)
                    {
                        MWBase::Environment::get().getWindowManager()->getDialogueWindow()->setPtr(ptrFound);
                    }
                }
                else
                {
                    MWBase::Environment::get().getWindowManager()->pushGuiMode(MWGui::GM_Dialogue, ptrFound);
                }
                
                LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Making dialogue choice of type %i", baseObject.dialogueChoiceType);

                if (baseObject.dialogueChoiceType == DialogueChoiceType::TOPIC)
                {
                    LOG_APPEND(TimedLog::LOG_VERBOSE, "-- topic was %s", baseObject.topicId.c_str());
                }

                std::string topic = baseObject.topicId;

                if (MWBase::Environment::get().getWindowManager()->getTranslationDataStorage().hasTranslation())
                {
                    char delimiter = '|';

                    // If we're using a translated version of Morrowind, we may have received a string that had the original
                    // topic delimited from its possible English translation by a | character, in which case we need to use
                    // the original topic here
                    if (topic.find(delimiter) != std::string::npos)
                    {
                        topic = topic.substr(0, topic.find(delimiter));
                    }
                    // Alternatively, we may have received a topic that needs to be translated into the current language's
                    // version of it
                    else
                    {
                        std::string translatedTopic = MWBase::Environment::get().getWindowManager()->getTranslationDataStorage().getLocalizedTopicId(topic);

                        if (!translatedTopic.empty())
                        {
                            topic = translatedTopic;
                        }
                    }
                }

                MWBase::Environment::get().getWindowManager()->getDialogueWindow()->activateDialogueChoice(baseObject.dialogueChoiceType, topic);
            }
            else
            {
                LOG_MESSAGE_SIMPLE(TimedLog::LOG_WARN, "Failed to make dialogue choice for %s %i-%i because it is not an actor!",
                    ptrFound.getCellRef().getRefId().c_str(), ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());
            }
        }
    }
}

void ObjectList::setClientLocals(MWWorld::CellStore* cellStore)
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            for (const auto& clientLocal : baseObject.clientLocals)
            {
                std::string valueAsString;
                std::string variableTypeAsString;

                if (clientLocal.variableType == mwmp::VARIABLE_TYPE::SHORT || clientLocal.variableType == mwmp::VARIABLE_TYPE::LONG)
                {
                    variableTypeAsString = clientLocal.variableType == mwmp::VARIABLE_TYPE::SHORT ? "short" : "long";
                    valueAsString = std::to_string(clientLocal.intValue);
                }
                else if (clientLocal.variableType == mwmp::VARIABLE_TYPE::FLOAT)
                {
                    variableTypeAsString = "float";
                    valueAsString = std::to_string(clientLocal.floatValue);
                }

                if (clientLocal.variableType == mwmp::VARIABLE_TYPE::SHORT)
                    ptrFound.getRefData().getLocals().mShorts.at(clientLocal.internalIndex) = clientLocal.intValue;
                else if (clientLocal.variableType == mwmp::VARIABLE_TYPE::LONG)
                    ptrFound.getRefData().getLocals().mLongs.at(clientLocal.internalIndex) = clientLocal.intValue;
                else if (clientLocal.variableType == mwmp::VARIABLE_TYPE::FLOAT)
                    ptrFound.getRefData().getLocals().mFloats.at(clientLocal.internalIndex) = clientLocal.floatValue;
            }
        }
    }
}

void ObjectList::setMemberShorts()
{
    /*
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s, index: %i, shortVal: %i", baseObject.refId.c_str(),
                   baseObject.index, baseObject.shortVal);

        // Mimic the way a Ptr is fetched in InterpreterContext for similar situations
        MWWorld::Ptr ptrFound = MWBase::Environment::get().getWorld()->searchPtr(baseObject.refId, false);

        if (ptrFound)
        {
            LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Found %s %i-%i", ptrFound.getCellRef().getRefId().c_str(),
                               ptrFound.getCellRef().getRefNum(), ptrFound.getCellRef().getMpNum());

            std::string scriptId = ptrFound.getClass().getScript(ptrFound);

            ptrFound.getRefData().setLocals(
                *MWBase::Environment::get().getWorld()->getStore().get<ESM::Script>().find(scriptId));

            ptrFound.getRefData().getLocals().mShorts.at(baseObject.index) = baseObject.shortVal;;
        }
    }
    */
}

void ObjectList::playMusic()
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- filename: %s", baseObject.musicFilename.c_str());

        MWBase::Environment::get().getSoundManager()->streamMusic(baseObject.musicFilename);
    }
}

void ObjectList::playVideo()
{
    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- filename: %s, allowSkipping: %s", baseObject.videoFilename.c_str(),
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
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(),
            baseObject.refNum, baseObject.mpNum);

        MWWorld::Ptr ptrFound = cellStore->searchExact(baseObject.refNum, baseObject.mpNum, baseObject.refId);

        if (ptrFound)
        {
            if (ptrFound.getClass().hasContainerStore(ptrFound))
                addEntireContainer(ptrFound);
            else
                LOG_APPEND(TimedLog::LOG_VERBOSE, "-- Object lacks container store", ptrFound.getCellRef().getRefId().c_str());
        }
    }
}

void ObjectList::addObjectGeneric(const MWWorld::Ptr& ptr)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    addBaseObject(baseObject);
}

void ObjectList::addObjectActivate(const MWWorld::Ptr& ptr, const MWWorld::Ptr& activatingActor)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.activatingActor = MechanicsHelper::getTarget(activatingActor);

    addBaseObject(baseObject);
}

void ObjectList::addObjectHit(const MWWorld::Ptr& ptr, const MWWorld::Ptr& hittingActor)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.hittingActor = MechanicsHelper::getTarget(hittingActor);
    baseObject.hitAttack.success = false;

    addBaseObject(baseObject);
}

void ObjectList::addObjectHit(const MWWorld::Ptr& ptr, const MWWorld::Ptr& hittingActor, const Attack hitAttack)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.hittingActor = MechanicsHelper::getTarget(hittingActor);
    baseObject.hitAttack = hitAttack;

    addBaseObject(baseObject);
}

void ObjectList::addObjectPlace(const MWWorld::Ptr& ptr, bool droppedByPlayer)
{
    if (ptr.getCellRef().getRefId().find("$dynamic") != std::string::npos)
    {
        MWBase::Environment::get().getWindowManager()->messageBox("You cannot place unsynchronized custom items in multiplayer.");
        return;
    }

    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
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

    addBaseObject(baseObject);
}

void ObjectList::addObjectSpawn(const MWWorld::Ptr& ptr)
{
    if (ptr.getCellRef().getRefId().find("$dynamic") != std::string::npos)
    {
        MWBase::Environment::get().getWindowManager()->messageBox("You're trying to spawn a custom object lacking a server-given refId, "
            "and those cannot be synchronized in multiplayer.");
        return;
    }

    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.isSummon = false;
    baseObject.summonDuration = -1;

    // Make sure we send the RefData position instead of the CellRef one, because that's what
    // we actually see on this client
    baseObject.position = ptr.getRefData().getPosition();

    addBaseObject(baseObject);
}

void ObjectList::addObjectSpawn(const MWWorld::Ptr& ptr, const MWWorld::Ptr& master, std::string spellId, int effectId, float duration)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.isSummon = true;
    baseObject.summonSpellId = spellId;
    baseObject.summonEffectId = effectId;
    baseObject.summonDuration = duration;
    baseObject.master = MechanicsHelper::getTarget(master);

    // Make sure we send the RefData position instead of the CellRef one, because that's what
    // we actually see on this client
    baseObject.position = ptr.getRefData().getPosition();

    addBaseObject(baseObject);
}

void ObjectList::addObjectLock(const MWWorld::Ptr& ptr, int lockLevel)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.lockLevel = lockLevel;
    addBaseObject(baseObject);
}

void ObjectList::addObjectDialogueChoice(const MWWorld::Ptr& ptr, std::string dialogueChoice)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);

    const MWWorld::Store<ESM::GameSetting>& gmst = MWBase::Environment::get().getWorld()->getStore().get<ESM::GameSetting>();

    // Because the actual text for any of the special dialogue choices can vary according to the game language used,
    // set the type of dialogue choice by doing a lot of checks
    if (dialogueChoice == gmst.find("sPersuasion")->mValue.getString())
        baseObject.dialogueChoiceType = static_cast<int>(DialogueChoiceType::PERSUASION);
    else if (dialogueChoice == gmst.find("sCompanionShare")->mValue.getString())
        baseObject.dialogueChoiceType = DialogueChoiceType::COMPANION_SHARE;
    else if (dialogueChoice == gmst.find("sBarter")->mValue.getString())
        baseObject.dialogueChoiceType = DialogueChoiceType::BARTER;
    else if (dialogueChoice == gmst.find("sSpells")->mValue.getString())
        baseObject.dialogueChoiceType = DialogueChoiceType::SPELLS;
    else if (dialogueChoice == gmst.find("sTravel")->mValue.getString())
        baseObject.dialogueChoiceType = DialogueChoiceType::TRAVEL;
    else if (dialogueChoice == gmst.find("sSpellMakingMenuTitle")->mValue.getString())
        baseObject.dialogueChoiceType = DialogueChoiceType::SPELLMAKING;
    else if (dialogueChoice == gmst.find("sEnchanting")->mValue.getString())
        baseObject.dialogueChoiceType = DialogueChoiceType::ENCHANTING;
    else if (dialogueChoice == gmst.find("sServiceTrainingTitle")->mValue.getString())
        baseObject.dialogueChoiceType = DialogueChoiceType::TRAINING;
    else if (dialogueChoice == gmst.find("sRepair")->mValue.getString())
        baseObject.dialogueChoiceType = DialogueChoiceType::REPAIR;
    else
    {
        baseObject.dialogueChoiceType = DialogueChoiceType::TOPIC;

        // For translated versions of the game, make sure we translate the topic back into English first
        if (MWBase::Environment::get().getWindowManager()->getTranslationDataStorage().hasTranslation())
            baseObject.topicId = dialogueChoice + "|" + MWBase::Environment::get().getWindowManager()->getTranslationDataStorage().topicID(dialogueChoice);
        else
            baseObject.topicId = dialogueChoice;
    }

    addBaseObject(baseObject);
}

void ObjectList::addObjectMiscellaneous(const MWWorld::Ptr& ptr, unsigned int goldPool, float lastGoldRestockHour, int lastGoldRestockDay)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.goldPool = goldPool;
    baseObject.lastGoldRestockHour = lastGoldRestockHour;
    baseObject.lastGoldRestockDay = lastGoldRestockDay;
    addBaseObject(baseObject);
}

void ObjectList::addObjectTrap(const MWWorld::Ptr& ptr, std::string trapSpellId, unsigned int trapAction, const ESM::Position&  trapTriggerPosition)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.trapAction = trapAction;
    baseObject.trapSpellId = trapSpellId;
    baseObject.trapTriggerPosition = trapTriggerPosition;
    addBaseObject(baseObject);
}

void ObjectList::addObjectScale(const MWWorld::Ptr& ptr, float scale)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.scale = scale;
    addBaseObject(baseObject);
}

void ObjectList::addObjectSound(const MWWorld::Ptr& ptr, std::string soundId, float volume, float pitch)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.soundId = soundId;
    baseObject.volume = volume;
    baseObject.pitch = pitch;
    addBaseObject(baseObject);
}

void ObjectList::addObjectState(const MWWorld::Ptr& ptr, bool objectState)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.objectState = objectState;
    addBaseObject(baseObject);
}

void ObjectList::addObjectAnimPlay(const MWWorld::Ptr& ptr, std::string group, int mode)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.animGroup = group;
    baseObject.animMode = mode;
    addBaseObject(baseObject);
}

void ObjectList::addDoorState(const MWWorld::Ptr& ptr, MWWorld::DoorState state)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    baseObject.doorState = static_cast<int>(state);
    addBaseObject(baseObject);
}

void ObjectList::addMusicPlay(std::string filename)
{
    mwmp::BaseObject baseObject;
    baseObject.musicFilename = filename;
    addBaseObject(baseObject);
}

void ObjectList::addVideoPlay(std::string filename, bool allowSkipping)
{
    mwmp::BaseObject baseObject;
    baseObject.videoFilename = filename;
    baseObject.allowSkipping = allowSkipping;
    addBaseObject(baseObject);
}

void ObjectList::addClientScriptLocal(const MWWorld::Ptr& ptr, int internalIndex, int value, mwmp::VARIABLE_TYPE variableType)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    ClientVariable clientLocal;
    clientLocal.internalIndex = internalIndex;
    clientLocal.variableType = variableType;
    clientLocal.intValue = value;
    baseObject.clientLocals.push_back(clientLocal);
    addBaseObject(baseObject);
}

void ObjectList::addClientScriptLocal(const MWWorld::Ptr& ptr, int internalIndex, float value)
{
    cell = *ptr.getCell()->getCell();

    mwmp::BaseObject baseObject = getBaseObjectFromPtr(ptr);
    ClientVariable clientLocal;
    clientLocal.internalIndex = internalIndex;
    clientLocal.variableType = mwmp::VARIABLE_TYPE::FLOAT;
    clientLocal.floatValue = value;
    baseObject.clientLocals.push_back(clientLocal);
    addBaseObject(baseObject);
}

void ObjectList::addScriptMemberShort(std::string refId, int index, int shortVal)
{
    /*
    mwmp::BaseObject baseObject;
    baseObject.refId = refId;
    baseObject.index = index;
    baseObject.shortVal = shortVal;
    addBaseObject(baseObject);
    */
}

void ObjectList::sendObjectActivate()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_ACTIVATE)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_ACTIVATE)->Send();
}

void ObjectList::sendObjectHit()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_HIT)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_HIT)->Send();
}

void ObjectList::sendObjectPlace()
{
    if (baseObjects.size() == 0)
        return;

    LOG_MESSAGE_SIMPLE(TimedLog::LOG_VERBOSE, "Sending ID_OBJECT_PLACE about %s", cell.getShortDescription().c_str());

    for (const auto &baseObject : baseObjects)
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s, count: %i", baseObject.refId.c_str(), baseObject.count);

    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_PLACE)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_PLACE)->Send();
}

void ObjectList::sendObjectSpawn()
{
    if (baseObjects.size() == 0)
        return;

    LOG_MESSAGE_SIMPLE(TimedLog::LOG_VERBOSE, "Sending ID_OBJECT_SPAWN about %s", cell.getShortDescription().c_str());

    for (const auto &baseObject : baseObjects)
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s-%i", baseObject.refId.c_str(), baseObject.refNum);

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

void ObjectList::sendObjectDialogueChoice()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_DIALOGUE_CHOICE)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_DIALOGUE_CHOICE)->Send();
}

void ObjectList::sendObjectMiscellaneous()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_MISCELLANEOUS)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_MISCELLANEOUS)->Send();
}

void ObjectList::sendObjectRestock()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_RESTOCK)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_RESTOCK)->Send();
}

void ObjectList::sendObjectSound()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_SOUND)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_OBJECT_SOUND)->Send();
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
    LOG_MESSAGE_SIMPLE(TimedLog::LOG_VERBOSE, "Sending ID_DOOR_STATE about %s", cell.getShortDescription().c_str());

    for (const auto &baseObject : baseObjects)
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s-%i, state: %s", baseObject.refId.c_str(), baseObject.refNum,
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

void ObjectList::sendClientScriptLocal()
{
    LOG_MESSAGE_SIMPLE(TimedLog::LOG_VERBOSE, "Sending ID_CLIENT_SCRIPT_LOCAL about %s", cell.getShortDescription().c_str());

    for (const auto &baseObject : baseObjects)
    {
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s %i-%i", baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum);

        for (const auto& clientLocal : baseObject.clientLocals)
        {
            std::string valueAsString;
            std::string variableTypeAsString;

            if (clientLocal.variableType == mwmp::VARIABLE_TYPE::SHORT || clientLocal.variableType == mwmp::VARIABLE_TYPE::LONG)
            {
                variableTypeAsString = clientLocal.variableType == mwmp::VARIABLE_TYPE::SHORT ? "short" : "long";
                valueAsString = std::to_string(clientLocal.intValue);
            }
            else if (clientLocal.variableType == mwmp::VARIABLE_TYPE::FLOAT)
            {
                variableTypeAsString = "float";
                valueAsString = std::to_string(clientLocal.floatValue);
            }

            LOG_APPEND(TimedLog::LOG_VERBOSE, "- type %s, value: %s", variableTypeAsString.c_str(), valueAsString.c_str());
        }
    }

    mwmp::Main::get().getNetworking()->getObjectPacket(ID_CLIENT_SCRIPT_LOCAL)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_CLIENT_SCRIPT_LOCAL)->Send();
}

void ObjectList::sendScriptMemberShort()
{
    /*
    LOG_MESSAGE_SIMPLE(TimedLog::LOG_VERBOSE, "Sending ID_SCRIPT_MEMBER_SHORT");

    for (const auto &baseObject : baseObjects)
        LOG_APPEND(TimedLog::LOG_VERBOSE, "- cellRef: %s, index: %i, shortVal: %i", baseObject.refId.c_str(),
                   baseObject.index, baseObject.shortVal);

    mwmp::Main::get().getNetworking()->getObjectPacket(ID_SCRIPT_MEMBER_SHORT)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_SCRIPT_MEMBER_SHORT)->Send();
    */
}

void ObjectList::sendContainer()
{
    std::string debugMessage = "Sending ID_CONTAINER with action ";
    
    if (action == mwmp::BaseObjectList::SET)
        debugMessage += "SET";
    else if (action == mwmp::BaseObjectList::ADD)
        debugMessage += "ADD";
    else if (action == mwmp::BaseObjectList::REMOVE)
        debugMessage += "REMOVE";

    debugMessage += " and subaction ";

    if (containerSubAction == mwmp::BaseObjectList::NONE)
        debugMessage += "NONE";
    else if (containerSubAction == mwmp::BaseObjectList::DRAG)
        debugMessage += "DRAG";
    else if (containerSubAction == mwmp::BaseObjectList::DROP)
        debugMessage += "DROP";
    else if (containerSubAction == mwmp::BaseObjectList::TAKE_ALL)
        debugMessage += "TAKE_ALL";
    else if (containerSubAction == mwmp::BaseObjectList::REPLY_TO_REQUEST)
        debugMessage += "REPLY_TO_REQUEST";

    debugMessage += "\n- cell " + cell.getShortDescription();

    for (const auto &baseObject : baseObjects)
    {
        debugMessage += "\n- container " + baseObject.refId + " " + std::to_string(baseObject.refNum) + "-" + std::to_string(baseObject.mpNum);

        for (const auto &containerItem : baseObject.containerItems)
        {
            debugMessage += "\n-- item " + containerItem.refId + ", count " + std::to_string(containerItem.count) +
                ", actionCount " + std::to_string(containerItem.actionCount);
        }
    }

    LOG_MESSAGE_SIMPLE(TimedLog::LOG_VERBOSE, "%s", debugMessage.c_str());

    mwmp::Main::get().getNetworking()->getObjectPacket(ID_CONTAINER)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_CONTAINER)->Send();
}

void ObjectList::sendConsoleCommand()
{
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_CONSOLE_COMMAND)->setObjectList(this);
    mwmp::Main::get().getNetworking()->getObjectPacket(ID_CONSOLE_COMMAND)->Send();
}
