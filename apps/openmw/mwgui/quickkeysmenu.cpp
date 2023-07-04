#include "quickkeysmenu.hpp"

#include <MyGUI_Button.h>
#include <MyGUI_EditBox.h>
#include <MyGUI_Gui.h>
#include <MyGUI_ImageBox.h>
#include <MyGUI_RenderManager.h>

#include <components/esm3/esmwriter.hpp>
#include <components/esm3/loadmgef.hpp>
#include <components/esm3/quickkeys.hpp>
#include <components/misc/resourcehelpers.hpp>
#include <components/resource/resourcesystem.hpp>

<<<<<<< HEAD
/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include <components/openmw-mp/TimedLog.hpp>
#include "../mwmp/Main.hpp"
#include "../mwmp/LocalPlayer.hpp"
/*
    End of tes3mp addition
*/

#include "../mwworld/inventorystore.hpp"
=======
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
#include "../mwworld/class.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/player.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/world.hpp"

#include "../mwmechanics/actorutil.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwmechanics/spellutil.hpp"

#include "itemselection.hpp"
#include "itemwidget.hpp"
#include "sortfilteritemmodel.hpp"
#include "spellview.hpp"

namespace MWGui
{

    QuickKeysMenu::QuickKeysMenu()
        : WindowBase("openmw_quickkeys_menu.layout")
        , mKey(std::vector<keyData>(10))
        , mSelected(nullptr)
        , mActivated(nullptr)

    {
        getWidget(mOkButton, "OKButton");
        getWidget(mInstructionLabel, "InstructionLabel");

        mMainWidget->setSize(mMainWidget->getWidth(),
            mMainWidget->getHeight() + (mInstructionLabel->getTextSize().height - mInstructionLabel->getHeight()));

        mOkButton->eventMouseButtonClick += MyGUI::newDelegate(this, &QuickKeysMenu::onOkButtonClicked);
        center();

        for (int i = 0; i < 10; ++i)
        {
            mKey[i].index = i + 1;
            getWidget(mKey[i].button, "QuickKey" + MyGUI::utility::toString(i + 1));
            mKey[i].button->eventMouseButtonClick += MyGUI::newDelegate(this, &QuickKeysMenu::onQuickKeyButtonClicked);

            unassign(&mKey[i]);
        }
    }

    void QuickKeysMenu::clear()
    {
        mActivated = nullptr;

        for (int i = 0; i < 10; ++i)
        {
            unassign(&mKey[i]);
        }
    }

    inline void QuickKeysMenu::validate(int index)
    {
        MWWorld::Ptr player = MWMechanics::getPlayer();
        MWWorld::InventoryStore& store = player.getClass().getInventoryStore(player);
        switch (mKey[index].type)
        {
            case ESM::QuickKeys::Type::Unassigned:
            case ESM::QuickKeys::Type::HandToHand:
            case ESM::QuickKeys::Type::Magic:
                break;
            case ESM::QuickKeys::Type::Item:
            case ESM::QuickKeys::Type::MagicItem:
            {
                MWWorld::Ptr item = *mKey[index].button->getUserData<MWWorld::Ptr>();
                // Make sure the item is available and is not broken
                if (item.isEmpty() || item.getRefData().getCount() < 1
                    || (item.getClass().hasItemHealth(item) && item.getClass().getItemHealth(item) <= 0))
                {
                    // Try searching for a compatible replacement
                    item = store.findReplacement(mKey[index].id);

                    if (!item.isEmpty())
                        mKey[index].button->setUserData(MWWorld::Ptr(item));

                    break;
                }
            }
        }
    }

    void QuickKeysMenu::onOpen()
    {
        WindowBase::onOpen();

        // Quick key index
        for (int index = 0; index < 10; ++index)
        {
            validate(index);
        }
    }

    void QuickKeysMenu::onClose()
    {
        WindowBase::onClose();

        if (mAssignDialog)
            mAssignDialog->setVisible(false);
        if (mItemSelectionDialog)
            mItemSelectionDialog->setVisible(false);
        if (mMagicSelectionDialog)
            mMagicSelectionDialog->setVisible(false);
    }

