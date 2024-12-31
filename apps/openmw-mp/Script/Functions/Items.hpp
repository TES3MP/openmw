#ifndef OPENMW_ITEMAPI_HPP
#define OPENMW_ITEMAPI_HPP

#define ITEMAPI \
    {"ClearInventoryChanges",                 ItemFunctions::ClearInventoryChanges},\
    \
    {"GetEquipmentSize",                      ItemFunctions::GetEquipmentSize},\
    {"GetEquipmentChangesSize",               ItemFunctions::GetEquipmentChangesSize},\
    {"GetInventoryChangesSize",               ItemFunctions::GetInventoryChangesSize},\
    {"GetInventoryChangesAction",             ItemFunctions::GetInventoryChangesAction},\
    \
    {"SetInventoryChangesAction",             ItemFunctions::SetInventoryChangesAction},\
    \
    {"EquipItem",                             ItemFunctions::EquipItem},\
    {"UnequipItem",                           ItemFunctions::UnequipItem},\
    \
    {"AddItemChange",                         ItemFunctions::AddItemChange},\
    \
    {"HasItemEquipped",                       ItemFunctions::HasItemEquipped},\
    \
    {"GetEquipmentChangesSlot",                ItemFunctions::GetEquipmentChangesSlot},\
    {"GetEquipmentItemRefId",                 ItemFunctions::GetEquipmentItemRefId},\
    {"GetEquipmentItemCount",                 ItemFunctions::GetEquipmentItemCount},\
    {"GetEquipmentItemCharge",                ItemFunctions::GetEquipmentItemCharge},\
    {"GetEquipmentItemEnchantmentCharge",     ItemFunctions::GetEquipmentItemEnchantmentCharge},\
    \
    {"GetInventoryItemRefId",                 ItemFunctions::GetInventoryItemRefId},\
    {"GetInventoryItemCount",                 ItemFunctions::GetInventoryItemCount},\
    {"GetInventoryItemCharge",                ItemFunctions::GetInventoryItemCharge},\
    {"GetInventoryItemEnchantmentCharge",     ItemFunctions::GetInventoryItemEnchantmentCharge},\
    {"GetInventoryItemSoul",                  ItemFunctions::GetInventoryItemSoul},\
    \
    {"GetUsedItemRefId",                      ItemFunctions::GetUsedItemRefId},\
    {"GetUsedItemCount",                      ItemFunctions::GetUsedItemCount},\
    {"GetUsedItemCharge",                     ItemFunctions::GetUsedItemCharge},\
    {"GetUsedItemEnchantmentCharge",          ItemFunctions::GetUsedItemEnchantmentCharge},\
    {"GetUsedItemSoul",                       ItemFunctions::GetUsedItemSoul},\
    \
    {"SendEquipment",                         ItemFunctions::SendEquipment},\
    {"SendInventoryChanges",                  ItemFunctions::SendInventoryChanges},\
    {"SendItemUse",                           ItemFunctions::SendItemUse},\
    {"SetUsedItem",                           ItemFunctions::SetUsedItem},\
    \
    {"InitializeInventoryChanges",            ItemFunctions::InitializeInventoryChanges},\
    {"AddItem",                               ItemFunctions::AddItem}

class ItemFunctions
{
public:

    /**
    * \brief Clear the last recorded inventory changes for a player.
    *
    * This is used to initialize the sending of new PlayerInventory packets.
    *
    * \param pid The player ID whose inventory changes should be used.
    * \return void
    */
    static void ClearInventoryChanges(unsigned short pid) noexcept;

    /**
    * \brief Get the number of slots used for equipment.
    *
    * The number is 19 before any dehardcoding is done in OpenMW.
    *
    * \return The number of slots.
    */
    static int GetEquipmentSize() noexcept;

    /**
    * \brief Get the number of indexes in a player's latest equipment changes.
    *
    * \param pid The player ID whose equipment changes should be used.
    * \return The number of indexes.
    */
    static unsigned int GetEquipmentChangesSize(unsigned short pid) noexcept;

