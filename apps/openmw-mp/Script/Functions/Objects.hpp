#ifndef OPENMW_OBJECTAPI_HPP
#define OPENMW_OBJECTAPI_HPP

#define OBJECTAPI \
    {"ReadReceivedObjectList",                ObjectFunctions::ReadReceivedObjectList},\
    \
    {"ClearObjectList",                       ObjectFunctions::ClearObjectList},\
    {"SetObjectListPid",                      ObjectFunctions::SetObjectListPid},\
    \
    {"CopyReceivedObjectListToStore",         ObjectFunctions::CopyReceivedObjectListToStore},\
    \
    {"GetObjectListSize",                     ObjectFunctions::GetObjectListSize},\
    {"GetObjectListOrigin",                   ObjectFunctions::GetObjectListOrigin},\
    {"GetObjectListClientScript",             ObjectFunctions::GetObjectListClientScript},\
    {"GetObjectListAction",                   ObjectFunctions::GetObjectListAction},\
    {"GetObjectListConsoleCommand",           ObjectFunctions::GetObjectListConsoleCommand},\
    {"GetObjectListContainerSubAction",       ObjectFunctions::GetObjectListContainerSubAction},\
    \
    {"IsObjectPlayer",                        ObjectFunctions::IsObjectPlayer},\
    {"GetObjectPid",                          ObjectFunctions::GetObjectPid},\
    {"GetObjectRefId",                        ObjectFunctions::GetObjectRefId},\
    {"GetObjectRefNum",                       ObjectFunctions::GetObjectRefNum},\
    {"GetObjectMpNum",                        ObjectFunctions::GetObjectMpNum},\
    {"GetObjectCount",                        ObjectFunctions::GetObjectCount},\
    {"GetObjectCharge",                       ObjectFunctions::GetObjectCharge},\
    {"GetObjectEnchantmentCharge",            ObjectFunctions::GetObjectEnchantmentCharge},\
    {"GetObjectSoul" ,                        ObjectFunctions::GetObjectSoul},\
    {"GetObjectGoldValue",                    ObjectFunctions::GetObjectGoldValue},\
    {"GetObjectScale",                        ObjectFunctions::GetObjectScale},\
    {"GetObjectSoundId",                      ObjectFunctions::GetObjectSoundId},\
    {"GetObjectState",                        ObjectFunctions::GetObjectState},\
    {"GetObjectDoorState",                    ObjectFunctions::GetObjectDoorState},\
    {"GetObjectLockLevel",                    ObjectFunctions::GetObjectLockLevel},\
    {"GetObjectTrapSpellId",                  ObjectFunctions::GetObjectTrapSpellId},\
    {"GetObjectTrapAction",                   ObjectFunctions::GetObjectTrapAction},\
    {"GetObjectDialogueChoiceType",           ObjectFunctions::GetObjectDialogueChoiceType},\
    {"GetObjectDialogueChoiceTopic",          ObjectFunctions::GetObjectDialogueChoiceTopic},\
    {"GetObjectGoldPool",                     ObjectFunctions::GetObjectGoldPool},\
    {"GetObjectLastGoldRestockHour",          ObjectFunctions::GetObjectLastGoldRestockHour},\
    {"GetObjectLastGoldRestockDay",           ObjectFunctions::GetObjectLastGoldRestockDay},\
    \
    {"DoesObjectHavePlayerActivating",        ObjectFunctions::DoesObjectHavePlayerActivating},\
    {"GetObjectActivatingPid",                ObjectFunctions::GetObjectActivatingPid},\
    {"GetObjectActivatingRefId",              ObjectFunctions::GetObjectActivatingRefId},\
    {"GetObjectActivatingRefNum",             ObjectFunctions::GetObjectActivatingRefNum},\
    {"GetObjectActivatingMpNum",              ObjectFunctions::GetObjectActivatingMpNum},\
    {"GetObjectActivatingName",               ObjectFunctions::GetObjectActivatingName},\
    \
    {"GetObjectHitSuccess",                   ObjectFunctions::GetObjectHitSuccess},\
    {"GetObjectHitDamage",                    ObjectFunctions::GetObjectHitDamage},\
    {"GetObjectHitBlock",                     ObjectFunctions::GetObjectHitBlock},\
    {"GetObjectHitKnockdown",                 ObjectFunctions::GetObjectHitKnockdown},\
    {"DoesObjectHavePlayerHitting",           ObjectFunctions::DoesObjectHavePlayerHitting},\
    {"GetObjectHittingPid",                   ObjectFunctions::GetObjectHittingPid},\
    {"GetObjectHittingRefId",                 ObjectFunctions::GetObjectHittingRefId},\
    {"GetObjectHittingRefNum",                ObjectFunctions::GetObjectHittingRefNum},\
    {"GetObjectHittingMpNum",                 ObjectFunctions::GetObjectHittingMpNum},\
    {"GetObjectHittingName",                  ObjectFunctions::GetObjectHittingName},\
    \
    {"GetObjectSummonState",                  ObjectFunctions::GetObjectSummonState},\
    {"GetObjectSummonEffectId",               ObjectFunctions::GetObjectSummonEffectId},\
    {"GetObjectSummonSpellId",                ObjectFunctions::GetObjectSummonSpellId},\
    {"GetObjectSummonDuration",               ObjectFunctions::GetObjectSummonDuration},\
    {"DoesObjectHavePlayerSummoner",          ObjectFunctions::DoesObjectHavePlayerSummoner},\
    {"GetObjectSummonerPid",                  ObjectFunctions::GetObjectSummonerPid},\
    {"GetObjectSummonerRefId",                ObjectFunctions::GetObjectSummonerRefId},\
    {"GetObjectSummonerRefNum",               ObjectFunctions::GetObjectSummonerRefNum},\
    {"GetObjectSummonerMpNum",                ObjectFunctions::GetObjectSummonerMpNum},\
    \
    {"GetObjectPosX",                         ObjectFunctions::GetObjectPosX},\
    {"GetObjectPosY",                         ObjectFunctions::GetObjectPosY},\
    {"GetObjectPosZ",                         ObjectFunctions::GetObjectPosZ},\
    {"GetObjectRotX",                         ObjectFunctions::GetObjectRotX},\
    {"GetObjectRotY",                         ObjectFunctions::GetObjectRotY},\
    {"GetObjectRotZ",                         ObjectFunctions::GetObjectRotZ},\
    \
    {"GetVideoFilename",                      ObjectFunctions::GetVideoFilename},\
    \
    {"GetClientLocalsSize",                   ObjectFunctions::GetClientLocalsSize},\
    {"GetClientLocalInternalIndex",           ObjectFunctions::GetClientLocalInternalIndex},\
    {"GetClientLocalVariableType",            ObjectFunctions::GetClientLocalVariableType},\
    {"GetClientLocalIntValue",                ObjectFunctions::GetClientLocalIntValue},\
    {"GetClientLocalFloatValue",              ObjectFunctions::GetClientLocalFloatValue},\
    \
    {"GetContainerChangesSize",               ObjectFunctions::GetContainerChangesSize},\
    {"GetContainerItemRefId",                 ObjectFunctions::GetContainerItemRefId},\
    {"GetContainerItemCount",                 ObjectFunctions::GetContainerItemCount},\
    {"GetContainerItemCharge",                ObjectFunctions::GetContainerItemCharge},\
    {"GetContainerItemEnchantmentCharge",     ObjectFunctions::GetContainerItemEnchantmentCharge},\
    {"GetContainerItemSoul",                  ObjectFunctions::GetContainerItemSoul},\
    {"GetContainerItemActionCount",           ObjectFunctions::GetContainerItemActionCount},\
    \
    {"DoesObjectHaveContainer",               ObjectFunctions::DoesObjectHaveContainer},\
    {"IsObjectDroppedByPlayer",               ObjectFunctions::IsObjectDroppedByPlayer},\
    \
    {"SetObjectListCell",                     ObjectFunctions::SetObjectListCell},\
    {"SetObjectListAction",                   ObjectFunctions::SetObjectListAction},\
    {"SetObjectListContainerSubAction",       ObjectFunctions::SetObjectListContainerSubAction},\
    {"SetObjectListConsoleCommand",           ObjectFunctions::SetObjectListConsoleCommand},\
    \
    {"SetObjectRefId",                        ObjectFunctions::SetObjectRefId},\
    {"SetObjectRefNum",                       ObjectFunctions::SetObjectRefNum},\
    {"SetObjectMpNum",                        ObjectFunctions::SetObjectMpNum},\
    {"SetObjectCount",                        ObjectFunctions::SetObjectCount},\
    {"SetObjectCharge",                       ObjectFunctions::SetObjectCharge},\
    {"SetObjectEnchantmentCharge",            ObjectFunctions::SetObjectEnchantmentCharge},\
    {"SetObjectSoul",                         ObjectFunctions::SetObjectSoul},\
    {"SetObjectGoldValue",                    ObjectFunctions::SetObjectGoldValue},\
    {"SetObjectScale",                        ObjectFunctions::SetObjectScale},\
    {"SetObjectState",                        ObjectFunctions::SetObjectState},\
    {"SetObjectLockLevel",                    ObjectFunctions::SetObjectLockLevel},\
    {"SetObjectTrapSpellId",                  ObjectFunctions::SetObjectTrapSpellId},\
    {"SetObjectTrapAction",                   ObjectFunctions::SetObjectTrapAction},\
    {"SetObjectDialogueChoiceType",           ObjectFunctions::SetObjectDialogueChoiceType},\
    {"SetObjectDialogueChoiceTopic",          ObjectFunctions::SetObjectDialogueChoiceTopic},\
    {"SetObjectGoldPool",                     ObjectFunctions::SetObjectGoldPool},\
    {"SetObjectLastGoldRestockHour",          ObjectFunctions::SetObjectLastGoldRestockHour},\
    {"SetObjectLastGoldRestockDay",           ObjectFunctions::SetObjectLastGoldRestockDay},\
    {"SetObjectDroppedByPlayerState",         ObjectFunctions::SetObjectDroppedByPlayerState},\
    {"SetObjectPosition",                     ObjectFunctions::SetObjectPosition},\
    {"SetObjectRotation",                     ObjectFunctions::SetObjectRotation},\
    {"SetObjectSound",                        ObjectFunctions::SetObjectSound},\
    \
    {"SetObjectSummonState",                  ObjectFunctions::SetObjectSummonState},\
    {"SetObjectSummonEffectId",               ObjectFunctions::SetObjectSummonEffectId},\
    {"SetObjectSummonSpellId",                ObjectFunctions::SetObjectSummonSpellId},\
    {"SetObjectSummonDuration",               ObjectFunctions::SetObjectSummonDuration},\
    {"SetObjectSummonerPid",                  ObjectFunctions::SetObjectSummonerPid},\
    {"SetObjectSummonerRefNum",               ObjectFunctions::SetObjectSummonerRefNum},\
    {"SetObjectSummonerMpNum",                ObjectFunctions::SetObjectSummonerMpNum},\
    \
    {"SetObjectActivatingPid",                ObjectFunctions::SetObjectActivatingPid},\
    \
    {"SetObjectDoorState",                    ObjectFunctions::SetObjectDoorState},\
    {"SetObjectDoorTeleportState",            ObjectFunctions::SetObjectDoorTeleportState},\
    {"SetObjectDoorDestinationCell",          ObjectFunctions::SetObjectDoorDestinationCell},\
    {"SetObjectDoorDestinationPosition",      ObjectFunctions::SetObjectDoorDestinationPosition},\
    {"SetObjectDoorDestinationRotation",      ObjectFunctions::SetObjectDoorDestinationRotation},\
    \
    {"SetPlayerAsObject",                     ObjectFunctions::SetPlayerAsObject},\
    \
    {"SetContainerItemRefId",                 ObjectFunctions::SetContainerItemRefId},\
    {"SetContainerItemCount",                 ObjectFunctions::SetContainerItemCount},\
    {"SetContainerItemCharge",                ObjectFunctions::SetContainerItemCharge},\
    {"SetContainerItemEnchantmentCharge",     ObjectFunctions::SetContainerItemEnchantmentCharge},\
    {"SetContainerItemSoul",                  ObjectFunctions::SetContainerItemSoul},\
    \
    {"SetContainerItemActionCountByIndex",    ObjectFunctions::SetContainerItemActionCountByIndex},\
    \
    {"AddObject",                             ObjectFunctions::AddObject},\
    {"AddClientLocalInteger",                 ObjectFunctions::AddClientLocalInteger},\
    {"AddClientLocalFloat",                   ObjectFunctions::AddClientLocalFloat},\
    {"AddContainerItem",                      ObjectFunctions::AddContainerItem},\
    \
    {"SendObjectActivate",                    ObjectFunctions::SendObjectActivate},\
    {"SendObjectPlace",                       ObjectFunctions::SendObjectPlace},\
    {"SendObjectSpawn",                       ObjectFunctions::SendObjectSpawn},\
    {"SendObjectDelete",                      ObjectFunctions::SendObjectDelete},\
    {"SendObjectLock",                        ObjectFunctions::SendObjectLock},\
    {"SendObjectDialogueChoice",              ObjectFunctions::SendObjectDialogueChoice},\
    {"SendObjectMiscellaneous",               ObjectFunctions::SendObjectMiscellaneous},\
    {"SendObjectRestock",                     ObjectFunctions::SendObjectRestock},\
    {"SendObjectTrap",                        ObjectFunctions::SendObjectTrap},\
    {"SendObjectScale",                       ObjectFunctions::SendObjectScale},\
    {"SendObjectSound",                       ObjectFunctions::SendObjectSound},\
    {"SendObjectState",                       ObjectFunctions::SendObjectState},\
    {"SendObjectMove",                        ObjectFunctions::SendObjectMove},\
    {"SendObjectRotate",                      ObjectFunctions::SendObjectRotate},\
    {"SendDoorState",                         ObjectFunctions::SendDoorState},\
    {"SendDoorDestination",                   ObjectFunctions::SendDoorDestination},\
    {"SendContainer",                         ObjectFunctions::SendContainer},\
    {"SendVideoPlay",                         ObjectFunctions::SendVideoPlay},\
    {"SendClientScriptLocal",                 ObjectFunctions::SendClientScriptLocal},\
    {"SendConsoleCommand",                    ObjectFunctions::SendConsoleCommand},\
    \
    {"ReadLastObjectList",                    ObjectFunctions::ReadLastObjectList},\
    {"ReadLastEvent",                         ObjectFunctions::ReadLastEvent},\
    {"InitializeObjectList",                  ObjectFunctions::InitializeObjectList},\
    {"InitializeEvent",                       ObjectFunctions::InitializeEvent},\
    {"CopyLastObjectListToStore",             ObjectFunctions::CopyLastObjectListToStore},\
    {"GetObjectChangesSize",                  ObjectFunctions::GetObjectChangesSize},\
    {"GetEventAction",                        ObjectFunctions::GetEventAction},\
    {"GetEventContainerSubAction",            ObjectFunctions::GetEventContainerSubAction},\
    {"GetObjectRefNumIndex",                  ObjectFunctions::GetObjectRefNumIndex},\
    {"GetObjectSummonerRefNumIndex",          ObjectFunctions::GetObjectSummonerRefNumIndex},\
    {"SetEventCell",                          ObjectFunctions::SetEventCell},\
    {"SetEventAction",                        ObjectFunctions::SetEventAction},\
    {"SetEventConsoleCommand",                ObjectFunctions::SetEventConsoleCommand},\
    {"SetObjectRefNumIndex",                  ObjectFunctions::SetObjectRefNumIndex},\
    {"SetObjectDisarmState",                  ObjectFunctions::SetObjectDisarmState},\
    {"AddWorldObject",                        ObjectFunctions::AddWorldObject}

