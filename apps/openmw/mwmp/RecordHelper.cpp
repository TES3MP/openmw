#include <components/openmw-mp/MWMPLog.hpp>

#include "../mwbase/environment.hpp"

#include "../mwworld/worldimp.hpp"

#include "RecordHelper.hpp"

bool RecordHelper::doesClassRecordExist(const std::string& id)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->getStore().get<ESM::Class>().search(id);
}

bool RecordHelper::doesRaceRecordExist(const std::string& id)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->getStore().get<ESM::Race>().search(id);
}

bool RecordHelper::doesCreatureRecordExist(const std::string& id)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->getStore().get<ESM::Creature>().search(id);
}

bool RecordHelper::doesNpcRecordExist(const std::string& id)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->getStore().get<ESM::NPC>().search(id);
}

bool RecordHelper::doesEnchantmentRecordExist(const std::string& id)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->getStore().get<ESM::Enchantment>().search(id);
}

bool RecordHelper::doesPotionRecordExist(const std::string& id)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->getStore().get<ESM::Potion>().search(id);
}

bool RecordHelper::doesSpellRecordExist(const std::string& id)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->getStore().get<ESM::Spell>().search(id);
}

bool RecordHelper::doesArmorRecordExist(const std::string& id)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->getStore().get<ESM::Armor>().search(id);
}

bool RecordHelper::doesBookRecordExist(const std::string& id)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->getStore().get<ESM::Book>().search(id);
}

bool RecordHelper::doesClothingRecordExist(const std::string& id)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->getStore().get<ESM::Clothing>().search(id);
}

bool RecordHelper::doesMiscellaneousRecordExist(const std::string& id)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->getStore().get<ESM::Miscellaneous>().search(id);
}

bool RecordHelper::doesWeaponRecordExist(const std::string& id)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->getStore().get<ESM::Weapon>().search(id);
}

std::string RecordHelper::createCreatureRecord(const ESM::Creature& record)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->createRecord(record)->mId;
}

std::string RecordHelper::createNpcRecord(const ESM::NPC& record)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    return world->createRecord(record)->mId;
}

void RecordHelper::overrideCreatureRecord(const mwmp::CreatureRecord& record)
{
    const ESM::Creature &recordData = record.data;

    if (recordData.mId.empty())
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with no id provided");
        return;
    }

    bool isExistingId = doesCreatureRecordExist(recordData.mId);
    MWBase::World *world = MWBase::Environment::get().getWorld();

    if (record.baseId.empty())
    {
        world->getModifiableStore().overrideRecord(recordData);
    }
    else if (doesCreatureRecordExist(record.baseId))
    {
        const ESM::Creature *baseData = world->getStore().get<ESM::Creature>().search(record.baseId);
        ESM::Creature finalData = *baseData;
        finalData.mId = recordData.mId;

        if (record.baseOverrides.hasName)
            finalData.mName = recordData.mName;

        if (record.baseOverrides.hasModel)
            finalData.mModel = recordData.mModel;

        if (record.baseOverrides.hasSubtype)
            finalData.mData.mType = recordData.mData.mType;

        if (record.baseOverrides.hasLevel)
            finalData.mData.mLevel = recordData.mData.mLevel;

        if (record.baseOverrides.hasHealth)
            finalData.mData.mHealth = recordData.mData.mHealth;

        if (record.baseOverrides.hasMagicka)
            finalData.mData.mMana = recordData.mData.mMana;

        if (record.baseOverrides.hasFatigue)
            finalData.mData.mFatigue = recordData.mData.mFatigue;

        if (record.baseOverrides.hasAiFight)
            finalData.mAiData.mFight = recordData.mAiData.mFight;

        if (record.baseOverrides.hasFlags)
            finalData.mFlags = recordData.mFlags;

        if (record.baseOverrides.hasScript)
            finalData.mScript = recordData.mScript;

        if (!record.inventoryBaseId.empty() && doesNpcRecordExist(record.inventoryBaseId))
            finalData.mInventory.mList = world->getStore().get<ESM::Creature>().search(record.inventoryBaseId)->mInventory.mList;
        else if (record.baseOverrides.hasInventory)
            finalData.mInventory.mList = recordData.mInventory.mList;

        world->getModifiableStore().overrideRecord(finalData);
    }
    else
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with invalid baseId %s", record.baseId.c_str());
        return;
    }

    if (isExistingId)
        world->updatePtrsWithRefId(recordData.mId);
}

