#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/Base/BaseObject.hpp>

#include <apps/openmw-mp/Networking.hpp>
#include <apps/openmw-mp/Player.hpp>
#include <apps/openmw-mp/Utils.hpp>
#include <apps/openmw-mp/Script/ScriptFunctions.hpp>

#include "Objects.hpp"

using namespace mwmp;

BaseObjectList *readObjectList;
BaseObjectList writeObjectList;

BaseObject tempObject;
const BaseObject emptyObject = {};

ContainerItem tempContainerItem;
const ContainerItem emptyContainerItem = {};

void ObjectFunctions::ReadReceivedObjectList() noexcept
{
    readObjectList = mwmp::Networking::getPtr()->getReceivedObjectList();
}

void ObjectFunctions::ClearObjectList() noexcept
{
    writeObjectList.cell.blank();
    writeObjectList.baseObjects.clear();
    writeObjectList.packetOrigin = mwmp::PACKET_ORIGIN::SERVER_SCRIPT;
}

void ObjectFunctions::SetObjectListPid(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    writeObjectList.guid = player->guid;
}

void ObjectFunctions::CopyReceivedObjectListToStore() noexcept
{
    writeObjectList = *readObjectList;
}

unsigned int ObjectFunctions::GetObjectListSize() noexcept
{
    return readObjectList->baseObjectCount;
}

unsigned char ObjectFunctions::GetObjectListOrigin() noexcept
{
    return readObjectList->packetOrigin;
}

const char *ObjectFunctions::GetObjectListClientScript() noexcept
{
    return readObjectList->originClientScript.c_str();
}

unsigned char ObjectFunctions::GetObjectListAction() noexcept
{
    return readObjectList->action;
}

const char *ObjectFunctions::GetObjectListConsoleCommand() noexcept
{
    return readObjectList->consoleCommand.c_str();
}

unsigned char ObjectFunctions::GetObjectListContainerSubAction() noexcept
{
    return readObjectList->containerSubAction;
}

bool ObjectFunctions::IsObjectPlayer(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).isPlayer;
}

int ObjectFunctions::GetObjectPid(unsigned int index) noexcept
{
    Player *player = Players::getPlayer(readObjectList->baseObjects.at(index).guid);

    if (player != nullptr)
        return player->getId();

    return -1;
}

const char *ObjectFunctions::GetObjectRefId(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).refId.c_str();
}

unsigned int ObjectFunctions::GetObjectRefNum(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).refNum;
}

unsigned int ObjectFunctions::GetObjectMpNum(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).mpNum;
}

int ObjectFunctions::GetObjectCount(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).count;
}

int ObjectFunctions::GetObjectCharge(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).charge;
}

double ObjectFunctions::GetObjectEnchantmentCharge(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).enchantmentCharge;
}

const char *ObjectFunctions::GetObjectSoul(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).soul.c_str();
}

int ObjectFunctions::GetObjectGoldValue(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).goldValue;
}

double ObjectFunctions::GetObjectScale(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).scale;
}

const char *ObjectFunctions::GetObjectSoundId(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).soundId.c_str();
}

bool ObjectFunctions::GetObjectState(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).objectState;
}

int ObjectFunctions::GetObjectDoorState(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).doorState;
}

int ObjectFunctions::GetObjectLockLevel(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).lockLevel;
}

const char* ObjectFunctions::GetObjectTrapSpellId(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).trapSpellId.c_str();
}

unsigned int ObjectFunctions::GetObjectTrapAction(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).trapAction;
}

unsigned int ObjectFunctions::GetObjectDialogueChoiceType(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).dialogueChoiceType;
}

const char* ObjectFunctions::GetObjectDialogueChoiceTopic(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).topicId.c_str();
}

unsigned int ObjectFunctions::GetObjectGoldPool(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).goldPool;
}

double ObjectFunctions::GetObjectLastGoldRestockHour(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).lastGoldRestockHour;
}

int ObjectFunctions::GetObjectLastGoldRestockDay(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).lastGoldRestockDay;
}

bool ObjectFunctions::DoesObjectHavePlayerActivating(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).activatingActor.isPlayer;
}

int ObjectFunctions::GetObjectActivatingPid(unsigned int index) noexcept
{
    Player *player = Players::getPlayer(readObjectList->baseObjects.at(index).activatingActor.guid);

    if (player != nullptr)
        return player->getId();

    return -1;
}

const char *ObjectFunctions::GetObjectActivatingRefId(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).activatingActor.refId.c_str();
}

