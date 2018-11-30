#include <components/openmw-mp/Log.hpp>

#include "../mwbase/environment.hpp"

#include "../mwgui/windowmanagerimp.hpp"

#include "../mwworld/player.hpp"
#include "../mwworld/worldimp.hpp"

#include "Worldstate.hpp"
#include "Main.hpp"
#include "Networking.hpp"
#include "RecordHelper.hpp"

using namespace mwmp;
using namespace std;

Worldstate::Worldstate()
{
    hasPlayerCollision = true;
    hasActorCollision = true;
    hasPlacedObjectCollision = false;
    useActorCollisionForPlacedObjects = false;
}

Worldstate::~Worldstate()
{

}

Networking *Worldstate::getNetworking()
{
    return mwmp::Main::get().getNetworking();
}

void Worldstate::addRecords()
{
    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Received ID_RECORD_DYNAMIC with %i records of type %i",
        recordsCount, recordsType);

    if (recordsType == mwmp::RECORD_TYPE::SPELL)
    {
        for (auto &&record : spellRecords)
        {
            bool hasBaseId = !record.baseId.empty();

            LOG_APPEND(Log::LOG_INFO, "- spell record %s, %s\n-- baseId is %s", record.data.mId.c_str(), record.data.mName.c_str(),
                hasBaseId ? record.baseId.c_str() : "empty");

            RecordHelper::overrideSpellRecord(record);
        }
    }
    else if (recordsType == mwmp::RECORD_TYPE::POTION)
    {
        for (auto &&record : potionRecords)
        {
            bool hasBaseId = !record.baseId.empty();

            LOG_APPEND(Log::LOG_INFO, "- potion record %s, %s\n-- baseId is %s", record.data.mId.c_str(), record.data.mName.c_str(),
                hasBaseId ? record.baseId.c_str() : "empty");

            RecordHelper::overridePotionRecord(record);
        }
    }
    else if (recordsType == mwmp::RECORD_TYPE::ENCHANTMENT)
    {
        for (auto &&record : enchantmentRecords)
        {
            bool hasBaseId = !record.baseId.empty();

            LOG_APPEND(Log::LOG_INFO, "- enchantment record %s, %i\n-- baseId is %s", record.data.mId.c_str(), record.data.mData.mType,
                hasBaseId ? record.baseId.c_str() : "empty");

            RecordHelper::overrideEnchantmentRecord(record);
        }
    }
    else if (recordsType == mwmp::RECORD_TYPE::CREATURE)
    {
        for (auto &&record : creatureRecords)
        {
            bool hasBaseId = !record.baseId.empty();

            LOG_APPEND(Log::LOG_INFO, "- creature record %s, %s\n-- baseId is %s", record.data.mId.c_str(), record.data.mName.c_str(),
                hasBaseId ? record.baseId.c_str() : "empty");

            RecordHelper::overrideCreatureRecord(record);
        }
    }
    else if (recordsType == mwmp::RECORD_TYPE::NPC)
    {
        for (auto &&record : npcRecords)
        {
            bool hasBaseId = !record.baseId.empty();

            LOG_APPEND(Log::LOG_INFO, "- NPC record %s, %s\n-- baseId is %s", record.data.mId.c_str(), record.data.mName.c_str(),
                hasBaseId ? record.baseId.c_str() : "empty");

            RecordHelper::overrideNpcRecord(record);
        }
    }
    else if (recordsType == mwmp::RECORD_TYPE::ARMOR)
    {
        for (auto &&record : armorRecords)
        {
            bool hasBaseId = !record.baseId.empty();

            LOG_APPEND(Log::LOG_INFO, "- armor record %s, %s\n-- baseId is %s", record.data.mId.c_str(), record.data.mName.c_str(),
                hasBaseId ? record.baseId.c_str() : "empty");

            RecordHelper::overrideArmorRecord(record);
        }
    }
    else if (recordsType == mwmp::RECORD_TYPE::BOOK)
    {
        for (auto &&record : bookRecords)
        {
            bool hasBaseId = !record.baseId.empty();

            LOG_APPEND(Log::LOG_INFO, "- book record %s, %s\n-- baseId is %s", record.data.mId.c_str(), record.data.mName.c_str(),
                hasBaseId ? record.baseId.c_str() : "empty");

            RecordHelper::overrideBookRecord(record);
        }
    }
    else if (recordsType == mwmp::RECORD_TYPE::CLOTHING)
    {
        for (auto &&record : clothingRecords)
        {
            bool hasBaseId = !record.baseId.empty();

            LOG_APPEND(Log::LOG_INFO, "- clothing record %s, %s\n-- baseId is %s", record.data.mId.c_str(), record.data.mName.c_str(),
                hasBaseId ? record.baseId.c_str() : "empty");

            RecordHelper::overrideClothingRecord(record);
        }
    }
    else if (recordsType == mwmp::RECORD_TYPE::MISCELLANEOUS)
    {
        for (auto &&record : miscellaneousRecords)
        {
            bool hasBaseId = !record.baseId.empty();

            LOG_APPEND(Log::LOG_INFO, "- miscellaneous record %s, %s\n-- baseId is %s", record.data.mId.c_str(), record.data.mName.c_str(),
                hasBaseId ? record.baseId.c_str() : "empty");

            RecordHelper::overrideMiscellaneousRecord(record);
        }
    }
    else if (recordsType == mwmp::RECORD_TYPE::WEAPON)
    {
        for (auto &&record : weaponRecords)
        {
            bool hasBaseId = !record.baseId.empty();

            LOG_APPEND(Log::LOG_INFO, "- weapon record %s, %s\n-- baseId is %s", record.data.mId.c_str(), record.data.mName.c_str(),
                hasBaseId ? record.baseId.c_str() : "empty");

            RecordHelper::overrideWeaponRecord(record);
        }
    }
}

