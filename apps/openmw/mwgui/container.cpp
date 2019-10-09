#include "container.hpp"

#include <MyGUI_InputManager.h>
#include <MyGUI_Button.h>

/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include <components/openmw-mp/MWMPLog.hpp>
#include "../mwmp/Main.hpp"
#include "../mwmp/Networking.hpp"
#include "../mwmp/LocalPlayer.hpp"
#include "../mwmp/ObjectList.hpp"
#include "../mwmp/CellController.hpp"
/*
    End of tes3mp addition
*/

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/inventorystore.hpp"

#include "../mwmechanics/creaturestats.hpp"

#include "countdialog.hpp"
#include "inventorywindow.hpp"

#include "itemview.hpp"
#include "itemwidget.hpp"
#include "inventoryitemmodel.hpp"
#include "containeritemmodel.hpp"
#include "sortfilteritemmodel.hpp"
#include "pickpocketitemmodel.hpp"
#include "draganddrop.hpp"

namespace MWGui
{

    ContainerWindow::ContainerWindow(DragAndDrop* dragAndDrop)
        : WindowBase("openmw_container_window.layout")
        , mDragAndDrop(dragAndDrop)
        , mSortModel(NULL)
        , mModel(NULL)
        , mSelectedItem(-1)
    {
        getWidget(mDisposeCorpseButton, "DisposeCorpseButton");
        getWidget(mTakeButton, "TakeButton");
        getWidget(mCloseButton, "CloseButton");

        getWidget(mItemView, "ItemView");
        mItemView->eventBackgroundClicked += MyGUI::newDelegate(this, &ContainerWindow::onBackgroundSelected);
        mItemView->eventItemClicked += MyGUI::newDelegate(this, &ContainerWindow::onItemSelected);

        mDisposeCorpseButton->eventMouseButtonClick += MyGUI::newDelegate(this, &ContainerWindow::onDisposeCorpseButtonClicked);
        mCloseButton->eventMouseButtonClick += MyGUI::newDelegate(this, &ContainerWindow::onCloseButtonClicked);
        mTakeButton->eventMouseButtonClick += MyGUI::newDelegate(this, &ContainerWindow::onTakeAllButtonClicked);

        setCoord(200,0,600,300);
    }

    void ContainerWindow::onItemSelected(int index)
    {
        if (mDragAndDrop->mIsOnDragAndDrop)
        {
            dropItem();
            return;
        }

        const ItemStack& item = mSortModel->getItem(index);

        // We can't take a conjured item from a container (some NPC we're pickpocketing, a box, etc)
        if (item.mFlags & ItemStack::Flag_Bound)
        {
            MWBase::Environment::get().getWindowManager()->messageBox("#{sContentsMessage1}");
            return;
        }

        MWWorld::Ptr object = item.mBase;
        int count = item.mCount;
        bool shift = MyGUI::InputManager::getInstance().isShiftPressed();
        if (MyGUI::InputManager::getInstance().isControlPressed())
            count = 1;

        mSelectedItem = mSortModel->mapToSource(index);

        if (count > 1 && !shift)
        {
            CountDialog* dialog = MWBase::Environment::get().getWindowManager()->getCountDialog();
            dialog->openCountDialog(object.getClass().getName(object), "#{sTake}", count);
            dialog->eventOkClicked.clear();
            dialog->eventOkClicked += MyGUI::newDelegate(this, &ContainerWindow::dragItem);
        }
        else
            dragItem (NULL, count);
    }

    void ContainerWindow::dragItem(MyGUI::Widget* sender, int count)
    {
        if (!mModel)
            return;

        if (!onTakeItem(mModel->getItem(mSelectedItem), count))
            return;

        /*
            Start of tes3mp addition

            Send an ID_CONTAINER packet every time an item starts being dragged
            from a container
        */
        mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
        objectList->reset();
        objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
        objectList->cell = *mPtr.getCell()->getCell();
        objectList->action = mwmp::BaseObjectList::REMOVE;
        objectList->containerSubAction = mwmp::BaseObjectList::DRAG;

        mwmp::BaseObject baseObject = objectList->getBaseObject(mPtr);
        MWWorld::Ptr itemPtr = mModel->getItem(mSelectedItem).mBase;
        objectList->addContainerItem(baseObject, itemPtr, count);
        objectList->addObject(baseObject);
        objectList->sendContainer();

        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Sending ID_CONTAINER about\n- Ptr cellRef: %s, %i\n- cell: %s\n- item: %s, %i",
            baseObject.refId.c_str(), baseObject.refNum, objectList->cell.getDescription().c_str(),
            itemPtr.getCellRef().getRefId().c_str(), itemPtr.getRefData().getCount());
        /*
            End of tes3mp addition
        */