unsigned int ObjectFunctions::GetObjectActivatingRefNum(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).activatingActor.refNum;
}

unsigned int ObjectFunctions::GetObjectActivatingMpNum(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).activatingActor.mpNum;
}

const char *ObjectFunctions::GetObjectActivatingName(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).activatingActor.name.c_str();
}

bool ObjectFunctions::GetObjectHitSuccess(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).hitAttack.success;
}

double ObjectFunctions::GetObjectHitDamage(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).hitAttack.damage;
}

bool ObjectFunctions::GetObjectHitBlock(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).hitAttack.block;
}

bool ObjectFunctions::GetObjectHitKnockdown(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).hitAttack.knockdown;
}

bool ObjectFunctions::DoesObjectHavePlayerHitting(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).hittingActor.isPlayer;
}

int ObjectFunctions::GetObjectHittingPid(unsigned int index) noexcept
{
    Player *player = Players::getPlayer(readObjectList->baseObjects.at(index).hittingActor.guid);

    if (player != nullptr)
        return player->getId();

    return -1;
}

const char *ObjectFunctions::GetObjectHittingRefId(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).hittingActor.refId.c_str();
}

unsigned int ObjectFunctions::GetObjectHittingRefNum(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).hittingActor.refNum;
}

unsigned int ObjectFunctions::GetObjectHittingMpNum(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).hittingActor.mpNum;
}

const char *ObjectFunctions::GetObjectHittingName(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).hittingActor.name.c_str();
}

bool ObjectFunctions::GetObjectSummonState(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).isSummon;
}

double ObjectFunctions::GetObjectSummonDuration(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).summonDuration;
}

double ObjectFunctions::GetObjectSummonEffectId(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).summonEffectId;
}

const char *ObjectFunctions::GetObjectSummonSpellId(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).summonSpellId.c_str();
}

bool ObjectFunctions::DoesObjectHavePlayerSummoner(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).master.isPlayer;
}

int ObjectFunctions::GetObjectSummonerPid(unsigned int index) noexcept
{
    Player *player = Players::getPlayer(readObjectList->baseObjects.at(index).master.guid);
    
    if (player != nullptr)
        return player->getId();

    return -1;
}

const char *ObjectFunctions::GetObjectSummonerRefId(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).master.refId.c_str();
}

unsigned int ObjectFunctions::GetObjectSummonerRefNum(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).master.refNum;
}

unsigned int ObjectFunctions::GetObjectSummonerMpNum(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).master.mpNum;
}

double ObjectFunctions::GetObjectPosX(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).position.pos[0];
}

double ObjectFunctions::GetObjectPosY(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).position.pos[1];
}

double ObjectFunctions::GetObjectPosZ(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).position.pos[2];
}

double ObjectFunctions::GetObjectRotX(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).position.rot[0];
}

double ObjectFunctions::GetObjectRotY(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).position.rot[1];
}

double ObjectFunctions::GetObjectRotZ(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).position.rot[2];
}

const char *ObjectFunctions::GetVideoFilename(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).videoFilename.c_str();
}

unsigned int ObjectFunctions::GetClientLocalsSize(unsigned int objectIndex) noexcept
{
    return readObjectList->baseObjects.at(objectIndex).clientLocals.size();
}

unsigned int ObjectFunctions::GetClientLocalInternalIndex(unsigned int objectIndex, unsigned int variableIndex) noexcept
{
    return readObjectList->baseObjects.at(objectIndex).clientLocals.at(variableIndex).internalIndex;
}

unsigned short ObjectFunctions::GetClientLocalVariableType(unsigned int objectIndex, unsigned int variableIndex) noexcept
{
    return readObjectList->baseObjects.at(objectIndex).clientLocals.at(variableIndex).variableType;
}

int ObjectFunctions::GetClientLocalIntValue(unsigned int objectIndex, unsigned int variableIndex) noexcept
{
    return readObjectList->baseObjects.at(objectIndex).clientLocals.at(variableIndex).intValue;
}

double ObjectFunctions::GetClientLocalFloatValue(unsigned int objectIndex, unsigned int variableIndex) noexcept
{
    return readObjectList->baseObjects.at(objectIndex).clientLocals.at(variableIndex).floatValue;
}

unsigned int ObjectFunctions::GetContainerChangesSize(unsigned int objectIndex) noexcept
{
    return readObjectList->baseObjects.at(objectIndex).containerItemCount;
}

const char *ObjectFunctions::GetContainerItemRefId(unsigned int objectIndex, unsigned int itemIndex) noexcept
{
    return readObjectList->baseObjects.at(objectIndex)
        .containerItems.at(itemIndex).refId.c_str();
}