    void QuickKeysMenu::unassign(keyData* key)
    {
        key->button->clearUserStrings();
        key->button->setItem(MWWorld::Ptr());

        while (key->button->getChildCount()) // Destroy number label
            MyGUI::Gui::getInstance().destroyWidget(key->button->getChildAt(0));

        if (key->index == 10)
        {
            key->type = ESM::QuickKeys::Type::HandToHand;

            MyGUI::ImageBox* image = key->button->createWidget<MyGUI::ImageBox>(
                "ImageBox", MyGUI::IntCoord(14, 13, 32, 32), MyGUI::Align::Default);

            image->setImageTexture("icons\\k\\stealth_handtohand.dds");
            image->setNeedMouseFocus(false);
        }
        else
        {
            key->type = ESM::QuickKeys::Type::Unassigned;
            key->id = ESM::RefId();
            key->name.clear();

            MyGUI::TextBox* textBox = key->button->createWidgetReal<MyGUI::TextBox>(
                "SandText", MyGUI::FloatCoord(0, 0, 1, 1), MyGUI::Align::Default);

            textBox->setTextAlign(MyGUI::Align::Center);
            textBox->setCaption(MyGUI::utility::toString(key->index));
            textBox->setNeedMouseFocus(false);
        }

        /*
            Start of tes3mp addition

            Send a PLAYER_QUICKKEYS packet whenever a key is unassigned, but only if the player
            is logged in on the server, so as to avoid doing anything doing at startup when all
            quick keys get unassigned by default
        */
        if (mwmp::Main::get().getLocalPlayer()->isLoggedIn() && !mwmp::Main::get().getLocalPlayer()->isReceivingQuickKeys)
        {
            mwmp::Main::get().getLocalPlayer()->sendQuickKey(key->index, Type_Unassigned);
        }
        /*
            End of tes3mp addition
        */
    }

    /*
        Start of tes3mp addition

        Allow unassigning an index directly from elsewhere in the code
    */
    void QuickKeysMenu::unassignIndex(int index)
    {
        unassign(&mKey[index]);
    }
    /*
        End of tes3mp addition
    */

    void QuickKeysMenu::onQuickKeyButtonClicked(MyGUI::Widget* sender)
    {
        int index = -1;
        for (int i = 0; i < 10; ++i)
        {
            if (sender == mKey[i].button || sender->getParent() == mKey[i].button)
            {
                index = i;
                break;
            }
        }
        assert(index != -1);
        if (index < 0)
        {
            mSelected = nullptr;
            return;
        }

        mSelected = &mKey[index];

        // prevent reallocation of zero key from ESM::QuickKeys::Type::HandToHand
        if (mSelected->index == 10)
            return;

        // open assign dialog
        if (!mAssignDialog)
            mAssignDialog = std::make_unique<QuickKeysMenuAssign>(this);

        mAssignDialog->setVisible(true);
    }

    void QuickKeysMenu::onOkButtonClicked(MyGUI::Widget* sender)
    {
        MWBase::Environment::get().getWindowManager()->removeGuiMode(GM_QuickKeysMenu);
    }

    void QuickKeysMenu::onItemButtonClicked(MyGUI::Widget* sender)
    {
        if (!mItemSelectionDialog)
        {
            mItemSelectionDialog = std::make_unique<ItemSelectionDialog>("#{sQuickMenu6}");
            mItemSelectionDialog->eventItemSelected += MyGUI::newDelegate(this, &QuickKeysMenu::onAssignItem);
            mItemSelectionDialog->eventDialogCanceled += MyGUI::newDelegate(this, &QuickKeysMenu::onAssignItemCancel);
        }
        mItemSelectionDialog->setVisible(true);
        mItemSelectionDialog->openContainer(MWMechanics::getPlayer());
        mItemSelectionDialog->setFilter(SortFilterItemModel::Filter_OnlyUsableItems);

        mAssignDialog->setVisible(false);
    }