void RecordHelper::overrideNpcRecord(const mwmp::NpcRecord& record)
{
    const ESM::NPC &recordData = record.data;

    if (recordData.mId.empty())
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with no id provided");
        return;
    }

    bool isExistingId = doesNpcRecordExist(recordData.mId);
    MWBase::World *world = MWBase::Environment::get().getWorld();

    if (record.baseId.empty())
    {
        if (!doesRaceRecordExist(recordData.mRace))
        {
            LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring new NPC record with invalid race provided");
            return;
        }
        else if (!doesClassRecordExist(recordData.mClass))
        {
            LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring new NPC record with invalid class provided");
            return;
        }
        else
            world->getModifiableStore().overrideRecord(recordData);
    }
    else if (doesNpcRecordExist(record.baseId))
    {
        const ESM::NPC *baseData = world->getStore().get<ESM::NPC>().search(record.baseId);
        ESM::NPC finalData = *baseData;
        finalData.mId = recordData.mId;

        if (record.baseOverrides.hasName)
            finalData.mName = recordData.mName;

        if (record.baseOverrides.hasFlags)
            finalData.mFlags = recordData.mFlags;

        // Because the gender is part of mFlags and can easily be set incorrectly there,
        // we handle it separately here
        if (record.baseOverrides.hasGender)
            finalData.setIsMale(recordData.isMale());
        else
            finalData.setIsMale(baseData->isMale());

        if (!record.data.mRace.empty())
            finalData.mRace = recordData.mRace;

        if (record.baseOverrides.hasModel)
            finalData.mModel = recordData.mModel;

        if (record.baseOverrides.hasHair)
            finalData.mHair = recordData.mHair;

        if (record.baseOverrides.hasHead)
            finalData.mHead = recordData.mHead;

        if (!recordData.mClass.empty())
            finalData.mClass = recordData.mClass;

        if (record.baseOverrides.hasFaction)
            finalData.mFaction = recordData.mFaction;

        if (record.baseOverrides.hasScript)
            finalData.mScript = recordData.mScript;

        if (record.baseOverrides.hasLevel)
            finalData.mNpdt.mLevel = recordData.mNpdt.mLevel;

        if (record.baseOverrides.hasHealth)
            finalData.mNpdt.mHealth = recordData.mNpdt.mHealth;

        if (record.baseOverrides.hasMagicka)
            finalData.mNpdt.mMana = recordData.mNpdt.mMana;

        if (record.baseOverrides.hasFatigue)
            finalData.mNpdt.mFatigue = recordData.mNpdt.mFatigue;

        if (record.baseOverrides.hasAiFight)
            finalData.mAiData.mFight = recordData.mAiData.mFight;

        if (record.baseOverrides.hasFlags)
            finalData.mFlags = recordData.mFlags;

        if (record.baseOverrides.hasAutoCalc)
        {
            finalData.mNpdtType = recordData.mNpdtType;

            if ((recordData.mFlags & ESM::NPC::Autocalc) != 0)
                finalData.mFlags |= ESM::NPC::Autocalc;
            else
                finalData.mFlags &= ~ESM::NPC::Autocalc;
        }

        if (!record.inventoryBaseId.empty() && doesNpcRecordExist(record.inventoryBaseId))
            finalData.mInventory.mList = world->getStore().get<ESM::NPC>().search(record.inventoryBaseId)->mInventory.mList;
        else if (record.baseOverrides.hasInventory)
            finalData.mInventory.mList = recordData.mInventory.mList;

        world->getModifiableStore().overrideRecord(finalData);
    }
    else
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with invalid baseId %s", record.baseId.c_str());
        return;
    }

    if (isExistingId)
        world->updatePtrsWithRefId(recordData.mId);
}

void RecordHelper::overrideEnchantmentRecord(const mwmp::EnchantmentRecord& record)
{
    const ESM::Enchantment &recordData = record.data;

    if (recordData.mId.empty())
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with no id provided");
        return;
    }

    MWBase::World *world = MWBase::Environment::get().getWorld();

    if (record.baseId.empty())
    {
        if (recordData.mEffects.mList.empty())
        {
            LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring new enchantment record with no effects");
            return;
        }
        else
            world->getModifiableStore().overrideRecord(recordData);
    }
    else if (doesEnchantmentRecordExist(record.baseId))
    {
        const ESM::Enchantment *baseData = world->getStore().get<ESM::Enchantment>().search(record.baseId);
        ESM::Enchantment finalData = *baseData;
        finalData.mId = recordData.mId;

        if (record.baseOverrides.hasSubtype)
            finalData.mData.mType = recordData.mData.mType;

        if (record.baseOverrides.hasCost)
            finalData.mData.mCost = recordData.mData.mCost;

        if (record.baseOverrides.hasCharge)
            finalData.mData.mCharge = recordData.mData.mCharge;

        if (record.baseOverrides.hasAutoCalc)
            finalData.mData.mAutocalc = recordData.mData.mAutocalc;

        if (record.baseOverrides.hasEffects)
            finalData.mEffects.mList = recordData.mEffects.mList;

        world->getModifiableStore().overrideRecord(finalData);
    }
    else
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with invalid baseId %s", record.baseId.c_str());
        return;
    }
}

