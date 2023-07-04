#include "containeritemmodel.hpp"

#include <algorithm>

<<<<<<< HEAD
/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include "../mwmp/Main.hpp"
#include "../mwmp/Networking.hpp"
#include "../mwmp/LocalPlayer.hpp"
#include "../mwmp/ObjectList.hpp"
/*
    End of tes3mp addition
*/

#include "../mwmechanics/creaturestats.hpp"
=======
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
#include "../mwmechanics/actorutil.hpp"
#include "../mwmechanics/creaturestats.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/containerstore.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/world.hpp"

namespace
{

    bool stacks(const MWWorld::Ptr& left, const MWWorld::Ptr& right)
    {
        if (left == right)
            return true;

        // If one of the items is in an inventory and currently equipped, we need to check stacking both ways to be sure
        if (left.getContainerStore() && right.getContainerStore())
            return left.getContainerStore()->stacks(left, right) && right.getContainerStore()->stacks(left, right);

        if (left.getContainerStore())
            return left.getContainerStore()->stacks(left, right);
        if (right.getContainerStore())
            return right.getContainerStore()->stacks(left, right);

        MWWorld::ContainerStore store;
        return store.stacks(left, right);
    }

}

namespace MWGui
{
    ContainerItemModel::ContainerItemModel(
        const std::vector<MWWorld::Ptr>& itemSources, const std::vector<MWWorld::Ptr>& worldItems)
        : mWorldItems(worldItems)
        , mTrading(true)
    {
        assert(!itemSources.empty());
        // Tie resolution lifetimes to the ItemModel
        mItemSources.reserve(itemSources.size());
        for (const MWWorld::Ptr& source : itemSources)
        {
            MWWorld::ContainerStore& store = source.getClass().getContainerStore(source);
            mItemSources.emplace_back(source, store.resolveTemporarily());
        }
    }

    ContainerItemModel::ContainerItemModel(const MWWorld::Ptr& source)
        : mTrading(false)
    {
        MWWorld::ContainerStore& store = source.getClass().getContainerStore(source);
        mItemSources.emplace_back(source, store.resolveTemporarily());
    }