int ObjectFunctions::GetContainerItemCount(unsigned int objectIndex, unsigned int itemIndex) noexcept
{
    return readObjectList->baseObjects.at(objectIndex)
        .containerItems.at(itemIndex).count;
}

int ObjectFunctions::GetContainerItemCharge(unsigned int objectIndex, unsigned int itemIndex) noexcept
{
    return readObjectList->baseObjects.at(objectIndex)
        .containerItems.at(itemIndex).charge;
}

double ObjectFunctions::GetContainerItemEnchantmentCharge(unsigned int objectIndex, unsigned int itemIndex) noexcept
{
    return readObjectList->baseObjects.at(objectIndex)
        .containerItems.at(itemIndex).enchantmentCharge;
}

const char *ObjectFunctions::GetContainerItemSoul(unsigned int objectIndex, unsigned int itemIndex) noexcept
{
    return readObjectList->baseObjects.at(objectIndex)
        .containerItems.at(itemIndex).soul.c_str();
}

int ObjectFunctions::GetContainerItemActionCount(unsigned int objectIndex, unsigned int itemIndex) noexcept
{
    return readObjectList->baseObjects.at(objectIndex)
        .containerItems.at(itemIndex).actionCount;
}

bool ObjectFunctions::DoesObjectHaveContainer(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).hasContainer;
}

bool ObjectFunctions::IsObjectDroppedByPlayer(unsigned int index) noexcept
{
    return readObjectList->baseObjects.at(index).droppedByPlayer;
}

void ObjectFunctions::SetObjectListCell(const char* cellDescription) noexcept
{
    writeObjectList.cell = Utils::getCellFromDescription(cellDescription);
}

void ObjectFunctions::SetObjectListAction(unsigned char action) noexcept
{
    writeObjectList.action = action;
}

void ObjectFunctions::SetObjectListContainerSubAction(unsigned char containerSubAction) noexcept
{
    writeObjectList.containerSubAction = containerSubAction;
}

void ObjectFunctions::SetObjectListConsoleCommand(const char* consoleCommand) noexcept
{
    writeObjectList.consoleCommand = consoleCommand;
}

void ObjectFunctions::SetObjectRefId(const char* refId) noexcept
{
    tempObject.refId = refId;
}

void ObjectFunctions::SetObjectRefNum(int refNum) noexcept
{
    tempObject.refNum = refNum;
}

void ObjectFunctions::SetObjectMpNum(int mpNum) noexcept
{
    tempObject.mpNum = mpNum;
}

void ObjectFunctions::SetObjectCount(int count) noexcept
{
    tempObject.count = count;
}

void ObjectFunctions::SetObjectCharge(int charge) noexcept
{
    tempObject.charge = charge;
}

void ObjectFunctions::SetObjectEnchantmentCharge(double enchantmentCharge) noexcept
{
    tempObject.enchantmentCharge = enchantmentCharge;
}

void ObjectFunctions::SetObjectSoul(const char* soul) noexcept
{
    tempObject.soul = soul;
}

void ObjectFunctions::SetObjectGoldValue(int goldValue) noexcept
{
    tempObject.goldValue = goldValue;
}

void ObjectFunctions::SetObjectScale(double scale) noexcept
{
    tempObject.scale = scale;
}

void ObjectFunctions::SetObjectState(bool objectState) noexcept
{
    tempObject.objectState = objectState;
}

void ObjectFunctions::SetObjectLockLevel(int lockLevel) noexcept
{
    tempObject.lockLevel = lockLevel;
}

void ObjectFunctions::SetObjectTrapSpellId(const char* trapSpellId) noexcept
{
    tempObject.trapSpellId = trapSpellId;
}

void ObjectFunctions::SetObjectTrapAction(unsigned int trapAction) noexcept
{
    tempObject.trapAction = trapAction;
}

void ObjectFunctions::SetObjectDialogueChoiceType(unsigned int dialogueChoiceType) noexcept
{
    tempObject.dialogueChoiceType = dialogueChoiceType;
}

void ObjectFunctions::SetObjectDialogueChoiceTopic(const char* topic) noexcept
{
    tempObject.topicId = topic;
}

void ObjectFunctions::SetObjectGoldPool(unsigned int goldPool) noexcept
{
    tempObject.goldPool = goldPool;
}

void ObjectFunctions::SetObjectLastGoldRestockHour(double lastGoldRestockHour) noexcept
{
    tempObject.lastGoldRestockHour = lastGoldRestockHour;
}