    /**
    * \brief Get the number of indexes in a player's latest inventory changes.
    *
    * \param pid The player ID whose inventory changes should be used.
    * \return The number of indexes.
    */
    static unsigned int GetInventoryChangesSize(unsigned short pid) noexcept;

    /**
    * \brief Get the action type used in a player's latest inventory changes.
    *
    * \param pid The player ID whose inventory changes should be used.
    * \return The action type (0 for SET, 1 for ADD, 2 for REMOVE).
    */
    static unsigned int GetInventoryChangesAction(unsigned short pid) noexcept;

    /**
    * \brief Set the action type in a player's inventory changes.
    *
    * \param pid The player ID whose inventory changes should be used.
    * \param action The action (0 for SET, 1 for ADD, 2 for REMOVE).
    * \return void
    */
    static void SetInventoryChangesAction(unsigned short pid, unsigned char action) noexcept;

    /**
    * \brief Equip an item in a certain slot of the equipment of a player.
    *
    * \param pid The player ID.
    * \param slot The equipment slot.
    * \param refId The refId of the item.
    * \param count The count of the item.
    * \param charge The charge of the item.
    * \param enchantmentCharge The enchantment charge of the item.
    * \return void
    */
    static void EquipItem(unsigned short pid, unsigned short slot, const char* refId, unsigned int count, int charge,
        double enchantmentCharge = -1) noexcept;
    
    /**
    * \brief Unequip the item in a certain slot of the equipment of a player.
    *
    * \param pid The player ID.
    * \param slot The equipment slot.
    * \return void
    */
    static void UnequipItem(unsigned short pid, unsigned short slot) noexcept;

    /**
    * \brief Add an item change to a player's inventory changes.
    *
    * \param pid The player ID.
    * \param refId The refId of the item.
    * \param count The count of the item.
    * \param charge The charge of the item.
    * \param enchantmentCharge The enchantment charge of the item.
    * \param soul The soul of the item.
    * \return void
    */
    static void AddItemChange(unsigned short pid, const char* refId, unsigned int count, int charge,
        double enchantmentCharge, const char* soul) noexcept;

    /**
    * \brief Check whether a player has equipped an item with a certain refId in any slot.
    *
    * \param pid The player ID.
    * \param refId The refId of the item.
    * \return Whether the player has the item equipped.
    */
    static bool HasItemEquipped(unsigned short pid, const char* refId);

    /**
    * \brief Get the slot used for the equipment item at a specific index in the most recent
    * equipment changes.
    *
    * \param pid The player ID.
    * \param changeIndex The index of the equipment change.
    * \return The slot.
    */
    static int GetEquipmentChangesSlot(unsigned short pid, unsigned int changeIndex) noexcept;

    /**
    * \brief Get the refId of the item in a certain slot of the equipment of a player.
    *
    * \param pid The player ID.
    * \param slot The slot of the equipment item.
    * \return The refId.
    */
    static const char *GetEquipmentItemRefId(unsigned short pid, unsigned short slot) noexcept;

    /**
    * \brief Get the count of the item in a certain slot of the equipment of a player.
    *
    * \param pid The player ID.
    * \param slot The slot of the equipment item.
    * \return The item count.
    */
    static int GetEquipmentItemCount(unsigned short pid, unsigned short slot) noexcept;

    /**
    * \brief Get the charge of the item in a certain slot of the equipment of a player.
    *
    * \param pid The player ID.
    * \param slot The slot of the equipment item.
    * \return The charge.
    */
    static int GetEquipmentItemCharge(unsigned short pid, unsigned short slot) noexcept;

    /**
    * \brief Get the enchantment charge of the item in a certain slot of the equipment of
    *        a player.
    *
    * \param pid The player ID.
    * \param slot The slot of the equipment item.
    * \return The enchantment charge.
    */
    static double GetEquipmentItemEnchantmentCharge(unsigned short pid, unsigned short slot) noexcept;