    void QuickKeysMenu::onMagicButtonClicked(MyGUI::Widget* sender)
    {
        if (!mMagicSelectionDialog)
        {
            mMagicSelectionDialog = std::make_unique<MagicSelectionDialog>(this);
        }
        mMagicSelectionDialog->setVisible(true);

        mAssignDialog->setVisible(false);
    }

    void QuickKeysMenu::onUnassignButtonClicked(MyGUI::Widget* sender)
    {
        unassign(mSelected);
        mAssignDialog->setVisible(false);
    }

    void QuickKeysMenu::onCancelButtonClicked(MyGUI::Widget* sender)
    {
        mAssignDialog->setVisible(false);
    }

    void QuickKeysMenu::onAssignItem(MWWorld::Ptr item)
    {
        assert(mSelected);

        while (mSelected->button->getChildCount()) // Destroy number label
            MyGUI::Gui::getInstance().destroyWidget(mSelected->button->getChildAt(0));

        mSelected->type = ESM::QuickKeys::Type::Item;
        mSelected->id = item.getCellRef().getRefId();
        mSelected->name = item.getClass().getName(item);

        mSelected->button->setItem(item, ItemWidget::Barter);
        mSelected->button->setUserString("ToolTipType", "ItemPtr");
        mSelected->button->setUserData(item);

        if (mItemSelectionDialog)
            mItemSelectionDialog->setVisible(false);

        /*
            Start of tes3mp addition

            Send a PlayerQuickKeys packet whenever a key is assigned to an item
            by a player, not by a packet received from the server
        */
        if (!mwmp::Main::get().getLocalPlayer()->isReceivingQuickKeys)
            mwmp::Main::get().getLocalPlayer()->sendQuickKey(mSelected->index, Type_Item, item.getCellRef().getRefId());
        /*
            End of tes3mp addition
        */
    }

    void QuickKeysMenu::onAssignItemCancel()
    {
        mItemSelectionDialog->setVisible(false);
    }

    void QuickKeysMenu::onAssignMagicItem(MWWorld::Ptr item)
    {
        assert(mSelected);

        while (mSelected->button->getChildCount()) // Destroy number label
            MyGUI::Gui::getInstance().destroyWidget(mSelected->button->getChildAt(0));

        mSelected->type = ESM::QuickKeys::Type::MagicItem;
        mSelected->id = item.getCellRef().getRefId();
        mSelected->name = item.getClass().getName(item);

        float scale = 1.f;
        MyGUI::ITexture* texture
            = MyGUI::RenderManager::getInstance().getTexture("textures\\menu_icon_select_magic_magic.dds");
        if (texture)
            scale = texture->getHeight() / 64.f;

        mSelected->button->setFrame(
            "textures\\menu_icon_select_magic_magic.dds", MyGUI::IntCoord(0, 0, 44 * scale, 44 * scale));
        mSelected->button->setIcon(item);

        mSelected->button->setUserString("ToolTipType", "ItemPtr");
        mSelected->button->setUserData(MWWorld::Ptr(item));

        if (mMagicSelectionDialog)
            mMagicSelectionDialog->setVisible(false);

        /*
            Start of tes3mp addition

            Send a PLAYER_QUICKKEYS packet whenever a key is assigned to an item's magic
        */
        if (!mwmp::Main::get().getLocalPlayer()->isReceivingQuickKeys)
            mwmp::Main::get().getLocalPlayer()->sendQuickKey(mSelected->index, Type_MagicItem, item.getCellRef().getRefId());
        /*
            End of tes3mp addition
        */
    }