void RecordHelper::overridePotionRecord(const mwmp::PotionRecord& record)
{
    const ESM::Potion &recordData = record.data;

    if (recordData.mId.empty())
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with no id provided");
        return;
    }

    bool isExistingId = doesPotionRecordExist(recordData.mId);
    MWBase::World *world = MWBase::Environment::get().getWorld();

    if (record.baseId.empty())
    {
        world->getModifiableStore().overrideRecord(recordData);
    }
    else if (doesPotionRecordExist(record.baseId))
    {
        const ESM::Potion *baseData = world->getStore().get<ESM::Potion>().search(record.baseId);
        ESM::Potion finalData = *baseData;
        finalData.mId = recordData.mId;

        if (record.baseOverrides.hasName)
            finalData.mName = recordData.mName;

        if (record.baseOverrides.hasModel)
            finalData.mModel = recordData.mModel;

        if (record.baseOverrides.hasIcon)
            finalData.mIcon = recordData.mIcon;

        if (record.baseOverrides.hasWeight)
            finalData.mData.mWeight = recordData.mData.mWeight;

        if (record.baseOverrides.hasValue)
            finalData.mData.mValue = recordData.mData.mValue;

        if (record.baseOverrides.hasAutoCalc)
            finalData.mData.mAutoCalc = recordData.mData.mAutoCalc;

        if (record.baseOverrides.hasScript)
            finalData.mScript = recordData.mScript;

        if (record.baseOverrides.hasEffects)
            finalData.mEffects.mList = recordData.mEffects.mList;

        world->getModifiableStore().overrideRecord(finalData);
    }
    else
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with invalid baseId %s", record.baseId.c_str());
        return;
    }

    if (isExistingId)
        world->updatePtrsWithRefId(recordData.mId);
}

void RecordHelper::overrideSpellRecord(const mwmp::SpellRecord& record)
{
    const ESM::Spell &recordData = record.data;

    if (recordData.mId.empty())
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with no id provided");
        return;
    }

    bool isExistingId = doesSpellRecordExist(recordData.mId);
    MWBase::World *world = MWBase::Environment::get().getWorld();

    if (record.baseId.empty())
    {
        world->getModifiableStore().overrideRecord(recordData);
    }
    else if (doesSpellRecordExist(record.baseId))
    {
        const ESM::Spell *baseData = world->getStore().get<ESM::Spell>().search(record.baseId);
        ESM::Spell finalData = *baseData;
        finalData.mId = recordData.mId;

        if (record.baseOverrides.hasName)
            finalData.mName = recordData.mName;

        if (record.baseOverrides.hasSubtype)
            finalData.mData.mType = recordData.mData.mType;

        if (record.baseOverrides.hasCost)
            finalData.mData.mCost = recordData.mData.mCost;

        if (record.baseOverrides.hasFlags)
            finalData.mData.mFlags = recordData.mData.mFlags;

        if (record.baseOverrides.hasEffects)
            finalData.mEffects.mList = recordData.mEffects.mList;

        world->getModifiableStore().overrideRecord(finalData);
    }
    else
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with invalid baseId %s", record.baseId.c_str());
        return;
    }
}