    /**
    * \brief Get the refId of the item at a certain index in a player's latest inventory
    *        changes.
    *
    * \param pid The player ID whose inventory changes should be used.
    * \param index The index of the inventory item.
    * \return The refId.
    */
    static const char *GetInventoryItemRefId(unsigned short pid, unsigned int index) noexcept;

    /**
    * \brief Get the count of the item at a certain index in a player's latest inventory
    *        changes.
    *
    * \param pid The player ID whose inventory changes should be used.
    * \param index The index of the inventory item.
    * \return The item count.
    */
    static int GetInventoryItemCount(unsigned short pid, unsigned int index) noexcept;

    /**
    * \brief Get the charge of the item at a certain index in a player's latest inventory
    *        changes.
    *
    * \param pid The player ID whose inventory changes should be used.
    * \param index The index of the inventory item.
    * \return The charge.
    */
    static int GetInventoryItemCharge(unsigned short pid, unsigned int index) noexcept;

    /**
    * \brief Get the enchantment charge of the item at a certain index in a player's
    *        latest inventory changes.
    *
    * \param pid The player ID whose inventory changes should be used.
    * \param index The index of the inventory item.
    * \return The enchantment charge.
    */
    static double GetInventoryItemEnchantmentCharge(unsigned short pid, unsigned int index) noexcept;

    /**
    * \brief Get the soul of the item at a certain index in a player's latest inventory
    *        changes.
    *
    * \param pid The player ID whose inventory changes should be used.
    * \param index The index of the inventory item.
    * \return The soul.
    */
    static const char *GetInventoryItemSoul(unsigned short pid, unsigned int index) noexcept;

    /**
    * \brief Get the refId of the item last used by a player.
    *
    * \param pid The player ID.
    * \return The refId.
    */
    static const char *GetUsedItemRefId(unsigned short pid) noexcept;

    /**
    * \brief Get the count of the item last used by a player.
    *
    * \param pid The player ID.
    * \return The item count.
    */
    static int GetUsedItemCount(unsigned short pid) noexcept;

    /**
    * \brief Get the charge of the item last used by a player.
    *
    * \param pid The player ID.
    * \return The charge.
    */
    static int GetUsedItemCharge(unsigned short pid) noexcept;

    /**
    * \brief Get the enchantment charge of the item last used by a player.
    *
    * \param pid The player ID.
    * \return The enchantment charge.
    */
    static double GetUsedItemEnchantmentCharge(unsigned short pid) noexcept;

    /**
    * \brief Get the soul of the item last used by a player.
    *
    * \param pid The player ID.
    * \return The soul.
    */
    static const char *GetUsedItemSoul(unsigned short pid) noexcept;

    /**
    * \brief Send a PlayerEquipment packet with a player's equipment.
    *
    * It is always sent to all players.
    *
    * \param pid The player ID whose equipment should be sent.
    * \return void
    */
    static void SendEquipment(unsigned short pid) noexcept;

    /**
    * \brief Send a PlayerInventory packet with a player's recorded inventory changes.
    *
    * \param pid The player ID whose inventory changes should be used.
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendInventoryChanges(unsigned short pid, bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send a PlayerItemUse causing a player to use their recorded usedItem.
    *
    * \param pid The player ID affected.
    * \return void
    */
    static void SendItemUse(unsigned short pid) noexcept;

    /**
    * \brief Update a player's recorded usedItem. Only usable on equipped items.
    *
    * \param pid The player ID affected.
    * \param refId The name of the equipped item.
    * \return bool
    */
    static bool SetUsedItem(unsigned short pid, const char* refId) noexcept;

    // All methods below are deprecated versions of methods from above

    static void InitializeInventoryChanges(unsigned short pid) noexcept;

    static void AddItem(unsigned short pid, const char* refId, unsigned int count, int charge,
        double enchantmentCharge, const char* soul) noexcept;

private:

};

#endif //OPENMW_ITEMAPI_HPP