    void QuickKeysMenu::onAssignMagic(const ESM::RefId& spellId)
    {
        assert(mSelected);
        while (mSelected->button->getChildCount()) // Destroy number label
            MyGUI::Gui::getInstance().destroyWidget(mSelected->button->getChildAt(0));

        const MWWorld::ESMStore& esmStore = *MWBase::Environment::get().getESMStore();
        const ESM::Spell* spell = esmStore.get<ESM::Spell>().find(spellId);

        mSelected->type = ESM::QuickKeys::Type::Magic;
        mSelected->id = spellId;
        mSelected->name = spell->mName;

        mSelected->button->setItem(MWWorld::Ptr());
        mSelected->button->setUserString("ToolTipType", "Spell");
        mSelected->button->setUserString("Spell", spellId.serialize());

        // use the icon of the first effect
        const ESM::MagicEffect* effect = esmStore.get<ESM::MagicEffect>().find(spell->mEffects.mList.front().mEffectID);

        std::string path = effect->mIcon;
        std::replace(path.begin(), path.end(), '/', '\\');
        int slashPos = path.rfind('\\');
        path.insert(slashPos + 1, "b_");
        path = Misc::ResourceHelpers::correctIconPath(path, MWBase::Environment::get().getResourceSystem()->getVFS());

        float scale = 1.f;
        MyGUI::ITexture* texture
            = MyGUI::RenderManager::getInstance().getTexture("textures\\menu_icon_select_magic.dds");
        if (texture)
            scale = texture->getHeight() / 64.f;

        mSelected->button->setFrame(
            "textures\\menu_icon_select_magic.dds", MyGUI::IntCoord(0, 0, 44 * scale, 44 * scale));
        mSelected->button->setIcon(path);

        if (mMagicSelectionDialog)
            mMagicSelectionDialog->setVisible(false);

        /*
            Start of tes3mp addition

            Send a PLAYER_QUICKKEYS packet whenever a key is assigned to a spell
        */
        if (!mwmp::Main::get().getLocalPlayer()->isReceivingQuickKeys)
            mwmp::Main::get().getLocalPlayer()->sendQuickKey(mSelected->index, Type_Magic, spellId);
        /*
            End of tes3mp addition
        */
    }

    void QuickKeysMenu::onAssignMagicCancel()
    {
        mMagicSelectionDialog->setVisible(false);
    }

    void QuickKeysMenu::updateActivatedQuickKey()
    {
        // there is no delayed action, nothing to do.
        if (!mActivated)
            return;

        activateQuickKey(mActivated->index);
    }