void RecordHelper::overrideArmorRecord(const mwmp::ArmorRecord& record)
{
    const ESM::Armor &recordData = record.data;

    if (recordData.mId.empty())
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with no id provided");
        return;
    }

    bool isExistingId = doesArmorRecordExist(recordData.mId);
    MWBase::World *world = MWBase::Environment::get().getWorld();

    if (record.baseId.empty())
    {
        if (!recordData.mEnchant.empty() && !doesEnchantmentRecordExist(recordData.mEnchant))
        {
            LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring new armor record with invalid enchantmentId %s", recordData.mEnchant.c_str());
            return;
        }
        else
            world->getModifiableStore().overrideRecord(recordData);
    }
    else if (doesArmorRecordExist(record.baseId))
    {
        const ESM::Armor *baseData = world->getStore().get<ESM::Armor>().search(record.baseId);
        ESM::Armor finalData = *baseData;
        finalData.mId = recordData.mId;

        finalData.mParts.mParts.at(0);

        if (record.baseOverrides.hasName)
            finalData.mName = recordData.mName;

        if (record.baseOverrides.hasModel)
            finalData.mModel = recordData.mModel;

        if (record.baseOverrides.hasIcon)
            finalData.mIcon = recordData.mIcon;

        if (record.baseOverrides.hasSubtype)
            finalData.mData.mType = recordData.mData.mType;

        if (record.baseOverrides.hasWeight)
            finalData.mData.mWeight = recordData.mData.mWeight;

        if (record.baseOverrides.hasValue)
            finalData.mData.mValue = recordData.mData.mValue;

        if (record.baseOverrides.hasHealth)
            finalData.mData.mHealth = recordData.mData.mHealth;

        if (record.baseOverrides.hasArmorRating)
            finalData.mData.mArmor = recordData.mData.mArmor;

        if (record.baseOverrides.hasEnchantmentId)
        {
            if (doesEnchantmentRecordExist(recordData.mEnchant))
                finalData.mEnchant = recordData.mEnchant;
            else
                LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring invalid enchantmentId %s", recordData.mEnchant.c_str());
        }

        if (record.baseOverrides.hasEnchantmentCharge)
            finalData.mData.mEnchant = recordData.mData.mEnchant;

        if (record.baseOverrides.hasScript)
            finalData.mScript = recordData.mScript;

        if (record.baseOverrides.hasBodyParts)
            finalData.mParts.mParts = recordData.mParts.mParts;

        world->getModifiableStore().overrideRecord(finalData);
    }
    else
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with invalid baseId %s", record.baseId.c_str());
        return;
    }

    if (isExistingId)
        world->updatePtrsWithRefId(recordData.mId);
}

void RecordHelper::overrideBookRecord(const mwmp::BookRecord& record)
{
    const ESM::Book &recordData = record.data;

    if (recordData.mId.empty())
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with no id provided");
        return;
    }

    bool isExistingId = doesBookRecordExist(recordData.mId);
    MWBase::World *world = MWBase::Environment::get().getWorld();

    if (record.baseId.empty())
    {
        if (!recordData.mEnchant.empty() && !doesEnchantmentRecordExist(recordData.mEnchant))
        {
            LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring new book record with invalid enchantmentId %s", recordData.mEnchant.c_str());
            return;
        }
        else
            world->getModifiableStore().overrideRecord(recordData);
    }
    else if (doesBookRecordExist(record.baseId))
    {
        const ESM::Book *baseData = world->getStore().get<ESM::Book>().search(record.baseId);
        ESM::Book finalData = *baseData;
        finalData.mId = recordData.mId;

        if (record.baseOverrides.hasName)
            finalData.mName = recordData.mName;

        if (record.baseOverrides.hasModel)
            finalData.mModel = recordData.mModel;

        if (record.baseOverrides.hasIcon)
            finalData.mIcon = recordData.mIcon;

        if (record.baseOverrides.hasText)
            finalData.mText = recordData.mText;

        if (record.baseOverrides.hasWeight)
            finalData.mData.mWeight = recordData.mData.mWeight;

        if (record.baseOverrides.hasValue)
            finalData.mData.mValue = recordData.mData.mValue;

        if (record.baseOverrides.hasScrollState)
            finalData.mData.mIsScroll = recordData.mData.mIsScroll;

        if (record.baseOverrides.hasSkillId)
            finalData.mData.mSkillId = recordData.mData.mSkillId;

        if (record.baseOverrides.hasEnchantmentId)
        {
            if (doesEnchantmentRecordExist(recordData.mEnchant))
                finalData.mEnchant = recordData.mEnchant;
            else
                LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring invalid enchantmentId %s", recordData.mEnchant.c_str());
        }

        if (record.baseOverrides.hasEnchantmentCharge)
            finalData.mData.mEnchant = recordData.mData.mEnchant;

        if (record.baseOverrides.hasScript)
            finalData.mScript = recordData.mScript;

        world->getModifiableStore().overrideRecord(finalData);
    }
    else
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with invalid baseId %s", record.baseId.c_str());
        return;
    }

    if (isExistingId)
        world->updatePtrsWithRefId(recordData.mId);
}