class ObjectFunctions
{
public:

    /**
    * \brief Use the last object list received by the server as the one being read.
    *
    * \return void
    */
    static void ReadReceivedObjectList() noexcept;

    /**
    * \brief Clear the data from the object list stored on the server.
    *
    * \return void
    */
    static void ClearObjectList() noexcept;

    /**
    * \brief Set the pid attached to the ObjectList.
    *
    * \param pid The player ID to whom the object list should be attached.
    * \return void
    */
    static void SetObjectListPid(unsigned short pid) noexcept;

    /**
    * \brief Take the contents of the read-only object list last received by the
    *        server from a player and move its contents to the stored object list
    *        that can be sent by the server.
    *
    * \return void
    */
    static void CopyReceivedObjectListToStore() noexcept;

    /**
    * \brief Get the number of indexes in the read object list.
    *
    * \return The number of indexes.
    */
    static unsigned int GetObjectListSize() noexcept;

    /**
    * \brief Get the origin of the read object list.
    *
    * \return The origin (0 for CLIENT_GAMEPLAY, 1 for CLIENT_CONSOLE, 2 for
    * CLIENT_DIALOGUE, 3 for CLIENT_SCRIPT_LOCAL, 4 for CLIENT_SCRIPT_GLOBAL,
    * 5 for SERVER_SCRIPT).
    */
    static unsigned char GetObjectListOrigin() noexcept;