    void QuickKeysMenu::activateQuickKey(int index)
    {
        assert(index >= 1 && index <= 10);

        keyData* key = &mKey[index - 1];

        MWWorld::Ptr player = MWMechanics::getPlayer();
        MWWorld::InventoryStore& store = player.getClass().getInventoryStore(player);
        const MWMechanics::CreatureStats& playerStats = player.getClass().getCreatureStats(player);

        validate(index - 1);

        // Delay action executing,
        // if player is busy for now (casting a spell, attacking someone, etc.)
        bool isDelayNeeded = MWBase::Environment::get().getMechanicsManager()->isAttackingOrSpell(player)
            || playerStats.getKnockedDown() || playerStats.getHitRecovery();

        bool godmode = MWBase::Environment::get().getWorld()->getGodModeState();
        bool isReturnNeeded = (!godmode && playerStats.isParalyzed()) || playerStats.isDead();

        if (isReturnNeeded)
        {
            return;
        }
        else if (isDelayNeeded)
        {
            mActivated = key;
            return;
        }
        else
        {
            mActivated = nullptr;
        }

        if (key->type == ESM::QuickKeys::Type::Item || key->type == ESM::QuickKeys::Type::MagicItem)
        {
            MWWorld::Ptr item = *key->button->getUserData<MWWorld::Ptr>();

            MWWorld::ContainerStoreIterator it = store.begin();
            for (; it != store.end(); ++it)
            {
                if (*it == item)
                    break;
            }
            if (it == store.end())
                item = nullptr;

            // check the item is available and not broken
            if (item.isEmpty() || item.getRefData().getCount() < 1
                || (item.getClass().hasItemHealth(item) && item.getClass().getItemHealth(item) <= 0))
            {
                item = store.findReplacement(key->id);

                if (item.isEmpty() || item.getRefData().getCount() < 1)
                {
                    MWBase::Environment::get().getWindowManager()->messageBox("#{sQuickMenu5} " + key->name);

                    return;
                }
            }

            if (key->type == ESM::QuickKeys::Type::Item)
            {
<<<<<<< HEAD
                bool isWeapon = item.getTypeName() == typeid(ESM::Weapon).name();
                bool isTool = item.getTypeName() == typeid(ESM::Probe).name() ||
                    item.getTypeName() == typeid(ESM::Lockpick).name();

                // delay weapon switching if player is busy
                if (isDelayNeeded && (isWeapon || isTool))
                {
                    mActivated = key;
                    return;
                }
                else if (isReturnNeeded && (isWeapon || isTool))
                {
                    return;
                }

                /*
                    Start of tes3mp change (major)

                    Instead of unilaterally using an item, send an ID_PLAYER_ITEM_USE packet and let the server
                    decide if the item actually gets used
                */

                /*
=======
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
                if (!store.isEquipped(item))
                    MWBase::Environment::get().getWindowManager()->useItem(item);
                MWWorld::ConstContainerStoreIterator rightHand
                    = store.getSlot(MWWorld::InventoryStore::Slot_CarriedRight);
                // change draw state only if the item is in player's right hand
                if (rightHand != store.end() && item == *rightHand)
                {
                    MWBase::Environment::get().getWorld()->getPlayer().setDrawState(MWMechanics::DrawState::Weapon);
                }
                */

                bool shouldDraw = isWeapon || isTool;
                
                if (!store.isEquipped(item))
                {
                    mwmp::Main::get().getLocalPlayer()->sendItemUse(item, false, shouldDraw ? MWMechanics::DrawState_Weapon : MWMechanics::DrawState_Nothing);
                }
                /*
                    End of tes3mp change (major)
                */
            }
            else if (key->type == ESM::QuickKeys::Type::MagicItem)
            {
                // equip, if it can be equipped and isn't yet equipped

                /*
                    Start of tes3mp change (major)

                    Instead of unilaterally using an item, send an ID_PLAYER_ITEM_USE packet and let the server
                    decide if the item actually gets used
                */
                /*
                if (!item.getClass().getEquipmentSlots(item).first.empty() && !store.isEquipped(item))
                {
                    MWBase::Environment::get().getWindowManager()->useItem(item);

                    // make sure that item was successfully equipped
                    if (!store.isEquipped(item))
                        return;
                }
                
                store.setSelectedEnchantItem(it);
<<<<<<< HEAD
                MWBase::Environment::get().getWorld()->getPlayer().setDrawState(MWMechanics::DrawState_Spell);
                */

                mwmp::Main::get().getLocalPlayer()->sendItemUse(item, true, MWMechanics::DrawState_Spell);
                /*
                    End of tes3mp change (major)
                */
=======
                MWBase::Environment::get().getWorld()->getPlayer().setDrawState(MWMechanics::DrawState::Spell);
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
            }
        }
        else if (key->type == ESM::QuickKeys::Type::Magic)
        {
            const ESM::RefId& spellId = key->id;

            // Make sure the player still has this spell
            MWMechanics::CreatureStats& stats = player.getClass().getCreatureStats(player);
            MWMechanics::Spells& spells = stats.getSpells();

            if (!spells.hasSpell(spellId))
            {
                MWBase::Environment::get().getWindowManager()->messageBox("#{sQuickMenu5} " + key->name);
                return;
            }

            store.setSelectedEnchantItem(store.end());
<<<<<<< HEAD
            MWBase::Environment::get().getWindowManager()
                ->setSelectedSpell(spellId, int(MWMechanics::getSpellSuccessChance(spellId, player)));
            MWBase::Environment::get().getWorld()->getPlayer().setDrawState(MWMechanics::DrawState_Spell);

            /*
                Start of tes3mp addition

                Send a PlayerMiscellaneous packet with the player's new selected spell
            */
            mwmp::Main::get().getLocalPlayer()->sendSelectedSpell(spellId);
            /*
                End of tes3mp addition
            */
=======
            MWBase::Environment::get().getWindowManager()->setSelectedSpell(
                spellId, int(MWMechanics::getSpellSuccessChance(spellId, player)));
            MWBase::Environment::get().getWorld()->getPlayer().setDrawState(MWMechanics::DrawState::Spell);
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
        }
        else if (key->type == ESM::QuickKeys::Type::HandToHand)
        {
            store.unequipSlot(MWWorld::InventoryStore::Slot_CarriedRight);
            MWBase::Environment::get().getWorld()->getPlayer().setDrawState(MWMechanics::DrawState::Weapon);
        }
    }