    bool ContainerItemModel::allowedToUseItems() const
    {
        if (mItemSources.empty())
            return true;

        MWWorld::Ptr ptr = MWMechanics::getPlayer();
        MWWorld::Ptr victim;

        // Check if the player is allowed to use items from opened container
        MWBase::MechanicsManager* mm = MWBase::Environment::get().getMechanicsManager();
        return mm->isAllowedToUse(ptr, mItemSources[0].first, victim);
    }

<<<<<<< HEAD
MWWorld::Ptr ContainerItemModel::copyItem (const ItemStack& item, size_t count, bool allowAutoEquip)
{
    auto& source = mItemSources[0];
    MWWorld::ContainerStore& store = source.first.getClass().getContainerStore(source.first);
    if (item.mBase.getContainerStore() == &store)
        throw std::runtime_error("Item to copy needs to be from a different container!");

    /*
        Start of tes3mp addition

        Send an ID_CONTAINER packet every time an item is added to a container here
    */
    mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
    objectList->reset();
    objectList->packetOrigin = mwmp::PACKET_ORIGIN::CLIENT_GAMEPLAY;
    objectList->cell = *source.first.getCell()->getCell();
    objectList->action = mwmp::BaseObjectList::ADD;
    objectList->containerSubAction = mwmp::BaseObjectList::NONE;
    mwmp::BaseObject baseObject = objectList->getBaseObjectFromPtr(source.first);
    objectList->addContainerItem(baseObject, item.mBase, count, 0);
    objectList->addBaseObject(baseObject);
    objectList->sendContainer();
    /*
        End of tes3mp addition
    */

    /*
        Start of tes3mp change (major)

        Instead of unilaterally adding the item to this source's ContainerStore on this
        client and returning the resulting Ptr, rely on the server to handle the item
        transfer and just return the original item Ptr as a placeholder return value
    */
    return item.mBase;
    /*
        End of tes3mp change (major)
    */
}

void ContainerItemModel::removeItem (const ItemStack& item, size_t count)
{
    int toRemove = count;

    for (auto& source : mItemSources)
=======
    ItemStack ContainerItemModel::getItem(ModelIndex index)
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
    {
        if (index < 0)
            throw std::runtime_error("Invalid index supplied");
        if (mItems.size() <= static_cast<size_t>(index))
            throw std::runtime_error("Item index out of range");
        return mItems[index];
    }

    size_t ContainerItemModel::getItemCount()
    {
        return mItems.size();
    }

    ItemModel::ModelIndex ContainerItemModel::getIndex(const ItemStack& item)
    {
        size_t i = 0;
        for (ItemStack& itemStack : mItems)
        {
            if (itemStack == item)
                return i;
            ++i;
        }
        return -1;
    }

    MWWorld::Ptr ContainerItemModel::copyItem(const ItemStack& item, size_t count, bool allowAutoEquip)
    {
        auto& source = mItemSources[0];
        MWWorld::ContainerStore& store = source.first.getClass().getContainerStore(source.first);
        if (item.mBase.getContainerStore() == &store)
            throw std::runtime_error("Item to copy needs to be from a different container!");
        return *store.add(item.mBase, count, allowAutoEquip);
    }

    void ContainerItemModel::removeItem(const ItemStack& item, size_t count)
    {
        int toRemove = count;

        for (auto& source : mItemSources)
        {
            MWWorld::ContainerStore& store = source.first.getClass().getContainerStore(source.first);

            for (MWWorld::ContainerStoreIterator it = store.begin(); it != store.end(); ++it)
            {
<<<<<<< HEAD
                /*
                    Start of tes3mp change (major)

                    Send an ID_CONTAINER packet every time an item is removed here and prevent any
                    unilateral item removal on this client, as long as this isn't the player's
                    currently open container and doesn't require the drag and drop logic dealt with
                    in MWGui::ContainerWindow instead
                */
                mwmp::CurrentContainer *currentContainer = &mwmp::Main::get().getLocalPlayer()->currentContainer;

                if (currentContainer->refNum != source.first.getCellRef().getRefNum().mIndex ||
                    currentContainer->mpNum != source.first.getCellRef().getMpNum())
                {
                    mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
                    objectList->reset();
                    objectList->packetOrigin = mwmp::PACKET_ORIGIN::CLIENT_GAMEPLAY;
                    objectList->cell = *source.first.getCell()->getCell();
                    objectList->action = mwmp::BaseObjectList::REMOVE;
                    objectList->containerSubAction = mwmp::BaseObjectList::NONE;
                    mwmp::BaseObject baseObject = objectList->getBaseObjectFromPtr(source.first);
                    objectList->addContainerItem(baseObject, *it, it->getRefData().getCount(), toRemove);
                    objectList->addBaseObject(baseObject);
                    objectList->sendContainer();
                    
                    toRemove -= it->getRefData().getCount();
                }
                else
                {
                    int quantity = it->mRef->mData.getCount(false);
                    // If this is a restocking quantity, just don't remove it
                    if (quantity < 0 && mTrading)
                        toRemove += quantity;
                    else
                        toRemove -= store.remove(*it, toRemove, source.first);
                }
                /*
                    End of tes3mp change (major)
                */

=======
                if (stacks(*it, item.mBase))
                {
                    int quantity = it->mRef->mData.getCount(false);
                    // If this is a restocking quantity, just don't remove it
                    if (quantity < 0 && mTrading)
                        toRemove += quantity;
                    else
                        toRemove -= store.remove(*it, toRemove);
                    if (toRemove <= 0)
                        return;
                }
            }
        }
        for (MWWorld::Ptr& source : mWorldItems)
        {
            if (stacks(source, item.mBase))
            {
                int refCount = source.getRefData().getCount();
                if (refCount - toRemove <= 0)
                    MWBase::Environment::get().getWorld()->deleteObject(source);
                else
                    source.getRefData().setCount(std::max(0, refCount - toRemove));
                toRemove -= refCount;
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
                if (toRemove <= 0)
                    return;
            }
        }

        throw std::runtime_error("Not enough items to remove could be found");
    }

    void ContainerItemModel::update()
    {
        mItems.clear();
        for (auto& source : mItemSources)
        {
<<<<<<< HEAD
            int refCount = source.getRefData().getCount();
            if (refCount - toRemove <= 0)
            {
                /*
                    Start of tes3mp addition

                    Send an ID_OBJECT_DELETE packet every time an item is removed from the world
                    because it has been purchased from its owner
                */
                mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
                objectList->reset();
                objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
                objectList->addObjectGeneric(source);
                objectList->sendObjectDelete();
                /*
                    End of tes3mp addition
                */

                MWBase::Environment::get().getWorld()->deleteObject(source);
            }
            else
                source.getRefData().setCount(std::max(0, refCount - toRemove));
            toRemove -= refCount;
            if (toRemove <= 0)
                return;
=======
            MWWorld::ContainerStore& store = source.first.getClass().getContainerStore(source.first);

            for (MWWorld::ContainerStoreIterator it = store.begin(); it != store.end(); ++it)
            {
                if (!(*it).getClass().showsInInventory(*it))
                    continue;

                bool found = false;
                for (ItemStack& itemStack : mItems)
                {
                    if (stacks(*it, itemStack.mBase))
                    {
                        // we already have an item stack of this kind, add to it
                        itemStack.mCount += it->getRefData().getCount();
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    // no stack yet, create one
                    ItemStack newItem(*it, this, it->getRefData().getCount());
                    mItems.push_back(newItem);
                }
            }
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
        }
        for (MWWorld::Ptr& source : mWorldItems)
        {
            bool found = false;
            for (ItemStack& itemStack : mItems)
            {
                if (stacks(source, itemStack.mBase))
                {
                    // we already have an item stack of this kind, add to it
                    itemStack.mCount += source.getRefData().getCount();
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                // no stack yet, create one
                ItemStack newItem(source, this, source.getRefData().getCount());
                mItems.push_back(newItem);
            }
        }
    }
    bool ContainerItemModel::onDropItem(const MWWorld::Ptr& item, int count)
    {
        if (mItemSources.empty())
            return false;

        MWWorld::Ptr target = mItemSources[0].first;

        if (target.getType() != ESM::Container::sRecordId)
            return true;

        // check container organic flag
        MWWorld::LiveCellRef<ESM::Container>* ref = target.get<ESM::Container>();
        if (ref->mBase->mFlags & ESM::Container::Organic)
        {
            MWBase::Environment::get().getWindowManager()->messageBox("#{sContentsMessage2}");
            return false;
        }

        // check that we don't exceed container capacity
        float weight = item.getClass().getWeight(item) * count;
        if (target.getClass().getCapacity(target) < target.getClass().getEncumbrance(target) + weight)
        {
            MWBase::Environment::get().getWindowManager()->messageBox("#{sContentsMessage3}");
            return false;
        }

        return true;
    }

    bool ContainerItemModel::onTakeItem(const MWWorld::Ptr& item, int count)
    {
        if (mItemSources.empty())
            return false;

        MWWorld::Ptr target = mItemSources[0].first;

        // Looting a dead corpse is considered OK
        if (target.getClass().isActor() && target.getClass().getCreatureStats(target).isDead())
            return true;

        MWWorld::Ptr player = MWMechanics::getPlayer();
        MWBase::Environment::get().getMechanicsManager()->itemTaken(player, item, target, count);

        return true;
    }

    bool ContainerItemModel::usesContainer(const MWWorld::Ptr& container)
    {
        for (const auto& source : mItemSources)
        {
            if (source.first == container)
                return true;
        }
        return false;
    }

}