    /**
    * \brief Get the client script that the read object list originated from.
    *
    * \return The ID of the client script.
    */
    static const char *GetObjectListClientScript() noexcept;

    /**
    * \brief Get the action type used in the read object list.
    *
    * \return The action type (0 for SET, 1 for ADD, 2 for REMOVE, 3 for REQUEST).
    */
    static unsigned char GetObjectListAction() noexcept;

    /**
    * \brief Get the console command used in the read object list.
    *
    * \return The console command.
    */
    static const char *GetObjectListConsoleCommand() noexcept;

    /**
    * \brief Get the container subaction type used in the read object list.
    *
    * \return The action type (0 for NONE, 1 for DRAG, 2 for DROP, 3 for TAKE_ALL).
    */
    static unsigned char GetObjectListContainerSubAction() noexcept;

    /**
    * \brief Check whether the object at a certain index in the read object list is a
    * player.
    *
    * Note: Although most player data and events are dealt with in Player packets,
    *       object activation is general enough for players themselves to be included
    *       as objects in ObjectActivate packets.
    *
    * \param index The index of the object.
    * \return Whether the object is a player.
    */
    static bool IsObjectPlayer(unsigned int index) noexcept;

    /**
    * \brief Get the player ID of the object at a certain index in the read object list,
    * only valid if the object is a player.
    *
    * Note: Currently, players can only be objects in ObjectActivate and ConsoleCommand
    *       packets.
    *
    * \param index The index of the object.
    * \return The player ID of the object.
    */
    static int GetObjectPid(unsigned int index) noexcept;