void RecordHelper::overrideClothingRecord(const mwmp::ClothingRecord& record)
{
    const ESM::Clothing &recordData = record.data;

    if (recordData.mId.empty())
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with no id provided");
        return;
    }

    bool isExistingId = doesClothingRecordExist(recordData.mId);
    MWBase::World *world = MWBase::Environment::get().getWorld();

    if (record.baseId.empty())
    {
        if (!recordData.mEnchant.empty() && !doesEnchantmentRecordExist(recordData.mEnchant))
        {
            LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring new clothing record with invalid enchantmentId %s", recordData.mEnchant.c_str());
            return;
        }
        else
            world->getModifiableStore().overrideRecord(recordData);
    }
    else if (doesClothingRecordExist(record.baseId))
    {
        const ESM::Clothing *baseData = world->getStore().get<ESM::Clothing>().search(record.baseId);
        ESM::Clothing finalData = *baseData;
        finalData.mId = recordData.mId;

        if (record.baseOverrides.hasName)
            finalData.mName = recordData.mName;

        if (record.baseOverrides.hasModel)
            finalData.mModel = recordData.mModel;

        if (record.baseOverrides.hasIcon)
            finalData.mIcon = recordData.mIcon;

        if (record.baseOverrides.hasSubtype)
            finalData.mData.mType = recordData.mData.mType;

        if (record.baseOverrides.hasWeight)
            finalData.mData.mWeight = recordData.mData.mWeight;

        if (record.baseOverrides.hasValue)
            finalData.mData.mValue = recordData.mData.mValue;

        if (record.baseOverrides.hasEnchantmentId)
        {
            if (doesEnchantmentRecordExist(recordData.mEnchant))
                finalData.mEnchant = recordData.mEnchant;
            else
                LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring invalid enchantmentId %s", recordData.mEnchant.c_str());
        }

        if (record.baseOverrides.hasEnchantmentCharge)
            finalData.mData.mEnchant = recordData.mData.mEnchant;

        if (record.baseOverrides.hasScript)
            finalData.mScript = recordData.mScript;

        if (record.baseOverrides.hasBodyParts)
            finalData.mParts.mParts = recordData.mParts.mParts;

        world->getModifiableStore().overrideRecord(finalData);
    }
    else
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with invalid baseId %s", record.baseId.c_str());
        return;
    }

    if (isExistingId)
        world->updatePtrsWithRefId(recordData.mId);
}

void RecordHelper::overrideMiscellaneousRecord(const mwmp::MiscellaneousRecord& record)
{
    const ESM::Miscellaneous &recordData = record.data;

    if (recordData.mId.empty())
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with no id provided");
        return;
    }

    bool isExistingId = doesMiscellaneousRecordExist(recordData.mId);
    MWBase::World *world = MWBase::Environment::get().getWorld();

    if (record.baseId.empty())
    {
        world->getModifiableStore().overrideRecord(recordData);
    }
    else if (doesMiscellaneousRecordExist(record.baseId))
    {
        const ESM::Miscellaneous *baseData = world->getStore().get<ESM::Miscellaneous>().search(record.baseId);
        ESM::Miscellaneous finalData = *baseData;
        finalData.mId = recordData.mId;

        if (record.baseOverrides.hasName)
            finalData.mName = recordData.mName;

        if (record.baseOverrides.hasModel)
            finalData.mModel = recordData.mModel;

        if (record.baseOverrides.hasIcon)
            finalData.mIcon = recordData.mIcon;

        if (record.baseOverrides.hasWeight)
            finalData.mData.mWeight = recordData.mData.mWeight;

        if (record.baseOverrides.hasValue)
            finalData.mData.mValue = recordData.mData.mValue;

        if (record.baseOverrides.hasKeyState)
            finalData.mData.mIsKey = recordData.mData.mIsKey;

        if (record.baseOverrides.hasScript)
            finalData.mScript = recordData.mScript;

        world->getModifiableStore().overrideRecord(finalData);
    }
    else
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with invalid baseId %s", record.baseId.c_str());
        return;
    }

    if (isExistingId)
        world->updatePtrsWithRefId(recordData.mId);
}

