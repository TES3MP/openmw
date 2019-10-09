#include "PacketRecordDynamic.hpp"

#include <components/openmw-mp/MWMPLog.hpp>
#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/Utils.hpp>

using namespace mwmp;

PacketRecordDynamic::PacketRecordDynamic(RakNet::RakPeerInterface *peer) : WorldstatePacket(peer)
{
    packetID = ID_RECORD_DYNAMIC;
    orderChannel = CHANNEL_WORLDSTATE;
}

void PacketRecordDynamic::Packet(RakNet::BitStream *bs, bool send)
{
    WorldstatePacket::Packet(bs, send);

    RW(worldstate->recordsType, send);

    if (send)
    {
        if (worldstate->recordsType == mwmp::RECORD_TYPE::SPELL)
            worldstate->recordsCount = Utils::getVectorSize(worldstate->spellRecords);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::POTION)
            worldstate->recordsCount = Utils::getVectorSize(worldstate->potionRecords);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::ENCHANTMENT)
            worldstate->recordsCount = Utils::getVectorSize(worldstate->enchantmentRecords);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::CREATURE)
            worldstate->recordsCount = Utils::getVectorSize(worldstate->creatureRecords);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::NPC)
            worldstate->recordsCount = Utils::getVectorSize(worldstate->npcRecords);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::ARMOR)
            worldstate->recordsCount = Utils::getVectorSize(worldstate->armorRecords);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::BOOK)
            worldstate->recordsCount = Utils::getVectorSize(worldstate->bookRecords);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::CLOTHING)
            worldstate->recordsCount = Utils::getVectorSize(worldstate->clothingRecords);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::MISCELLANEOUS)
            worldstate->recordsCount = Utils::getVectorSize(worldstate->miscellaneousRecords);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::WEAPON)
            worldstate->recordsCount = Utils::getVectorSize(worldstate->weaponRecords);
        else
        {
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, "Processed invalid ID_RECORD_DYNAMIC packet about unimplemented recordsType %i",
                worldstate->recordsType);
            return;
        }
    }

    RW(worldstate->recordsCount, send);

    if (worldstate->recordsCount > maxRecords)
    {
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, "Processed invalid ID_RECORD_DYNAMIC packet with %i records, above the maximum of %i",
            worldstate->recordsCount, maxRecords);
        LOG_APPEND(MWMPLog::LOG_ERROR, "- The packet was ignored after that point");
        return;
    }

    if (!send)
    {
        if (worldstate->recordsType == mwmp::RECORD_TYPE::SPELL)
            Utils::resetVector(worldstate->spellRecords, worldstate->recordsCount);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::POTION)
            Utils::resetVector(worldstate->potionRecords, worldstate->recordsCount);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::ENCHANTMENT)
            Utils::resetVector(worldstate->enchantmentRecords, worldstate->recordsCount);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::CREATURE)
            Utils::resetVector(worldstate->creatureRecords, worldstate->recordsCount);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::NPC)
            Utils::resetVector(worldstate->npcRecords, worldstate->recordsCount);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::ARMOR)
            Utils::resetVector(worldstate->armorRecords, worldstate->recordsCount);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::BOOK)
            Utils::resetVector(worldstate->bookRecords, worldstate->recordsCount);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::CLOTHING)
            Utils::resetVector(worldstate->clothingRecords, worldstate->recordsCount);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::MISCELLANEOUS)
            Utils::resetVector(worldstate->miscellaneousRecords, worldstate->recordsCount);
        else if (worldstate->recordsType == mwmp::RECORD_TYPE::WEAPON)
            Utils::resetVector(worldstate->weaponRecords, worldstate->recordsCount);
    }

    if (worldstate->recordsType == mwmp::RECORD_TYPE::SPELL)
    {
        for (auto &&record : worldstate->spellRecords)
        {
            auto &&recordData = record.data;

            RW(record.baseId, send, true);
            RW(recordData.mId, send, true);
            RW(recordData.mName, send, true);
            RW(recordData.mData.mType, send);
            RW(recordData.mData.mCost, send);
            RW(recordData.mData.mFlags, send);
            ProcessEffects(recordData.mEffects, send);

            if (!record.baseId.empty())
            {
                auto &&overrides = record.baseOverrides;
                RW(overrides.hasName, send);
                RW(overrides.hasSubtype, send);
                RW(overrides.hasCost, send);
                RW(overrides.hasFlags, send);
                RW(overrides.hasEffects, send);
            }
        }
    }
    else if (worldstate->recordsType == mwmp::RECORD_TYPE::POTION)
    {
        for (auto &&record : worldstate->potionRecords)
        {
            auto &recordData = record.data;

            RW(record.baseId, send, true);
            RW(recordData.mId, send, true);
            RW(recordData.mName, send, true);
            RW(recordData.mModel, send, true);
            RW(recordData.mIcon, send, true);
            RW(recordData.mData.mWeight, send);
            RW(recordData.mData.mValue, send);
            RW(recordData.mData.mAutoCalc, send);
            RW(recordData.mScript, send, true);
            ProcessEffects(recordData.mEffects, send);

            if (!record.baseId.empty())
            {
                auto &&overrides = record.baseOverrides;
                RW(overrides.hasName, send);
                RW(overrides.hasModel, send);
                RW(overrides.hasIcon, send);
                RW(overrides.hasWeight, send);
                RW(overrides.hasValue, send);
                RW(overrides.hasAutoCalc, send);
                RW(overrides.hasScript, send);
                RW(overrides.hasEffects, send);
            }
        }
    }
    else if (worldstate->recordsType == mwmp::RECORD_TYPE::ENCHANTMENT)
    {
        for (auto &&record : worldstate->enchantmentRecords)
        {
            auto &recordData = record.data;

            RW(record.baseId, send, true);
            RW(recordData.mId, send, true);
            RW(recordData.mData.mType, send);
            RW(recordData.mData.mCost, send);
            RW(recordData.mData.mCharge, send);
            RW(recordData.mData.mAutocalc, send);
            ProcessEffects(recordData.mEffects, send);

            if (!record.baseId.empty())
            {
                auto &&overrides = record.baseOverrides;
                RW(overrides.hasSubtype, send);
                RW(overrides.hasCost, send);
                RW(overrides.hasCharge, send);
                RW(overrides.hasAutoCalc, send);
                RW(overrides.hasEffects, send);
            }
        }
    }
    else if (worldstate->recordsType == mwmp::RECORD_TYPE::CREATURE)
    {
        for (auto &&record : worldstate->creatureRecords)
        {
            auto &recordData = record.data;

            RW(record.baseId, send, true);
            RW(record.inventoryBaseId, send, true);
            RW(recordData.mId, send, true);
            RW(recordData.mName, send, true);
            RW(recordData.mModel, send, true);
            RW(recordData.mData.mType, send);
            RW(recordData.mData.mLevel, send);
            RW(recordData.mData.mHealth, send);
            RW(recordData.mData.mMana, send);
            RW(recordData.mData.mFatigue, send);
            RW(recordData.mAiData.mFight, send);
            RW(recordData.mFlags, send);
            RW(recordData.mScript, send, true);
            ProcessInventoryList(record.inventory, recordData.mInventory, send);

            if (!record.baseId.empty())
            {
                auto &&overrides = record.baseOverrides;
                RW(overrides.hasName, send);
                RW(overrides.hasModel, send);
                RW(overrides.hasSubtype, send);
                RW(overrides.hasLevel, send);
                RW(overrides.hasHealth, send);
                RW(overrides.hasMagicka, send);
                RW(overrides.hasFatigue, send);
                RW(overrides.hasAiFight, send);
                RW(overrides.hasFlags, send);
                RW(overrides.hasScript, send);
                RW(overrides.hasInventory, send);
            }
        }
    }
    else if (worldstate->recordsType == mwmp::RECORD_TYPE::NPC)
    {
        for (auto &&record : worldstate->npcRecords)
        {
            auto &recordData = record.data;

            RW(record.baseId, send, true);
            RW(record.inventoryBaseId, send, true);
            RW(recordData.mId, send, true);
            RW(recordData.mName, send, true);
            RW(recordData.mFlags, send);
            RW(recordData.mRace, send, true);
            RW(recordData.mModel, send, true);
            RW(recordData.mHair, send, true);
            RW(recordData.mHead, send, true);
            RW(recordData.mClass, send, true);
            RW(recordData.mFaction, send, true);
            RW(recordData.mScript, send, true);
            RW(recordData.mNpdt.mLevel, send);
            RW(recordData.mNpdt.mHealth, send);
            RW(recordData.mNpdt.mMana, send);
            RW(recordData.mNpdt.mFatigue, send);
            RW(recordData.mAiData.mFight, send);
            RW(recordData.mNpdtType, send);
            ProcessInventoryList(record.inventory, recordData.mInventory, send);

            if (!record.baseId.empty())
            {
                auto &&overrides = record.baseOverrides;
                RW(overrides.hasName, send);
                RW(overrides.hasGender, send);
                RW(overrides.hasFlags, send);
                RW(overrides.hasModel, send);
                RW(overrides.hasHair, send);
                RW(overrides.hasHead, send);
                RW(overrides.hasFaction, send);
                RW(overrides.hasScript, send);
                RW(overrides.hasLevel, send);
                RW(overrides.hasHealth, send);
                RW(overrides.hasMagicka, send);
                RW(overrides.hasFatigue, send);
                RW(overrides.hasAiFight, send);
                RW(overrides.hasAutoCalc, send);
                RW(overrides.hasInventory, send);
            }
        }
    }
    else if (worldstate->recordsType == mwmp::RECORD_TYPE::ARMOR)
    {
        for (auto &&record : worldstate->armorRecords)
        {
            auto &recordData = record.data;

            RW(record.baseId, send, true);
            RW(recordData.mId, send, true);
            RW(recordData.mName, send, true);
            RW(recordData.mModel, send, true);
            RW(recordData.mIcon, send, true);
            RW(recordData.mData.mType, send);
            RW(recordData.mData.mWeight, send);
            RW(recordData.mData.mValue, send);
            RW(recordData.mData.mHealth, send);
            RW(recordData.mData.mArmor, send);
            RW(recordData.mData.mEnchant, send);
            RW(recordData.mEnchant, send, true);
            RW(recordData.mScript, send, true);
            ProcessBodyParts(recordData.mParts, send);

            if (!record.baseId.empty())
            {
                auto &&overrides = record.baseOverrides;
                RW(overrides.hasName, send);
                RW(overrides.hasModel, send);
                RW(overrides.hasIcon, send);
                RW(overrides.hasSubtype, send);
                RW(overrides.hasWeight, send);
                RW(overrides.hasValue, send);
                RW(overrides.hasHealth, send);
                RW(overrides.hasArmorRating, send);
                RW(overrides.hasEnchantmentCharge, send);
                RW(overrides.hasEnchantmentId, send);
                RW(overrides.hasScript, send);
                RW(overrides.hasBodyParts, send);
            }
        }
    }
    else if (worldstate->recordsType == mwmp::RECORD_TYPE::BOOK)
    {
        for (auto &&record : worldstate->bookRecords)
        {
            auto &recordData = record.data;

            RW(record.baseId, send, true);
            RW(recordData.mId, send, true);
            RW(recordData.mName, send, true);
            RW(recordData.mModel, send, true);
            RW(recordData.mIcon, send, true);
            RW(recordData.mText, send, true);
            RW(recordData.mData.mWeight, send);
            RW(recordData.mData.mValue, send);
            RW(recordData.mData.mIsScroll, send);
            RW(recordData.mData.mSkillId, send);
            RW(recordData.mData.mEnchant, send);
            RW(recordData.mEnchant, send, true);
            RW(recordData.mScript, send, true);

            if (!record.baseId.empty())
            {
                auto &&overrides = record.baseOverrides;
                RW(overrides.hasName, send);
                RW(overrides.hasModel, send);
                RW(overrides.hasIcon, send);
                RW(overrides.hasText, send);
                RW(overrides.hasWeight, send);
                RW(overrides.hasValue, send);
                RW(overrides.hasScrollState, send);
                RW(overrides.hasSkillId, send);
                RW(overrides.hasEnchantmentCharge, send);
                RW(overrides.hasEnchantmentId, send);
                RW(overrides.hasScript, send);
            }
        }
    }
    else if (worldstate->recordsType == mwmp::RECORD_TYPE::CLOTHING)
    {
        for (auto &&record : worldstate->clothingRecords)
        {
            auto &recordData = record.data;

            RW(record.baseId, send, true);
            RW(recordData.mId, send, true);
            RW(recordData.mName, send, true);
            RW(recordData.mModel, send, true);
            RW(recordData.mIcon, send, true);
            RW(recordData.mData.mType, send);
            RW(recordData.mData.mWeight, send);
            RW(recordData.mData.mValue, send);
            RW(recordData.mData.mEnchant, send);
            RW(recordData.mEnchant, send, true);
            RW(recordData.mScript, send, true);
            ProcessBodyParts(recordData.mParts, send);

            if (!record.baseId.empty())
            {
                auto &&overrides = record.baseOverrides;
                RW(overrides.hasName, send);
                RW(overrides.hasModel, send);
                RW(overrides.hasIcon, send);
                RW(overrides.hasSubtype, send);
                RW(overrides.hasWeight, send);
                RW(overrides.hasValue, send);
                RW(overrides.hasEnchantmentCharge, send);
                RW(overrides.hasEnchantmentId, send);
                RW(overrides.hasScript, send);
                RW(overrides.hasBodyParts, send);
            }
        }
    }
    else if (worldstate->recordsType == mwmp::RECORD_TYPE::MISCELLANEOUS)
    {
        for (auto &&record : worldstate->miscellaneousRecords)
        {
            auto &recordData = record.data;

            RW(record.baseId, send, true);
            RW(recordData.mId, send, true);
            RW(recordData.mName, send, true);
            RW(recordData.mModel, send, true);
            RW(recordData.mIcon, send, true);
            RW(recordData.mData.mWeight, send);
            RW(recordData.mData.mValue, send);
            RW(recordData.mData.mIsKey, send);
            RW(recordData.mScript, send, true);

            if (!record.baseId.empty())
            {
                auto &&overrides = record.baseOverrides;
                RW(overrides.hasName, send);
                RW(overrides.hasModel, send);
                RW(overrides.hasIcon, send);
                RW(overrides.hasWeight, send);
                RW(overrides.hasValue, send);
                RW(overrides.hasKeyState, send);
                RW(overrides.hasScript, send);
            }
        }
    }
    else if (worldstate->recordsType == mwmp::RECORD_TYPE::WEAPON)
    {
        for (auto &&record : worldstate->weaponRecords)
        {
            auto &recordData = record.data;

            RW(record.baseId, send, true);
            RW(recordData.mId, send, true);
            RW(recordData.mName, send, true);
            RW(recordData.mModel, send, true);
            RW(recordData.mIcon, send, true);
            RW(recordData.mData.mType, send);
            RW(recordData.mData.mWeight, send);
            RW(recordData.mData.mValue, send);
            RW(recordData.mData.mHealth, send);
            RW(recordData.mData.mSpeed, send);
            RW(recordData.mData.mReach, send);
            RW(recordData.mData.mChop[0], send);
            RW(recordData.mData.mChop[1], send);
            RW(recordData.mData.mSlash[0], send);
            RW(recordData.mData.mSlash[1], send);
            RW(recordData.mData.mThrust[0], send);
            RW(recordData.mData.mThrust[1], send);
            RW(recordData.mData.mFlags, send);
            RW(recordData.mData.mEnchant, send);
            RW(recordData.mEnchant, send, true);
            RW(recordData.mScript, send, true);

            if (!record.baseId.empty())
            {
                auto &&overrides = record.baseOverrides;
                RW(overrides.hasName, send);
                RW(overrides.hasModel, send);
                RW(overrides.hasIcon, send);
                RW(overrides.hasSubtype, send);
                RW(overrides.hasWeight, send);
                RW(overrides.hasValue, send);
                RW(overrides.hasHealth, send);
                RW(overrides.hasSpeed, send);
                RW(overrides.hasReach, send);
                RW(overrides.hasDamageChop, send);
                RW(overrides.hasDamageSlash, send);
                RW(overrides.hasDamageThrust, send);
                RW(overrides.hasFlags, send);
                RW(overrides.hasEnchantmentCharge, send);
                RW(overrides.hasEnchantmentId, send);
                RW(overrides.hasScript, send);
            }
        }
    }
}