    /**
    * \brief Get the refId of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The refId.
    */
    static const char *GetObjectRefId(unsigned int index) noexcept;

    /**
    * \brief Get the refNum of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The refNum.
    */
    static unsigned int GetObjectRefNum(unsigned int index) noexcept;

    /**
    * \brief Get the mpNum of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The mpNum.
    */
    static unsigned int GetObjectMpNum(unsigned int index) noexcept;

    /**
    * \brief Get the count of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The object count.
    */
    static int GetObjectCount(unsigned int index) noexcept;

    /**
    * \brief Get the charge of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The charge.
    */
    static int GetObjectCharge(unsigned int index) noexcept;

    /**
    * \brief Get the enchantment charge of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The enchantment charge.
    */
    static double GetObjectEnchantmentCharge(unsigned int index) noexcept;

    /**
    * \brief Get the soul of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The soul.
    */
    static const char *GetObjectSoul(unsigned int index) noexcept;

    /**
    * \brief Get the gold value of the object at a certain index in the read object list.
    *
    * This is used solely to get the gold value of gold. It is not used for other objects.
    *
    * \param index The index of the object.
    * \return The gold value.
    */
    static int GetObjectGoldValue(unsigned int index) noexcept;

    /**
    * \brief Get the object scale of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The object scale.
    */
    static double GetObjectScale(unsigned int index) noexcept;

    /**
    * \brief Get the object sound ID of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The object sound ID.
    */
    static const char *GetObjectSoundId(unsigned int index) noexcept;

    /**
    * \brief Get the object state of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The object state.
    */
    static bool GetObjectState(unsigned int index) noexcept;

    /**
    * \brief Get the door state of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The door state.
    */
    static int GetObjectDoorState(unsigned int index) noexcept;

    /**
    * \brief Get the lock level of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The lock level.
    */
    static int GetObjectLockLevel(unsigned int index) noexcept;

    /**
    * \brief Get the object trap spell ID of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The trap spell ID.
    */
    static const char* GetObjectTrapSpellId(unsigned int index) noexcept;

    /**
    * \brief Get the trap action for the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The trap action.
    */
    static unsigned int GetObjectTrapAction(unsigned int index) noexcept;

    /**
    * \brief Get the dialogue choice type for the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The dialogue choice type.
    */
    static unsigned int GetObjectDialogueChoiceType(unsigned int index) noexcept;

    /**
    * \brief Get the dialogue choice topic for the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The dialogue choice topic.
    */
    static const char *GetObjectDialogueChoiceTopic(unsigned int index) noexcept;

    /**
    * \brief Get the gold pool of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The gold pool.
    */
    static unsigned int GetObjectGoldPool(unsigned int index) noexcept;

    /**
    * \brief Get the hour of the last gold restock of the object at a certain index in the
    * read object list.
    *
    * \param index The index of the object.
    * \return The hour of the last gold restock.
    */
    static double GetObjectLastGoldRestockHour(unsigned int index) noexcept;

    /**
    * \brief Get the day of the last gold restock of the object at a certain index in the
    * read object list.
    *
    * \param index The index of the object.
    * \return The day of the last gold restock.
    */
    static int GetObjectLastGoldRestockDay(unsigned int index) noexcept;

    /**
    * \brief Check whether the object at a certain index in the read object list has been
    * activated by a player.
    *
    * \param index The index of the object.
    * \return Whether the object has been activated by a player.
    */
    static bool DoesObjectHavePlayerActivating(unsigned int index) noexcept;

    /**
    * \brief Get the player ID of the player activating the object at a certain index in the
    * read object list.
    *
    * \param index The index of the object.
    * \return The player ID of the activating player.
    */
    static int GetObjectActivatingPid(unsigned int index) noexcept;

    /**
    * \brief Get the refId of the actor activating the object at a certain index in the read
    * object list.
    *
    * \param index The index of the object.
    * \return The refId of the activating actor.
    */
    static const char *GetObjectActivatingRefId(unsigned int index) noexcept;

    /**
    * \brief Get the refNum of the actor activating the object at a certain index in the read
    * object list.
    *
    * \param index The index of the object.
    * \return The refNum of the activating actor.
    */
    static unsigned int GetObjectActivatingRefNum(unsigned int index) noexcept;

    /**
    * \brief Get the mpNum of the actor activating the object at a certain index in the read
    * object list.
    *
    * \param index The index of the object.
    * \return The mpNum of the activating actor.
    */
    static unsigned int GetObjectActivatingMpNum(unsigned int index) noexcept;

    /**
    * \brief Get the name of the actor activating the object at a certain index in the read
    * object list.
    *
    * \param index The index of the object.
    * \return The name of the activating actor.
    */
    static const char *GetObjectActivatingName(unsigned int index) noexcept;

    /**
    * \brief Check whether the object at a certain index in the read object list has been
    *        hit successfully.
    *
    * \param index The index of the object.
    * \return The success state.
    */
    static bool GetObjectHitSuccess(unsigned int index) noexcept;

    /**
    * \brief Get the damage caused to the object at a certain index in the read object list
    *        in a hit.
    *
    * \param index The index of the object.
    * \return The damage.
    */
    static double GetObjectHitDamage(unsigned int index) noexcept;

    /**
    * \brief Check whether the object at a certain index in the read object list has
    *        blocked the hit on it.
    *
    * \param index The index of the object.
    * \return The block state.
    */
    static bool GetObjectHitBlock(unsigned int index) noexcept;

    /**
    * \brief Check whether the object at a certain index in the read object list has been
    *        knocked down.
    *
    * \param index The index of the object.
    * \return The knockdown state.
    */
    static bool GetObjectHitKnockdown(unsigned int index) noexcept;

    /**
    * \brief Check whether the object at a certain index in the read object list has been
    * hit by a player.
    *
    * \param index The index of the object.
    * \return Whether the object has been hit by a player.
    */
    static bool DoesObjectHavePlayerHitting(unsigned int index) noexcept;