        /*
            Start of tes3mp change (major)

            Avoid running any of the original code for dragging items, to prevent possibilities
            for item duping or interaction with restricted containers
        */
        return;
        /*
            End of tes3mp change (major)
        */

        mDragAndDrop->startDrag(mSelectedItem, mSortModel, mModel, mItemView, count);
    }

    void ContainerWindow::dropItem()
    {
        if (!mModel)
            return;

        bool success = mModel->onDropItem(mDragAndDrop->mItem.mBase, mDragAndDrop->mDraggedCount);

        /*
            Start of tes3mp addition

            Send an ID_CONTAINER packet every time an item is dropped in a container
        */
        if (success)
        {
            mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
            objectList->reset();
            objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
            objectList->cell = *mPtr.getCell()->getCell();
            objectList->action = mwmp::BaseObjectList::ADD;
            objectList->containerSubAction = mwmp::BaseObjectList::DROP;

            mwmp::BaseObject baseObject = objectList->getBaseObject(mPtr);
            MWWorld::Ptr itemPtr = mDragAndDrop->mItem.mBase;
            mwmp::ContainerItem containerItem;
            containerItem.refId = itemPtr.getCellRef().getRefId();

            // Make sure we get the drag and drop count, not the count of the original item
            containerItem.count = mDragAndDrop->mDraggedCount;

            containerItem.charge = itemPtr.getCellRef().getCharge();
            containerItem.enchantmentCharge = itemPtr.getCellRef().getEnchantmentCharge();
            containerItem.soul = itemPtr.getCellRef().getSoul();

            baseObject.containerItems.push_back(containerItem);
            objectList->addObject(baseObject);
            objectList->sendContainer();

            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Sending ID_CONTAINER about\n- Ptr cellRef: %s %i-%i\n- cell: %s\n- item: %s %i, %i",
                baseObject.refId.c_str(), baseObject.refNum, baseObject.mpNum, objectList->cell.getDescription().c_str(),
                containerItem.refId.c_str(), containerItem.count, containerItem.charge);
        }
        /*
            End of tes3mp addition
        */

        /*
            Start of tes3mp change (major)

            For valid drops, avoid running the original code for the item transfer, to prevent unilateral
            item duping or interaction on this client

            Instead, finish the drag in a way that removes the items in it, and let the server's reply handle
            the rest
        */
        if (success)
            // mDragAndDrop->drop(mModel, mItemView);
            mDragAndDrop->finish(true);
        /*
            End of tes3mp change (major)
        */
    }

    void ContainerWindow::onBackgroundSelected()
    {
        if (mDragAndDrop->mIsOnDragAndDrop)
            dropItem();
    }

    void ContainerWindow::setPtr(const MWWorld::Ptr& container)
    {
        /*
            Start of tes3mp addition

            Mark this container as open for multiplayer logic purposes
        */
        mwmp::Main::get().getLocalPlayer()->storeCurrentContainer(container);
        /*
            End of tes3mp addition
        */

        mPtr = container;

        bool loot = mPtr.getClass().isActor() && mPtr.getClass().getCreatureStats(mPtr).isDead();

        if (mPtr.getClass().hasInventoryStore(mPtr))
        {
            if (mPtr.getClass().isNpc() && !loot)
            {
                // we are stealing stuff
                mModel = new PickpocketItemModel(mPtr, new InventoryItemModel(container),
                                                 !mPtr.getClass().getCreatureStats(mPtr).getKnockedDown());
            }
            else
                mModel = new InventoryItemModel(container);
        }
        else
        {
            mModel = new ContainerItemModel(container);
        }

        mDisposeCorpseButton->setVisible(loot);

        mSortModel = new SortFilterItemModel(mModel);

        mItemView->setModel (mSortModel);
        mItemView->resetScrollBars();

        MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(mCloseButton);

        setTitle(container.getClass().getName(container));
    }

    void ContainerWindow::resetReference()
    {
        ReferenceInterface::resetReference();
        mItemView->setModel(NULL);
        mModel = NULL;
        mSortModel = NULL;
    }

    void ContainerWindow::onClose()
    {
        /*
            Start of tes3mp addition

            Mark this container as closed for multiplayer logic purposes
        */
        mwmp::Main::get().getLocalPlayer()->clearCurrentContainer();
        /*
            End of tes3mp addition
        */

        WindowBase::onClose();

        if (mModel)
            mModel->onClose();
    }

    void ContainerWindow::onCloseButtonClicked(MyGUI::Widget* _sender)
    {
        MWBase::Environment::get().getWindowManager()->removeGuiMode(GM_Container);
    }

    void ContainerWindow::onTakeAllButtonClicked(MyGUI::Widget* _sender)
    {
        if(mDragAndDrop != NULL && mDragAndDrop->mIsOnDragAndDrop)
            return;

        /*
            Start of tes3mp addition

            Send an ID_CONTAINER packet every time the Take All button is used on
            a container
        */
        mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
        objectList->reset();
        objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
        objectList->cell = *mPtr.getCell()->getCell();
        objectList->action = mwmp::BaseObjectList::REMOVE;
        objectList->containerSubAction = mwmp::BaseObjectList::TAKE_ALL;
        objectList->addEntireContainer(mPtr);
        objectList->sendContainer();

        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Sending ID_CONTAINER about\n- Ptr cellRef: %s, %i-%i\n- cell: %s",
            mPtr.getCellRef().getRefId().c_str(), mPtr.getCellRef().getRefNum().mIndex, mPtr.getCellRef().getMpNum(),
            objectList->cell.getDescription().c_str());
        /*
            End of tes3mp addition
        */

        /*
            Start of tes3mp change (major)

            Avoid running any of the original code for taking all items, to prevent
            possibilities for item duping or interaction with restricted containers
        */
        return;
        /*
            End of tes3mp change (major)
        */

        // transfer everything into the player's inventory
        ItemModel* playerModel = MWBase::Environment::get().getWindowManager()->getInventoryWindow()->getModel();
        mModel->update();

        // unequip all items to avoid unequipping/reequipping
        if (mPtr.getClass().hasInventoryStore(mPtr))
        {
            MWWorld::InventoryStore& invStore = mPtr.getClass().getInventoryStore(mPtr);
            for (size_t i=0; i<mModel->getItemCount(); ++i)
            {
                const ItemStack& item = mModel->getItem(i);
                if (invStore.isEquipped(item.mBase) == false)
                    continue;

                invStore.unequipItem(item.mBase, mPtr);
            }
        }

        mModel->update();

        for (size_t i=0; i<mModel->getItemCount(); ++i)
        {
            if (i==0)
            {
                // play the sound of the first object
                MWWorld::Ptr item = mModel->getItem(i).mBase;
                std::string sound = item.getClass().getUpSoundId(item);
                MWBase::Environment::get().getWindowManager()->playSound(sound);
            }

            const ItemStack& item = mModel->getItem(i);

            if (!onTakeItem(item, item.mCount))
                break;

            mModel->moveItem(item, item.mCount, playerModel);
        }

        MWBase::Environment::get().getWindowManager()->removeGuiMode(GM_Container);
    }

    void ContainerWindow::onDisposeCorpseButtonClicked(MyGUI::Widget *sender)
    {
        if(mDragAndDrop == NULL || !mDragAndDrop->mIsOnDragAndDrop)
        {
            onTakeAllButtonClicked(mTakeButton);
            
            if (mPtr.getClass().isPersistent(mPtr))
                MWBase::Environment::get().getWindowManager()->messageBox("#{sDisposeCorpseFail}");
            /*
                Start of tes3mp change (major)

                Instead of deleting the corpse on this client, simply send an ID_OBJECT_DELETE
                packet to the server as a request for the deletion
            */
            else
            {
                mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
                objectList->reset();
                objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
                objectList->addObjectDelete(mPtr);
                objectList->sendObjectDelete();
            }
            /*
                End of tes3mp change (major)
            */
        }
    }

    void ContainerWindow::onReferenceUnavailable()
    {
        MWBase::Environment::get().getWindowManager()->removeGuiMode(GM_Container);
    }

    bool ContainerWindow::onTakeItem(const ItemStack &item, int count)
    {
        return mModel->onTakeItem(item.mBase, count);
    }

    /*
        Start of tes3mp addition

        Make it possible to check from elsewhere whether there is currently an
        item being dragged in the container window
    */
    bool ContainerWindow::isOnDragAndDrop()
    {
        return mDragAndDrop->mIsOnDragAndDrop;
    }
    /*
        End of tes3mp addition
    */

    /*
        Start of tes3mp addition

        Make it possible to drag a specific item Ptr instead of having to rely
        on an index that may have changed in the meantime, for drags that
        require approval from the server
    */
    bool ContainerWindow::dragItemByPtr(const MWWorld::Ptr& itemPtr, int dragCount)
    {
        ItemModel::ModelIndex newIndex = -1;
        for (unsigned int i = 0; i < mModel->getItemCount(); ++i)
        {
            if (mModel->getItem(i).mBase == itemPtr)
            {
                newIndex = i;
                break;
            }
        }

        if (newIndex != -1)
        {
            mDragAndDrop->startDrag(newIndex, mSortModel, mModel, mItemView, dragCount);
            return true;
        }

        return false;
    }
    /*
        End of tes3mp addition
    */
}