    /*
        Start of tes3mp addition

        Make it possible to add quickKeys from elsewhere in the code
    */
    void QuickKeysMenu::setSelectedIndex(int index)
    {
        mSelected = &mKey[index];
    }
    /*
        End of tes3mp addition
    */

    // ---------------------------------------------------------------------------------------------------------

    QuickKeysMenuAssign::QuickKeysMenuAssign(QuickKeysMenu* parent)
        : WindowModal("openmw_quickkeys_menu_assign.layout")
        , mParent(parent)
    {
        getWidget(mLabel, "Label");
        getWidget(mItemButton, "ItemButton");
        getWidget(mMagicButton, "MagicButton");
        getWidget(mUnassignButton, "UnassignButton");
        getWidget(mCancelButton, "CancelButton");

        mItemButton->eventMouseButtonClick += MyGUI::newDelegate(mParent, &QuickKeysMenu::onItemButtonClicked);
        mMagicButton->eventMouseButtonClick += MyGUI::newDelegate(mParent, &QuickKeysMenu::onMagicButtonClicked);
        mUnassignButton->eventMouseButtonClick += MyGUI::newDelegate(mParent, &QuickKeysMenu::onUnassignButtonClicked);
        mCancelButton->eventMouseButtonClick += MyGUI::newDelegate(mParent, &QuickKeysMenu::onCancelButtonClicked);

        int maxWidth = mLabel->getTextSize().width + 24;
        maxWidth = std::max(maxWidth, mItemButton->getTextSize().width + 24);
        maxWidth = std::max(maxWidth, mMagicButton->getTextSize().width + 24);
        maxWidth = std::max(maxWidth, mUnassignButton->getTextSize().width + 24);
        maxWidth = std::max(maxWidth, mCancelButton->getTextSize().width + 24);

        mMainWidget->setSize(maxWidth + 24, mMainWidget->getHeight());
        mLabel->setSize(maxWidth, mLabel->getHeight());

        mItemButton->setCoord((maxWidth - mItemButton->getTextSize().width - 24) / 2 + 8, mItemButton->getTop(),
            mItemButton->getTextSize().width + 24, mItemButton->getHeight());
        mMagicButton->setCoord((maxWidth - mMagicButton->getTextSize().width - 24) / 2 + 8, mMagicButton->getTop(),
            mMagicButton->getTextSize().width + 24, mMagicButton->getHeight());
        mUnassignButton->setCoord((maxWidth - mUnassignButton->getTextSize().width - 24) / 2 + 8,
            mUnassignButton->getTop(), mUnassignButton->getTextSize().width + 24, mUnassignButton->getHeight());
        mCancelButton->setCoord((maxWidth - mCancelButton->getTextSize().width - 24) / 2 + 8, mCancelButton->getTop(),
            mCancelButton->getTextSize().width + 24, mCancelButton->getHeight());

        center();
    }