    /**
    * \brief Get the player ID of the player hitting the object at a certain index in the
    * read object list.
    *
    * \param index The index of the object.
    * \return The player ID of the hitting player.
    */
    static int GetObjectHittingPid(unsigned int index) noexcept;

    /**
    * \brief Get the refId of the actor hitting the object at a certain index in the read
    * object list.
    *
    * \param index The index of the object.
    * \return The refId of the hitting actor.
    */
    static const char *GetObjectHittingRefId(unsigned int index) noexcept;

    /**
    * \brief Get the refNum of the actor hitting the object at a certain index in the read
    * object list.
    *
    * \param index The index of the object.
    * \return The refNum of the hitting actor.
    */
    static unsigned int GetObjectHittingRefNum(unsigned int index) noexcept;

    /**
    * \brief Get the mpNum of the actor hitting the object at a certain index in the read
    * object list.
    *
    * \param index The index of the object.
    * \return The mpNum of the hitting actor.
    */
    static unsigned int GetObjectHittingMpNum(unsigned int index) noexcept;

    /**
    * \brief Get the name of the actor hitting the object at a certain index in the read
    * object list.
    *
    * \param index The index of the object.
    * \return The name of the hitting actor.
    */
    static const char *GetObjectHittingName(unsigned int index) noexcept;

    /**
    * \brief Check whether the object at a certain index in the read object list is a
    * summon.
    *
    * Only living actors can be summoned.
    *
    * \return The summon state.
    */
    static bool GetObjectSummonState(unsigned int index) noexcept;

    /**
    * \brief Get the summon effect ID of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The summon effect ID.
    */
    static double GetObjectSummonEffectId(unsigned int index) noexcept;

    /**
    * \brief Get the summon spell ID of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The summon spell ID.
    */
    static const char *GetObjectSummonSpellId(unsigned int index) noexcept;

    /**
    * \brief Get the summon duration of the object at a certain index in the read object list.
    *
    * Note: Returns -1 if indefinite.
    *
    * \param index The index of the object.
    * \return The summon duration.
    */
    static double GetObjectSummonDuration(unsigned int index) noexcept;

    /**
    * \brief Check whether the object at a certain index in the read object list has a player
    * as its summoner.
    *
    * Only living actors can be summoned.
    *
    * \param index The index of the object.
    * \return Whether a player is the summoner of the object.
    */
    static bool DoesObjectHavePlayerSummoner(unsigned int index) noexcept;

    /**
    * \brief Get the player ID of the summoner of the object at a certain index in the read object
    * list.
    *
    * \param index The index of the object.
    * \return The player ID of the summoner.
    */
    static int GetObjectSummonerPid(unsigned int index) noexcept;

    /**
    * \brief Get the refId of the actor summoner of the object at a certain index in the read object
    * list.
    *
    * \param index The index of the object.
    * \return The refId of the summoner.
    */
    static const char *GetObjectSummonerRefId(unsigned int index) noexcept;

    /**
    * \brief Get the refNum of the actor summoner of the object at a certain index in the read object
    * list.
    *
    * \param index The index of the object.
    * \return The refNum of the summoner.
    */
    static unsigned int GetObjectSummonerRefNum(unsigned int index) noexcept;

    /**
    * \brief Get the mpNum of the actor summoner of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The mpNum of the summoner.
    */
    static unsigned int GetObjectSummonerMpNum(unsigned int index) noexcept;

    /**
    * \brief Get the X position of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The X position.
    */
    static double GetObjectPosX(unsigned int index) noexcept;

    /**
    * \brief Get the Y position of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The Y position.
    */
    static double GetObjectPosY(unsigned int index) noexcept;

    /**
    * \brief Get the Z position at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The Z position.
    */
    static double GetObjectPosZ(unsigned int index) noexcept;

    /**
    * \brief Get the X rotation of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The X rotation.
    */
    static double GetObjectRotX(unsigned int index) noexcept;

    /**
    * \brief Get the Y rotation of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The Y rotation.
    */
    static double GetObjectRotY(unsigned int index) noexcept;

    /**
    * \brief Get the Z rotation of the object at a certain index in the read object list.
    *
    * \param index The index of the object.
    * \return The Z rotation.
    */
    static double GetObjectRotZ(unsigned int index) noexcept;

    /**
    * \brief Get the videoFilename of the object at a certain index in the read object list.
    *
    * \return The videoFilename.
    */
    static const char *GetVideoFilename(unsigned int index) noexcept;

    /**
    * \brief Get the number of client local variables of the object at a certain index in the
    * read object list.
    *
    * \param objectIndex The index of the object.
    * \return The number of client local variables.
    */
    static unsigned int GetClientLocalsSize(unsigned int objectIndex) noexcept;

    /**
    * \brief Get the internal script index of the client local variable at a certain variableIndex in
    * the client locals of the object at a certain objectIndex in the read object list.
    *
    * \param objectIndex The index of the object.
    * \param variableIndex The index of the client local.
    * \return The internal script index.
    */
    static unsigned int GetClientLocalInternalIndex(unsigned int objectIndex, unsigned int variableIndex) noexcept;

    /**
    * \brief Get the type of the client local variable at a certain variableIndex in the client locals
    * of the object at a certain objectIndex in the read object list.
    *
    * \param objectIndex The index of the object.
    * \param variableIndex The index of the client local.
    * \return The variable type (0 for INTEGER, 1 for LONG, 2 for FLOAT).
    */
    static unsigned short GetClientLocalVariableType(unsigned int objectIndex, unsigned int variableIndex) noexcept;

    /**
    * \brief Get the integer value of the client local variable at a certain variableIndex in the client
    * locals of the object at a certain objectIndex in the read object list.
    *
    * \param objectIndex The index of the object.
    * \param variableIndex The index of the client local.
    * \return The integer value.
    */
    static int GetClientLocalIntValue(unsigned int objectIndex, unsigned int variableIndex) noexcept;

    /**
    * \brief Get the float value of the client local variable at a certain variableIndex in the client
    * locals of the object at a certain objectIndex in the read object list.
    *
    * \param objectIndex The index of the object.
    * \param variableIndex The index of the client local.
    * \return The float value.
    */
    static double GetClientLocalFloatValue(unsigned int objectIndex, unsigned int variableIndex) noexcept;