bool Worldstate::containsExploredMapTile(int cellX, int cellY)
{
    for (const auto &mapTile : exploredMapTiles)
    {
        if (mapTile.x == cellX && mapTile.y == cellY)
            return true;
    }

    return false;
}

void Worldstate::markExploredMapTile(int cellX, int cellY)
{
    mwmp::MapTile exploredTile;
    exploredTile.x = cellX;
    exploredTile.y = cellY;
    exploredMapTiles.push_back(exploredTile);
}

void Worldstate::setMapExplored()
{
    for (const auto &mapTile : mapTiles)
    {
        const MWWorld::CellStore *cellStore = MWBase::Environment::get().getWorld()->getExterior(mapTile.x, mapTile.y);

        if (!cellStore->getCell()->mName.empty())
            MWBase::Environment::get().getWindowManager()->addVisitedLocation(cellStore->getCell()->mName, mapTile.x, mapTile.y);

        MWBase::Environment::get().getWindowManager()->setGlobalMapImage(mapTile.x, mapTile.y, mapTile.imageData);

        // Keep this tile marked as explored so we don't send any more packets for it
        markExploredMapTile(mapTile.x, mapTile.y);
    }
}

void Worldstate::setWeather()
{
    MWBase::World *world = MWBase::Environment::get().getWorld();

    // There's a chance we've been sent the weather for a region right after a teleportation
    // that hasn't been registered in the WeatherManager yet, meaning the WeatherManager
    // doesn't have the correct new region set for us, so make sure we update it
    world->updateWeather(0);

    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Setting weather for region: %s, currentWeather: %i, "
        "nextWeather: %i, queuedWeather: %i, transitionFactor: %f, forceWeather is %s",
        weather.region.c_str(), weather.currentWeather, weather.nextWeather,
        weather.queuedWeather, weather.transitionFactor, forceWeather ? "true" : "false");

    world->setRegionWeather(weather.region.c_str(), weather.currentWeather, weather.nextWeather,
        weather.queuedWeather, weather.transitionFactor, forceWeather);
}

void Worldstate::sendMapExplored(int cellX, int cellY, const std::vector<char>& imageData)
{
    mapTiles.clear();

    mwmp::MapTile mapTile;
    mapTile.x = cellX;
    mapTile.y = cellY;
    mapTile.imageData = imageData;

    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_PLAYER_MAP with x: %i, y: %i", cellX, cellY);

    mapTiles.push_back(mapTile);

    getNetworking()->getWorldstatePacket(ID_WORLD_MAP)->setWorldstate(this);
    getNetworking()->getWorldstatePacket(ID_WORLD_MAP)->Send();
}