void PacketRecordDynamic::ProcessEffects(ESM::EffectList &effectList, bool send)
{
    uint32_t effectCount;

    if (send)
        effectCount = static_cast<uint32_t>(effectList.mList.size());

    RW(effectCount, send);

    if (effectCount > maxEffects)
    {
        return;
    }

    if (!send)
    {
        effectList.mList.clear();
        effectList.mList.resize(effectCount);
    }

    for (auto &&effect : effectList.mList)
    {
        RW(effect.mEffectID, send);
        RW(effect.mAttribute, send);
        RW(effect.mSkill, send);
        RW(effect.mRange, send);
        RW(effect.mArea, send);
        RW(effect.mDuration, send);
        RW(effect.mMagnMax, send);
        RW(effect.mMagnMin, send);
    }
}

void PacketRecordDynamic::ProcessBodyParts(ESM::PartReferenceList &partList, bool send)
{
    uint32_t partCount;

    if (send)
        partCount = static_cast<uint32_t>(partList.mParts.size());

    RW(partCount, send);

    if (partCount > maxParts)
    {
        return;
    }

    if (!send)
    {
        partList.mParts.clear();
        partList.mParts.resize(partCount);
    }

    for (auto &&part : partList.mParts)
    {
        RW(part.mPart, send);
        RW(part.mMale, send, true);
        RW(part.mFemale, send, true);
    }
}

// ESM::InventoryList has a strange structure that makes it hard to read in packets directly, so we just deal with it
// here with the help of a separate mwmp::Item vector
void PacketRecordDynamic::ProcessInventoryList(std::vector<mwmp::Item> &inventory, ESM::InventoryList &inventoryList, bool send)
{
    uint32_t itemCount;

    if (send)
        itemCount = static_cast<uint32_t>(inventory.size());

    RW(itemCount, send);

    if (itemCount > maxItems)
    {
        return;
    }

    if (!send)
    {
        inventory.clear();
        inventory.resize(itemCount);
        inventoryList.mList.clear();
    }

    for (auto &&item : inventory)
    {
        RW(item.refId, send, true);
        RW(item.count, send, true);

        if (!send)
        {
            ESM::ContItem contItem;
            contItem.mItem.assign(item.refId);
            contItem.mCount = item.count;
            inventoryList.mList.push_back(contItem);
        }
    }
}