    /**
    * \brief Get the number of container item indexes of the object at a certain index in the
    * read object list.
    *
    * \param objectIndex The index of the object.
    * \return The number of container item indexes.
    */
    static unsigned int GetContainerChangesSize(unsigned int objectIndex) noexcept;

    /**
    * \brief Get the refId of the container item at a certain itemIndex in the container changes
    * of the object at a certain objectIndex in the read object list.
    *
    * \param objectIndex The index of the object.
    * \param itemIndex The index of the container item.
    * \return The refId.
    */
    static const char *GetContainerItemRefId(unsigned int objectIndex, unsigned int itemIndex) noexcept;

    /**
    * \brief Get the item count of the container item at a certain itemIndex in the container
    * changes of the object at a certain objectIndex in the read object list.
    *
    * \param objectIndex The index of the object.
    * \param itemIndex The index of the container item.
    * \return The item count.
    */
    static int GetContainerItemCount(unsigned int objectIndex, unsigned int itemIndex) noexcept;

    /**
    * \brief Get the charge of the container item at a certain itemIndex in the container changes
    * of the object at a certain objectIndex in the read object list.
    *
    * \param objectIndex The index of the object.
    * \param itemIndex The index of the container item.
    * \return The charge.
    */
    static int GetContainerItemCharge(unsigned int objectIndex, unsigned int itemIndex) noexcept;

    /**
    * \brief Get the enchantment charge of the container item at a certain itemIndex in the container changes
    * of the object at a certain objectIndex in the read object list.
    *
    * \param objectIndex The index of the object.
    * \param itemIndex The index of the container item.
    * \return The enchantment charge.
    */
    static double GetContainerItemEnchantmentCharge(unsigned int objectIndex, unsigned int itemIndex) noexcept;

    /**
    * \brief Get the soul of the container item at a certain itemIndex in the container changes
    * of the object at a certain objectIndex in the read object list.
    *
    * \param objectIndex The index of the object.
    * \param itemIndex The index of the container item.
    * \return The soul.
    */
    static const char *GetContainerItemSoul(unsigned int objectIndex, unsigned int itemIndex) noexcept;

    /**
    * \brief Get the action count of the container item at a certain itemIndex in the container
    * changes of the object at a certain objectIndex in the read object list.
    *
    * \param objectIndex The index of the object.
    * \param itemIndex The index of the container item.
    * \return The action count.
    */
    static int GetContainerItemActionCount(unsigned int objectIndex, unsigned int itemIndex) noexcept;

    /**
    * \brief Check whether the object at a certain index in the read object list has a container.
    * 
    * Note: Only ObjectLists from ObjectPlace packets contain this information. Objects from
    *       received ObjectSpawn packets can always be assumed to have a container.
    *
    * \param index The index of the object.
    * \return Whether the object has a container.
    */
    static bool DoesObjectHaveContainer(unsigned int index) noexcept;

    /**
    * \brief Check whether the object at a certain index in the read object list has been
    *        dropped by a player.
    *
    * Note: Only ObjectLists from ObjectPlace packets contain this information.
    *
    * \param index The index of the object.
    * \return Whether the object has been dropped by a player.
    */
    static bool IsObjectDroppedByPlayer(unsigned int index) noexcept;

    /**
    * \brief Set the cell of the temporary object list stored on the server.
    *
    * The cell is determined to be an exterior cell if it fits the pattern of a number followed
    * by a comma followed by another number.
    *
    * \param cellDescription The description of the cell.
    * \return void
    */
    static void SetObjectListCell(const char* cellDescription) noexcept;

    /**
    * \brief Set the action type of the temporary object list stored on the server.
    *
    * \param action The action type (0 for SET, 1 for ADD, 2 for REMOVE, 3 for REQUEST).
    * \return void
    */
    static void SetObjectListAction(unsigned char action) noexcept;

    /**
    * \brief Set the container subaction type of the temporary object list stored on the server.
    *
    * \param subAction The action type (0 for NONE, 1 for DRAG, 2 for DROP, 3 for TAKE_ALL,
    *                  4 for REPLY_TO_REQUEST, 5 for RESTOCK_RESULT).
    * \return void
    */
    static void SetObjectListContainerSubAction(unsigned char subAction) noexcept;

    /**
    * \brief Set the console command of the temporary object list stored on the server.
    *
    * When sent, the command will run once on every object added to the object list. If no objects
    * have been added, it will run once without any object reference.
    *
    * \param consoleCommand The console command.
    * \return void
    */
    static void SetObjectListConsoleCommand(const char* consoleCommand) noexcept;

    /**
    * \brief Set the refId of the temporary object stored on the server.
    *
    * \param refId The refId.
    * \return void
    */
    static void SetObjectRefId(const char* refId) noexcept;

    /**
    * \brief Set the refNum of the temporary object stored on the server.
    *
    * Every object loaded from .ESM and .ESP data files has a unique refNum which needs to be
    * retained to refer to it in packets.
    * 
    * On the other hand, objects placed or spawned via the server should always have a refNum
    * of 0.
    *
    * \param refNum The refNum.
    * \return void
    */
    static void SetObjectRefNum(int refNum) noexcept;

    /**
    * \brief Set the mpNum of the temporary object stored on the server.
    *
    * Every object placed or spawned via the server is assigned an mpNum by incrementing the last
    * mpNum stored on the server. Scripts should take care to ensure that mpNums are kept unique
    * for these objects.
    * 
    * Objects loaded from .ESM and .ESP data files should always have an mpNum of 0, because they
    * have unique refNumes instead.
    *
    * \param mpNum The mpNum.
    * \return void
    */
    static void SetObjectMpNum(int mpNum) noexcept;

    /**
    * \brief Set the object count of the temporary object stored on the server.
    *
    * This determines the quantity of an object, with the exception of gold.
    *
    * \param count The object count.
    * \return void
    */
    static void SetObjectCount(int count) noexcept;

    /**
    * \brief Set the charge of the temporary object stored on the server.
    *
    * Object durabilities are set through this value.
    *
    * \param charge The charge.
    * \return void
    */
    static void SetObjectCharge(int charge) noexcept;

    /**
    * \brief Set the enchantment charge of the temporary object stored on the server.
    *
    * Object durabilities are set through this value.
    *
    * \param enchantmentCharge The enchantment charge.
    * \return void
    */
    static void SetObjectEnchantmentCharge(double enchantmentCharge) noexcept;