void Worldstate::sendWeather(std::string region, int currentWeather, int nextWeather, int queuedWeather, float transitionFactor)
{
    forceWeather = false;
    weather.region = region;
    weather.currentWeather = currentWeather;
    weather.nextWeather = nextWeather;
    weather.queuedWeather = queuedWeather;
    weather.transitionFactor = transitionFactor;

    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_PLAYER_WEATHER with region: %s, currentWeather: %i, "
        "nextWeather: %i, queuedWeather, %i, transitionFactor: %f",
        region.c_str(), currentWeather, nextWeather, queuedWeather, transitionFactor);

    getNetworking()->getWorldstatePacket(ID_WORLD_WEATHER)->setWorldstate(this);
    getNetworking()->getWorldstatePacket(ID_WORLD_WEATHER)->Send();
}

void Worldstate::sendEnchantmentRecord(const ESM::Enchantment* enchantment)
{
    enchantmentRecords.clear();

    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_RECORD_DYNAMIC with enchantment");

    recordsType = mwmp::RECORD_TYPE::ENCHANTMENT;

    mwmp::EnchantmentRecord record;
    record.data = *enchantment;
    enchantmentRecords.push_back(record);

    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->setWorldstate(this);
    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->Send();
}

void Worldstate::sendPotionRecord(const ESM::Potion* potion)
{
    potionRecords.clear();

    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_RECORD_DYNAMIC with potion %s", potion->mName.c_str());

    recordsType = mwmp::RECORD_TYPE::POTION;

    mwmp::PotionRecord record;
    record.data = *potion;
    potionRecords.push_back(record);

    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->setWorldstate(this);
    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->Send();
}

void Worldstate::sendSpellRecord(const ESM::Spell* spell)
{
    spellRecords.clear();

    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_RECORD_DYNAMIC with spell %s", spell->mName.c_str());

    recordsType = mwmp::RECORD_TYPE::SPELL;

    mwmp::SpellRecord record;
    record.data = *spell;
    spellRecords.push_back(record);

    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->setWorldstate(this);
    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->Send();
}

void Worldstate::sendArmorRecord(const ESM::Armor* armor, std::string baseId)
{
    armorRecords.clear();

    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_RECORD_DYNAMIC with armor %s", armor->mName.c_str());

    recordsType = mwmp::RECORD_TYPE::ARMOR;

    mwmp::ArmorRecord record;
    record.data = *armor;
    record.baseId = baseId;
    record.baseOverrides.hasName = true;
    record.baseOverrides.hasEnchantmentId = true;
    record.baseOverrides.hasEnchantmentCharge = true;
    armorRecords.push_back(record);

    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->setWorldstate(this);
    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->Send();
}

void Worldstate::sendBookRecord(const ESM::Book* book, std::string baseId)
{
    bookRecords.clear();

    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_RECORD_DYNAMIC with book %s", book->mName.c_str());

    recordsType = mwmp::RECORD_TYPE::BOOK;

    mwmp::BookRecord record;
    record.data = *book;
    record.baseId = baseId;
    record.baseOverrides.hasName = true;
    record.baseOverrides.hasEnchantmentId = true;
    record.baseOverrides.hasEnchantmentCharge = true;
    bookRecords.push_back(record);

    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->setWorldstate(this);
    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->Send();
}

void Worldstate::sendClothingRecord(const ESM::Clothing* clothing, std::string baseId)
{
    clothingRecords.clear();

    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_RECORD_DYNAMIC with clothing %s", clothing->mName.c_str());

    recordsType = mwmp::RECORD_TYPE::CLOTHING;

    mwmp::ClothingRecord record;
    record.data = *clothing;
    record.baseId = baseId;
    record.baseOverrides.hasName = true;
    record.baseOverrides.hasEnchantmentId = true;
    record.baseOverrides.hasEnchantmentCharge = true;
    clothingRecords.push_back(record);

    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->setWorldstate(this);
    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->Send();
}

void Worldstate::sendWeaponRecord(const ESM::Weapon* weapon, std::string baseId)
{
    weaponRecords.clear();

    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_RECORD_DYNAMIC with weapon %s", weapon->mName.c_str());

    recordsType = mwmp::RECORD_TYPE::WEAPON;

    mwmp::WeaponRecord record;
    record.data = *weapon;
    record.baseId = baseId;
    record.baseOverrides.hasName = true;
    record.baseOverrides.hasEnchantmentId = true;
    record.baseOverrides.hasEnchantmentCharge = true;
    weaponRecords.push_back(record);

    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->setWorldstate(this);
    getNetworking()->getWorldstatePacket(ID_RECORD_DYNAMIC)->Send();
}