void RecordHelper::overrideWeaponRecord(const mwmp::WeaponRecord& record)
{
    const ESM::Weapon &recordData = record.data;

    if (recordData.mId.empty())
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with no id provided");
        return;
    }

    bool isExistingId = doesWeaponRecordExist(recordData.mId);
    MWBase::World *world = MWBase::Environment::get().getWorld();

    if (record.baseId.empty())
    {
        if (!recordData.mEnchant.empty() && !doesEnchantmentRecordExist(recordData.mEnchant))
        {
            LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring new weapon record with invalid enchantmentId %s", recordData.mEnchant.c_str());
            return;
        }
        else
            world->getModifiableStore().overrideRecord(recordData);
    }
    else if (doesWeaponRecordExist(record.baseId))
    {
        const ESM::Weapon *baseData = world->getStore().get<ESM::Weapon>().search(record.baseId);
        ESM::Weapon finalData = *baseData;
        finalData.mId = recordData.mId;

        if (record.baseOverrides.hasName)
            finalData.mName = recordData.mName;

        if (record.baseOverrides.hasModel)
            finalData.mModel = recordData.mModel;

        if (record.baseOverrides.hasIcon)
            finalData.mIcon = recordData.mIcon;

        if (record.baseOverrides.hasSubtype)
            finalData.mData.mType = recordData.mData.mType;

        if (record.baseOverrides.hasWeight)
            finalData.mData.mWeight = recordData.mData.mWeight;

        if (record.baseOverrides.hasValue)
            finalData.mData.mValue = recordData.mData.mValue;

        if (record.baseOverrides.hasHealth)
            finalData.mData.mHealth = recordData.mData.mHealth;

        if (record.baseOverrides.hasSpeed)
            finalData.mData.mSpeed = recordData.mData.mSpeed;

        if (record.baseOverrides.hasReach)
            finalData.mData.mReach = recordData.mData.mReach;

        if (record.baseOverrides.hasDamageChop)
        {
            finalData.mData.mChop[0] = recordData.mData.mChop[0];
            finalData.mData.mChop[1] = recordData.mData.mChop[1];
        }

        if (record.baseOverrides.hasDamageSlash)
        {
            finalData.mData.mSlash[0] = recordData.mData.mSlash[0];
            finalData.mData.mSlash[1] = recordData.mData.mSlash[1];
        }

        if (record.baseOverrides.hasDamageThrust)
        {
            finalData.mData.mThrust[0] = recordData.mData.mThrust[0];
            finalData.mData.mThrust[1] = recordData.mData.mThrust[1];
        }

        if (record.baseOverrides.hasFlags)
            finalData.mData.mFlags = recordData.mData.mFlags;

        if (record.baseOverrides.hasEnchantmentId)
        {
            if (doesEnchantmentRecordExist(recordData.mEnchant))
                finalData.mEnchant = recordData.mEnchant;
            else
                LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring invalid enchantmentId %s", recordData.mEnchant.c_str());
        }

        if (record.baseOverrides.hasEnchantmentCharge)
            finalData.mData.mEnchant = recordData.mData.mEnchant;

        if (record.baseOverrides.hasScript)
            finalData.mScript = recordData.mScript;

        world->getModifiableStore().overrideRecord(finalData);
    }
    else
    {
        LOG_APPEND(MWMPLog::LOG_INFO, "-- Ignoring record override with invalid baseId %s", record.baseId.c_str());
        return;
    }

    if (isExistingId)
        world->updatePtrsWithRefId(recordData.mId);
}

void RecordHelper::overrideCreatureRecord(const ESM::Creature& record)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    world->getModifiableStore().overrideRecord(record);
}

void RecordHelper::overrideNpcRecord(const ESM::NPC& record)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    world->getModifiableStore().overrideRecord(record);
}

void RecordHelper::overrideEnchantmentRecord(const ESM::Enchantment& record)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    world->getModifiableStore().overrideRecord(record);
}

void RecordHelper::overridePotionRecord(const ESM::Potion& record)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    world->getModifiableStore().overrideRecord(record);
}

void RecordHelper::overrideSpellRecord(const ESM::Spell& record)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    world->getModifiableStore().overrideRecord(record);
}

void RecordHelper::overrideArmorRecord(const ESM::Armor& record)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    world->getModifiableStore().overrideRecord(record);
}

void RecordHelper::overrideBookRecord(const ESM::Book& record)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    world->getModifiableStore().overrideRecord(record);
}

void RecordHelper::overrideClothingRecord(const ESM::Clothing& record)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    world->getModifiableStore().overrideRecord(record);
}

void RecordHelper::overrideMiscellaneousRecord(const ESM::Miscellaneous& record)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    world->getModifiableStore().overrideRecord(record);
}

void RecordHelper::overrideWeaponRecord(const ESM::Weapon& record)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    world->getModifiableStore().overrideRecord(record);
}