    /**
    * \brief Set the soul of the temporary object stored on the server.
    *
    * \param soul The ID of the soul.
    * \return void
    */
    static void SetObjectSoul(const char* soul) noexcept;

    /**
    * \brief Set the gold value of the temporary object stored on the server.
    *
    * This is used solely to set the gold value for gold. It has no effect on other objects.
    *
    * \param goldValue The gold value.
    * \return void
    */
    static void SetObjectGoldValue(int goldValue) noexcept;

    /**
    * \brief Set the scale of the temporary object stored on the server.
    *
    * Objects are smaller or larger than their default size based on their scale.
    *
    * \param scale The scale.
    * \return void
    */
    static void SetObjectScale(double scale) noexcept;

    /**
    * \brief Set the object state of the temporary object stored on the server.
    *
    * Objects are enabled or disabled based on their object state.
    *
    * \param objectState The object state.
    * \return void
    */
    static void SetObjectState(bool objectState) noexcept;

    /**
    * \brief Set the lock level of the temporary object stored on the server.
    *
    * \param lockLevel The lock level.
    * \return void
    */
    static void SetObjectLockLevel(int lockLevel) noexcept;

    /**
    * \brief Set the trap spell ID of the temporary object stored on the server.
    *
    * \param trapSpellId The trap spell ID.
    * \return void
    */
    static void SetObjectTrapSpellId(const char* trapSpellId) noexcept;

    /**
    * \brief Set the trap disarm state of the temporary object stored on the server.
    *
    * \param disarmState The disarmState.
    * \return void
    */
    static void SetObjectTrapAction(unsigned int trapAction) noexcept;

    /**
    * \brief Set the dialogue choice type of the temporary object stored on the server.
    *
    * \param dialogueChoiceType The dialogue choice type.
    * \return void
    */
    static void SetObjectDialogueChoiceType(unsigned int dialogueChoiceType) noexcept;

    /**
    * \brief Set the dialogue choice topic for the temporary object stored on the server.
    *
    * \param topic The dialogue choice topic.
    * \return void
    */
    static void SetObjectDialogueChoiceTopic(const char* topic) noexcept;

    /**
    * \brief Set the gold pool of the temporary object stored on the server.
    *
    * \param goldPool The gold pool.
    * \return void
    */
    static void SetObjectGoldPool(unsigned int goldPool) noexcept;

    /**
    * \brief Set the hour of the last gold restock of the temporary object stored on the server.
    *
    * \param hour The hour of the last gold restock.
    * \return void
    */
    static void SetObjectLastGoldRestockHour(double hour) noexcept;

    /**
    * \brief Set the day of the last gold restock of the temporary object stored on the server.
    *
    * \param day The day of the last gold restock.
    * \return void
    */
    static void SetObjectLastGoldRestockDay(int day) noexcept;

    /**
    * \brief Set the droppedByPlayer state of the temporary object stored on the server.
    *
    * \param dropedByPlayerState Whether the object has been dropped by a player or not.
    * \return void
    */
    static void SetObjectDroppedByPlayerState(bool dropedByPlayerState) noexcept;

    /**
    * \brief Set the position of the temporary object stored on the server.
    *
    * \param x The X position.
    * \param y The Y position.
    * \param z The Z position.
    * \return void
    */
    static void SetObjectPosition(double x, double y, double z) noexcept;

    /**
    * \brief Set the rotation of the temporary object stored on the server.
    *
    * \param x The X rotation.
    * \param y The Y rotation.
    * \param z The Z rotation.
    * \return void
    */
    static void SetObjectRotation(double x, double y, double z) noexcept;

    static void SetObjectSound(const char* soundId, double volume, double pitch) noexcept;

    /**
    * \brief Set the summon state of the temporary object stored on the server.
    *
    * This only affects living actors and determines whether they are summons of another
    * living actor.
    *
    * \param summonState The summon state.
    * \return void
    */
    static void SetObjectSummonState(bool summonState) noexcept;

    /**
    * \brief Set the summon effect ID of the temporary object stored on the server.
    *
    * \param summonEffectId The summon effect ID.
    * \return void
    */
    static void SetObjectSummonEffectId(int summonEffectId) noexcept;

    /**
    * \brief Set the summon spell ID of the temporary object stored on the server.
    *
    * \param summonSpellId The summon spell ID.
    * \return void
    */
    static void SetObjectSummonSpellId(const char* summonSpellId) noexcept;

    /**
    * \brief Set the summon duration of the temporary object stored on the server.
    *
    * \param summonDuration The summon duration.
    * \return void
    */
    static void SetObjectSummonDuration(double summonDuration) noexcept;

    /**
    * \brief Set the player ID of the summoner of the temporary object stored on the server.
    *
    * \param pid The player ID of the summoner.
    * \return void
    */
    static void SetObjectSummonerPid(unsigned short pid) noexcept;

    /**
    * \brief Set the refNum of the actor summoner of the temporary object stored on the server.
    *
    * \param refNum The refNum of the summoner.
    * \return void
    */
    static void SetObjectSummonerRefNum(int refNum) noexcept;

    /**
    * \brief Set the mpNum of the actor summoner of the temporary object stored on the server.
    *
    * \param mpNum The mpNum of the summoner.
    * \return void
    */
    static void SetObjectSummonerMpNum(int mpNum) noexcept;

    /**
    * \brief Set the player ID of the player activating the temporary object stored on the
    *        server. Currently only used for ObjectActivate packets.
    *
    * \param pid The pid of the player.
    * \return void
    */
    static void SetObjectActivatingPid(unsigned short pid) noexcept;

    /**
    * \brief Set the door state of the temporary object stored on the server.
    *
    * Doors are open or closed based on their door state.
    *
    * \param doorState The door state.
    * \return void
    */
    static void SetObjectDoorState(int doorState) noexcept;

    /**
    * \brief Set the teleport state of the temporary object stored on the server.
    *
    * If a door's teleport state is true, interacting with the door teleports a player to its
    * destination. If it's false, it opens and closes like a regular door.
    *
    * \param teleportState The teleport state.
    * \return void
    */
    static void SetObjectDoorTeleportState(bool teleportState) noexcept;

