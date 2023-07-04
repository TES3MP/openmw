#ifndef MGUI_CONTAINER_H
#define MGUI_CONTAINER_H

#include "referenceinterface.hpp"
#include "windowbase.hpp"

#include "itemmodel.hpp"

namespace MyGUI
{
    class Gui;
    class Widget;
}

namespace MWGui
{
    class ContainerWindow;
    class ItemView;
    class SortFilterItemModel;
}

namespace MWGui
{
    class ContainerWindow : public WindowBase, public ReferenceInterface
    {
    public:
        ContainerWindow(DragAndDrop* dragAndDrop);

        void setPtr(const MWWorld::Ptr& container) override;
        void onClose() override;
        void clear() override { resetReference(); }

        void onFrame(float dt) override { checkReferenceAvailable(); }

        void resetReference() override;

<<<<<<< HEAD
        /*
            Start of tes3mp addition

            Make it possible to check from elsewhere whether there is currently an
            item being dragged in the container window
        */
        bool isOnDragAndDrop();
        /*
            End of tes3mp addition
        */

        /*
            Start of tes3mp addition

            Make it possible to drag a specific item Ptr instead of having to rely
            on an index that may have changed in the meantime, for drags that
            require approval from the server
        */
        bool dragItemByPtr(const MWWorld::Ptr& itemPtr, int dragCount);
        /*
            End of tes3mp addition
        */
=======
        void onDeleteCustomData(const MWWorld::Ptr& ptr) override;

        void treatNextOpenAsLoot() { mTreatNextOpenAsLoot = true; }
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

    private:
        DragAndDrop* mDragAndDrop;

        MWGui::ItemView* mItemView;
        SortFilterItemModel* mSortModel;
        ItemModel* mModel;
        int mSelectedItem;
        bool mTreatNextOpenAsLoot;
        MyGUI::Button* mDisposeCorpseButton;
        MyGUI::Button* mTakeButton;
        MyGUI::Button* mCloseButton;

        void onItemSelected(int index);
        void onBackgroundSelected();
        void dragItem(MyGUI::Widget* sender, int count);
        void dropItem();
        void onCloseButtonClicked(MyGUI::Widget* _sender);
        void onTakeAllButtonClicked(MyGUI::Widget* _sender);
        void onDisposeCorpseButtonClicked(MyGUI::Widget* sender);

        /// @return is taking the item allowed?
        bool onTakeItem(const ItemStack& item, int count);

        void onReferenceUnavailable() override;
    };
}
#endif // CONTAINER_H