void ObjectFunctions::SetObjectLastGoldRestockDay(int lastGoldRestockDay) noexcept
{
    tempObject.lastGoldRestockDay = lastGoldRestockDay;
}

void ObjectFunctions::SetObjectDroppedByPlayerState(bool droppedByPlayer) noexcept
{
    tempObject.droppedByPlayer = droppedByPlayer;
}

void ObjectFunctions::SetObjectPosition(double x, double y, double z) noexcept
{
    tempObject.position.pos[0] = x;
    tempObject.position.pos[1] = y;
    tempObject.position.pos[2] = z;
}

void ObjectFunctions::SetObjectRotation(double x, double y, double z) noexcept
{
    tempObject.position.rot[0] = x;
    tempObject.position.rot[1] = y;
    tempObject.position.rot[2] = z;
}

void ObjectFunctions::SetObjectSound(const char* soundId, double volume, double pitch) noexcept
{
    tempObject.soundId = soundId;
    tempObject.volume = volume;
    tempObject.pitch = pitch;
}

void ObjectFunctions::SetObjectSummonState(bool summonState) noexcept
{
    tempObject.isSummon = summonState;
}

void ObjectFunctions::SetObjectSummonEffectId(int summonEffectId) noexcept
{
    tempObject.summonEffectId = summonEffectId;
}

void ObjectFunctions::SetObjectSummonSpellId(const char* summonSpellId) noexcept
{
    tempObject.summonSpellId = summonSpellId;
}

void ObjectFunctions::SetObjectSummonDuration(double summonDuration) noexcept
{
    tempObject.summonDuration = summonDuration;
}

void ObjectFunctions::SetObjectSummonerPid(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    tempObject.master.isPlayer = true;
    tempObject.master.guid = player->guid;
}

void ObjectFunctions::SetObjectSummonerRefNum(int refNum) noexcept
{
    tempObject.master.isPlayer = false;
    tempObject.master.refNum = refNum;
}

void ObjectFunctions::SetObjectSummonerMpNum(int mpNum) noexcept
{
    tempObject.master.isPlayer = false;
    tempObject.master.mpNum = mpNum;
}

void ObjectFunctions::SetObjectActivatingPid(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    tempObject.activatingActor.isPlayer = true;
    tempObject.activatingActor.guid = player->guid;
}

void ObjectFunctions::SetObjectDoorState(int doorState) noexcept
{
    tempObject.doorState = doorState;
}

void ObjectFunctions::SetObjectDoorTeleportState(bool teleportState) noexcept
{
    tempObject.teleportState = teleportState;
}

void ObjectFunctions::SetObjectDoorDestinationCell(const char* cellDescription) noexcept
{
    tempObject.destinationCell = Utils::getCellFromDescription(cellDescription);
}

void ObjectFunctions::SetObjectDoorDestinationPosition(double x, double y, double z) noexcept
{
    tempObject.destinationPosition.pos[0] = x;
    tempObject.destinationPosition.pos[1] = y;
    tempObject.destinationPosition.pos[2] = z;
}

void ObjectFunctions::SetObjectDoorDestinationRotation(double x, double z) noexcept
{
    tempObject.destinationPosition.rot[0] = x;
    tempObject.destinationPosition.rot[2] = z;
}

void ObjectFunctions::SetPlayerAsObject(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    tempObject.guid = player->guid;
    tempObject.isPlayer = true;
}

void ObjectFunctions::SetContainerItemRefId(const char* refId) noexcept
{
    tempContainerItem.refId = refId;
}

void ObjectFunctions::SetContainerItemCount(int count) noexcept
{
    tempContainerItem.count = count;
}

void ObjectFunctions::SetContainerItemCharge(int charge) noexcept
{
    tempContainerItem.charge = charge;
}

void ObjectFunctions::SetContainerItemEnchantmentCharge(double enchantmentCharge) noexcept
{
    tempContainerItem.enchantmentCharge = enchantmentCharge;
}

void ObjectFunctions::SetContainerItemSoul(const char* soul) noexcept
{
    tempContainerItem.soul = soul;
}

void ObjectFunctions::SetContainerItemActionCountByIndex(unsigned int objectIndex, unsigned int itemIndex, int actionCount) noexcept
{
    writeObjectList.baseObjects.at(objectIndex).containerItems.at(itemIndex).actionCount = actionCount;
}

void ObjectFunctions::AddObject() noexcept
{
    writeObjectList.baseObjects.push_back(tempObject);

    tempObject = emptyObject;
}