    /**
    * \brief Set the door destination cell of the temporary object stored on the server.
    *
    * The cell is determined to be an exterior cell if it fits the pattern of a number followed
    * by a comma followed by another number.
    *
    * \param cellDescription The description of the cell.
    * \return void
    */
    static void SetObjectDoorDestinationCell(const char* cellDescription) noexcept;

    /**
    * \brief Set the door destination position of the temporary object stored on the server.
    *
    * \param x The X position.
    * \param y The Y position.
    * \param z The Z position.
    * \return void
    */
    static void SetObjectDoorDestinationPosition(double x, double y, double z) noexcept;

    /**
    * \brief Set the door destination rotation of the temporary object stored on the server.
    *
    * Note: Because this sets the rotation a player will have upon using the door, and rotation
    *       on the Y axis has no effect on players, the Y value has been omitted as an argument.
    *
    * \param x The X rotation.
    * \param z The Z rotation.
    * \return void
    */
    static void SetObjectDoorDestinationRotation(double x, double z) noexcept;

    /**
    * \brief Set a player as the object in the temporary object stored on the server.
    *        Currently only used for ConsoleCommand packets.
    *
    * \param pid The pid of the player.
    * \return void
    */
    static void SetPlayerAsObject(unsigned short pid) noexcept;

    /**
    * \brief Set the refId of the temporary container item stored on the server.
    *
    * \param refId The refId.
    * \return void
    */
    static void SetContainerItemRefId(const char* refId) noexcept;

    /**
    * \brief Set the item count of the temporary container item stored on the server.
    *
    * \param count The item count.
    * \return void
    */
    static void SetContainerItemCount(int count) noexcept;

    /**
    * \brief Set the charge of the temporary container item stored on the server.
    *
    * \param charge The charge.
    * \return void
    */
    static void SetContainerItemCharge(int charge) noexcept;

    /**
    * \brief Set the enchantment charge of the temporary container item stored on the server.
    *
    * \param enchantmentCharge The enchantment charge.
    * \return void
    */
    static void SetContainerItemEnchantmentCharge(double enchantmentCharge) noexcept;

    /**
    * \brief Set the soul of the temporary container item stored on the server.
    *
    * \param soul The soul.
    * \return void
    */
    static void SetContainerItemSoul(const char* soul) noexcept;

    /**
    * \brief Set the action count of the container item at a certain itemIndex in the container
    * changes of the object at a certain objectIndex in the object list stored on the server.
    *
    * When resending a received Container packet, this allows you to correct the amount of items
    * removed from a container by a player when it conflicts with what other players have already
    * taken.
    *
    * \param objectIndex The index of the object.
    * \param itemIndex The index of the container item.
    * \param actionCount The action count.
    * \return void
    */
    static void SetContainerItemActionCountByIndex(unsigned int objectIndex, unsigned int itemIndex, int actionCount) noexcept;

    /**
    * \brief Add a copy of the server's temporary object to the server's currently stored object
    * list.
    *
    * In the process, the server's temporary object will automatically be cleared so a new
    * one can be set up.
    *
    * \return void
    */
    static void AddObject() noexcept;

    /**
    * \brief Add a client local variable with an integer value to the client locals of the server's
    * temporary object.
    *
    * \param internalIndex The internal script index of the client local.
    * \param variableType The variable type (0 for SHORT, 1 for LONG).
    * \param intValue The integer value of the client local.
    * \return void
    */
    static void AddClientLocalInteger(int internalIndex, int intValue, unsigned int variableType) noexcept;

    /**
    * \brief Add a client local variable with a float value to the client locals of the server's
    * temporary object.
    *
    * \param internalIndex The internal script index of the client local.
    * \param floatValue The float value of the client local.
    * \return void
    */
    static void AddClientLocalFloat(int internalIndex, double floatValue) noexcept;

    /**
    * \brief Add a copy of the server's temporary container item to the container changes of the
    * server's temporary object.
    *
    * In the process, the server's temporary container item will automatically be cleared so a new
    * one can be set up.
    *
    * \return void
    */
    static void AddContainerItem() noexcept;

    /**
    * \brief Send an ObjectActivate packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectActivate(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectPlace packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectPlace(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectSpawn packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectSpawn(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectDelete packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectDelete(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectLock packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectLock(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectDialogueChoice packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectDialogueChoice(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectMiscellaneous packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectMiscellaneous(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectRestock packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectRestock(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectTrap packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectTrap(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectScale packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectScale(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectSound packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectSound(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectState packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectState(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectMove packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectMove(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send an ObjectRotate packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendObjectRotate(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send a DoorState packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendDoorState(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send a DoorDestination packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendDoorDestination(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send a Container packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendContainer(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send a VideoPlay packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendVideoPlay(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send a ClientScriptLocal packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendClientScriptLocal(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;

    /**
    * \brief Send a ConsoleCommand packet.
    *
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param skipAttachedPlayer Whether the packet should skip being sent to the player attached
    *                           to the packet (false by default).
    * \return void
    */
    static void SendConsoleCommand(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;


    // All methods below are deprecated versions of methods from above

    static void ReadLastObjectList() noexcept;
    static void ReadLastEvent() noexcept;
    static void InitializeObjectList(unsigned short pid) noexcept;
    static void InitializeEvent(unsigned short pid) noexcept;
    static void CopyLastObjectListToStore() noexcept;
    static unsigned int GetObjectChangesSize() noexcept;
    static unsigned char GetEventAction() noexcept;
    static unsigned char GetEventContainerSubAction() noexcept;
    static unsigned int GetObjectRefNumIndex(unsigned int index) noexcept;
    static unsigned int GetObjectSummonerRefNumIndex(unsigned int index) noexcept;
    static void SetEventCell(const char* cellDescription) noexcept;
    static void SetEventAction(unsigned char action) noexcept;
    static void SetEventConsoleCommand(const char* consoleCommand) noexcept;
    static void SetObjectRefNumIndex(int refNum) noexcept;
    static void SetObjectDisarmState(bool disarmState) noexcept;
    static void AddWorldObject() noexcept;

};


#endif //OPENMW_OBJECTAPI_HPP