    void QuickKeysMenu::write(ESM::ESMWriter& writer)
    {
        writer.startRecord(ESM::REC_KEYS);

        ESM::QuickKeys keys;

        // NB: The quick key with index 9 always has Hand-to-Hand type and must not be saved
        for (int i = 0; i < 9; ++i)
        {
            ItemWidget* button = mKey[i].button;

            const ESM::QuickKeys::Type type = mKey[i].type;

            ESM::QuickKeys::QuickKey key;
            key.mType = type;

            switch (type)
            {
                case ESM::QuickKeys::Type::Unassigned:
                case ESM::QuickKeys::Type::HandToHand:
                    break;
                case ESM::QuickKeys::Type::Item:
                case ESM::QuickKeys::Type::MagicItem:
                {
                    MWWorld::Ptr item = *button->getUserData<MWWorld::Ptr>();
                    key.mId = item.getCellRef().getRefId();
                    break;
                }
                case ESM::QuickKeys::Type::Magic:
                    key.mId = ESM::RefId::deserialize(button->getUserString("Spell"));
                    break;
            }

            keys.mKeys.push_back(key);
        }

        keys.save(writer);

        writer.endRecord(ESM::REC_KEYS);
    }

    void QuickKeysMenu::readRecord(ESM::ESMReader& reader, uint32_t type)
    {
        if (type != ESM::REC_KEYS)
            return;

        ESM::QuickKeys keys;
        keys.load(reader);

        MWWorld::Ptr player = MWMechanics::getPlayer();
        MWWorld::InventoryStore& store = player.getClass().getInventoryStore(player);

        int i = 0;
        for (ESM::QuickKeys::QuickKey& quickKey : keys.mKeys)
        {
            // NB: The quick key with index 9 always has Hand-to-Hand type and must not be loaded
            if (i >= 9)
                return;

            mSelected = &mKey[i];

            switch (quickKey.mType)
            {
                case ESM::QuickKeys::Type::Magic:
                    if (MWBase::Environment::get().getESMStore()->get<ESM::Spell>().search(quickKey.mId))
                        onAssignMagic(quickKey.mId);
                    break;
                case ESM::QuickKeys::Type::Item:
                case ESM::QuickKeys::Type::MagicItem:
                {
                    // Find the item by id
                    MWWorld::Ptr item = store.findReplacement(quickKey.mId);

                    if (item.isEmpty())
                        unassign(mSelected);
                    else
                    {
                        if (quickKey.mType == ESM::QuickKeys::Type::Item)
                            onAssignItem(item);
                        else // if (quickKey.mType == ESM::QuickKeys::Type::MagicItem)
                            onAssignMagicItem(item);
                    }

                    break;
                }
                case ESM::QuickKeys::Type::Unassigned:
                case ESM::QuickKeys::Type::HandToHand:
                    unassign(mSelected);
                    break;
            }

            ++i;
        }
    }

    // ---------------------------------------------------------------------------------------------------------

    MagicSelectionDialog::MagicSelectionDialog(QuickKeysMenu* parent)
        : WindowModal("openmw_magicselection_dialog.layout")
        , mParent(parent)
    {
        getWidget(mCancelButton, "CancelButton");
        getWidget(mMagicList, "MagicList");
        mCancelButton->eventMouseButtonClick += MyGUI::newDelegate(this, &MagicSelectionDialog::onCancelButtonClicked);

        mMagicList->setShowCostColumn(false);
        mMagicList->setHighlightSelected(false);
        mMagicList->eventSpellClicked += MyGUI::newDelegate(this, &MagicSelectionDialog::onModelIndexSelected);

        center();
    }

    void MagicSelectionDialog::onCancelButtonClicked(MyGUI::Widget* sender)
    {
        exit();
    }

    bool MagicSelectionDialog::exit()
    {
        mParent->onAssignMagicCancel();
        return true;
    }

    void MagicSelectionDialog::onOpen()
    {
        WindowModal::onOpen();

        mMagicList->setModel(new SpellModel(MWMechanics::getPlayer()));
        mMagicList->resetScrollbars();
    }

    void MagicSelectionDialog::onModelIndexSelected(SpellModel::ModelIndex index)
    {
        const Spell& spell = mMagicList->getModel()->getItem(index);
        if (spell.mType == Spell::Type_EnchantedItem)
            mParent->onAssignMagicItem(spell.mItem);
        else
            mParent->onAssignMagic(spell.mId);
    }

}