void ObjectFunctions::AddClientLocalInteger(int internalIndex, int intValue, unsigned int variableType) noexcept
{
    ClientVariable clientLocal;
    clientLocal.internalIndex = internalIndex;
    clientLocal.intValue = intValue;
    clientLocal.variableType = variableType;

    tempObject.clientLocals.push_back(clientLocal);
}

void ObjectFunctions::AddClientLocalFloat(int internalIndex, double floatValue) noexcept
{
    ClientVariable clientLocal;
    clientLocal.internalIndex = internalIndex;
    clientLocal.floatValue = floatValue;
    clientLocal.variableType = mwmp::VARIABLE_TYPE::FLOAT;

    tempObject.clientLocals.push_back(clientLocal);
}

void ObjectFunctions::AddContainerItem() noexcept
{
    tempObject.containerItems.push_back(tempContainerItem);

    tempContainerItem = emptyContainerItem;
}

void ObjectFunctions::SendObjectActivate(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_ACTIVATE);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectPlace(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_PLACE);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectSpawn(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_SPAWN);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectDelete(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_DELETE);
    packet->setObjectList(&writeObjectList);
    
    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectLock(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_LOCK);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectDialogueChoice(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket* packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_DIALOGUE_CHOICE);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectMiscellaneous(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket* packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_MISCELLANEOUS);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectRestock(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_RESTOCK);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectTrap(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_TRAP);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectScale(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_SCALE);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectSound(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_SOUND);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectState(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_STATE);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectMove(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket* packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_MOVE);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendObjectRotate(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket* packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_OBJECT_ROTATE);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendDoorState(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_DOOR_STATE);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendDoorDestination(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_DOOR_DESTINATION);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendContainer(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_CONTAINER);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendVideoPlay(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_VIDEO_PLAY);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendClientScriptLocal(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket* packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_CLIENT_SCRIPT_LOCAL);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ObjectFunctions::SendConsoleCommand(bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    mwmp::ObjectPacket *packet = mwmp::Networking::get().getObjectPacketController()->GetPacket(ID_CONSOLE_COMMAND);
    packet->setObjectList(&writeObjectList);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}


// All methods below are deprecated versions of methods from above

void ObjectFunctions::ReadLastObjectList() noexcept
{
    ReadReceivedObjectList();
}

void ObjectFunctions::ReadLastEvent() noexcept
{
    ReadReceivedObjectList();
}

void ObjectFunctions::InitializeObjectList(unsigned short pid) noexcept
{
    ClearObjectList();
    SetObjectListPid(pid);
}

void ObjectFunctions::InitializeEvent(unsigned short pid) noexcept
{
    InitializeObjectList(pid);
}

void ObjectFunctions::CopyLastObjectListToStore() noexcept
{
    CopyReceivedObjectListToStore();
}

unsigned int ObjectFunctions::GetObjectChangesSize() noexcept
{
    return GetObjectListSize();
}

unsigned char ObjectFunctions::GetEventAction() noexcept
{
    return GetObjectListAction();
}

unsigned char ObjectFunctions::GetEventContainerSubAction() noexcept
{
    return GetObjectListContainerSubAction();
}

unsigned int ObjectFunctions::GetObjectRefNumIndex(unsigned int index) noexcept
{
    return GetObjectRefNum(index);
}

unsigned int ObjectFunctions::GetObjectSummonerRefNumIndex(unsigned int index) noexcept
{
    return GetObjectSummonerRefNum(index);
}

void ObjectFunctions::SetEventCell(const char* cellDescription) noexcept
{
    SetObjectListCell(cellDescription);
}

void ObjectFunctions::SetEventAction(unsigned char action) noexcept
{
    SetObjectListAction(action);
}

void ObjectFunctions::SetEventConsoleCommand(const char* consoleCommand) noexcept
{
    SetObjectListConsoleCommand(consoleCommand);
}

void ObjectFunctions::SetObjectRefNumIndex(int refNum) noexcept
{
    SetObjectRefNum(refNum);
}

void ObjectFunctions::SetObjectDisarmState(bool disarmState) noexcept
{
    if (disarmState)
        tempObject.trapAction = mwmp::BaseObjectList::TRAP_ACTION::DISARM;
    else
        tempObject.trapAction = mwmp::BaseObjectList::TRAP_ACTION::TRIGGER;
}

void ObjectFunctions::AddWorldObject() noexcept
{
    AddObject();
}
