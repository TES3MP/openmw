#include "summoning.hpp"

#include <components/debug/debuglog.hpp>
#include <components/esm/refid.hpp>
#include <components/esm3/loadmgef.hpp>
#include <components/esm3/loadstat.hpp>
#include <components/misc/resourcehelpers.hpp>

/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include <components/openmw-mp/TimedLog.hpp>
#include "../mwmechanics/creaturestats.hpp"
#include "../mwmp/Main.hpp"
#include "../mwmp/Networking.hpp"
#include "../mwmp/CellController.hpp"
#include "../mwmp/ObjectList.hpp"
/*
    End of tes3mp addition
*/

#include "../mwbase/environment.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/world.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwworld/manualref.hpp"

#include "../mwrender/animation.hpp"

#include "aifollow.hpp"
#include "creaturestats.hpp"

namespace MWMechanics
{

    bool isSummoningEffect(int effectId)
    {
        return ((effectId >= ESM::MagicEffect::SummonScamp && effectId <= ESM::MagicEffect::SummonStormAtronach)
            || (effectId == ESM::MagicEffect::SummonCenturionSphere)
            || (effectId >= ESM::MagicEffect::SummonFabricant && effectId <= ESM::MagicEffect::SummonCreature05));
    }

    static const std::map<int, ESM::RefId>& getSummonMap()
    {
        static std::map<int, ESM::RefId> summonMap;

        if (summonMap.size() > 0)
            return summonMap;

        const std::map<int, std::string_view> summonMapToGameSetting{
            { ESM::MagicEffect::SummonAncestralGhost, "sMagicAncestralGhostID" },
            { ESM::MagicEffect::SummonBonelord, "sMagicBonelordID" },
            { ESM::MagicEffect::SummonBonewalker, "sMagicLeastBonewalkerID" },
            { ESM::MagicEffect::SummonCenturionSphere, "sMagicCenturionSphereID" },
            { ESM::MagicEffect::SummonClannfear, "sMagicClannfearID" },
            { ESM::MagicEffect::SummonDaedroth, "sMagicDaedrothID" },
            { ESM::MagicEffect::SummonDremora, "sMagicDremoraID" },
            { ESM::MagicEffect::SummonFabricant, "sMagicFabricantID" },
            { ESM::MagicEffect::SummonFlameAtronach, "sMagicFlameAtronachID" },
            { ESM::MagicEffect::SummonFrostAtronach, "sMagicFrostAtronachID" },
            { ESM::MagicEffect::SummonGoldenSaint, "sMagicGoldenSaintID" },
            { ESM::MagicEffect::SummonGreaterBonewalker, "sMagicGreaterBonewalkerID" },
            { ESM::MagicEffect::SummonHunger, "sMagicHungerID" },
            { ESM::MagicEffect::SummonScamp, "sMagicScampID" },
            { ESM::MagicEffect::SummonSkeletalMinion, "sMagicSkeletalMinionID" },
            { ESM::MagicEffect::SummonStormAtronach, "sMagicStormAtronachID" },
            { ESM::MagicEffect::SummonWingedTwilight, "sMagicWingedTwilightID" },
            { ESM::MagicEffect::SummonWolf, "sMagicCreature01ID" },
            { ESM::MagicEffect::SummonBear, "sMagicCreature02ID" },
            { ESM::MagicEffect::SummonBonewolf, "sMagicCreature03ID" },
            { ESM::MagicEffect::SummonCreature04, "sMagicCreature04ID" },
            { ESM::MagicEffect::SummonCreature05, "sMagicCreature05ID" },
        };

        for (const auto& it : summonMapToGameSetting)
        {
            summonMap[it.first] = ESM::RefId::stringRefId(
                MWBase::Environment::get().getESMStore()->get<ESM::GameSetting>().find(it.second)->mValue.getString());
        }
        return summonMap;
    }

    ESM::RefId getSummonedCreature(int effectId)
    {
        const auto& summonMap = getSummonMap();
        auto it = summonMap.find(effectId);
        if (it != summonMap.end())
        {
            return it->second;
        }
        return ESM::RefId();
    }

    int summonCreature(int effectId, const MWWorld::Ptr& summoner)
    {
        const ESM::RefId& creatureID = getSummonedCreature(effectId);
        int creatureActorId = -1;
        if (!creatureID.empty())
        {
            try
            {
                auto world = MWBase::Environment::get().getWorld();
                MWWorld::ManualRef ref(world->getStore(), creatureID, 1);

                MWMechanics::CreatureStats& summonedCreatureStats
                    = ref.getPtr().getClass().getCreatureStats(ref.getPtr());

                // Make the summoned creature follow its master and help in fights
                AiFollow package(summoner);
                summonedCreatureStats.getAiSequence().stack(package, ref.getPtr());
                creatureActorId = summonedCreatureStats.getActorId();

                MWWorld::Ptr placed = world->safePlaceObject(ref.getPtr(), summoner, summoner.getCell(), 0, 120.f);

                MWRender::Animation* anim = world->getAnimation(placed);
                if (anim)
                {
<<<<<<< HEAD
                    int creatureActorId = -1;

                    /*
                        Start of tes3mp change (major)

                        Send an ID_OBJECT_SPAWN packet every time a creature is summoned in a cell that we hold
                        authority over, then delete the creature and wait for the server to send it back with a
                        unique mpNum of its own

                        Comment out most of the code here except for the actual placement of the Ptr and the
                        creatureActorId insertion into the creatureMap
                    */
                    try
                    {
                        MWWorld::ManualRef ref(MWBase::Environment::get().getWorld()->getStore(), creatureID, 1);

                        /*
                        MWMechanics::CreatureStats& summonedCreatureStats = ref.getPtr().getClass().getCreatureStats(ref.getPtr());

                        // Make the summoned creature follow its master and help in fights
                        AiFollow package(mActor);
                        summonedCreatureStats.getAiSequence().stack(package, ref.getPtr());
                        creatureActorId = summonedCreatureStats.getActorId();
                        */

                        MWWorld::Ptr placed = MWBase::Environment::get().getWorld()->safePlaceObject(ref.getPtr(), mActor, mActor.getCell(), 0, 120.f);

                        /*
                        MWRender::Animation* anim = MWBase::Environment::get().getWorld()->getAnimation(placed);
                        if (anim)
                        {
                            const ESM::Static* fx = MWBase::Environment::get().getWorld()->getStore().get<ESM::Static>()
                                    .search("VFX_Summon_Start");
                            if (fx)
                                anim->addEffect("meshes\\" + fx->mModel, -1, false);
                        }
                        */

                        if (mwmp::Main::get().getCellController()->hasLocalAuthority(*placed.getCell()->getCell()))
                        {
                            mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
                            objectList->reset();
                            objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;

                            MWMechanics::CreatureStats *actorCreatureStats = &mActor.getClass().getCreatureStats(mActor);
                            int effectId = it->mEffectId;
                            std::string sourceId = it->mSourceId;
                            float duration = actorCreatureStats->getActiveSpells().getEffectDuration(effectId, sourceId);
                            objectList->addObjectSpawn(placed, mActor, sourceId, effectId, duration);
                            objectList->sendObjectSpawn();
                        }

                        MWBase::Environment::get().getWorld()->deleteObject(placed);
                    }
                    catch (std::exception& e)
                    {
                        Log(Debug::Error) << "Failed to spawn summoned creature: " << e.what();
                        // still insert into creatureMap so we don't try to spawn again every frame, that would spam the warning log
                    }

                    creatureMap.emplace(*it, creatureActorId);
                    /*
                        End of tes3mp change (major)
                    */
=======
                    const ESM::Static* fx
                        = world->getStore().get<ESM::Static>().search(ESM::RefId::stringRefId("VFX_Summon_Start"));
                    if (fx)
                    {
                        const VFS::Manager* const vfs = MWBase::Environment::get().getResourceSystem()->getVFS();
                        anim->addEffect(Misc::ResourceHelpers::correctMeshPath(fx->mModel, vfs), -1, false);
                    }
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
                }
            }
            catch (std::exception& e)
            {
                Log(Debug::Error) << "Failed to spawn summoned creature: " << e.what();
                // still insert into creatureMap so we don't try to spawn again every frame, that would spam the warning
                // log
            }

            summoner.getClass().getCreatureStats(summoner).getSummonedCreatureMap().emplace(effectId, creatureActorId);
        }
        return creatureActorId;
    }

    void updateSummons(const MWWorld::Ptr& summoner, bool cleanup)
    {
        MWMechanics::CreatureStats& creatureStats = summoner.getClass().getCreatureStats(summoner);
        auto& creatureMap = creatureStats.getSummonedCreatureMap();

        std::vector<int> graveyard = creatureStats.getSummonedCreatureGraveyard();
        creatureStats.getSummonedCreatureGraveyard().clear();

        for (const int creature : graveyard)
            MWBase::Environment::get().getMechanicsManager()->cleanupSummonedCreature(summoner, creature);

        if (!cleanup)
            return;

        for (auto it = creatureMap.begin(); it != creatureMap.end();)
        {
            if (it->second == -1)
            {
                // Keep the spell effect active if we failed to spawn anything
                it++;
                continue;
            }
            MWWorld::Ptr ptr = MWBase::Environment::get().getWorld()->searchPtrViaActorId(it->second);
            if (!ptr.isEmpty() && ptr.getClass().getCreatureStats(ptr).isDead()
                && ptr.getClass().getCreatureStats(ptr).isDeathAnimationFinished())
            {
                // Purge the magic effect so a new creature can be summoned if desired
                auto summon = *it;
                creatureMap.erase(it++);
                purgeSummonEffect(summoner, summon);
            }
            else
                ++it;
        }
    }

    void purgeSummonEffect(const MWWorld::Ptr& summoner, const std::pair<int, int>& summon)
    {
        auto& creatureStats = summoner.getClass().getCreatureStats(summoner);
        creatureStats.getActiveSpells().purge(
            [summon](const auto& spell, const auto& effect) {
                return effect.mEffectId == summon.first && effect.mArg == summon.second;
            },
            summoner);

        MWBase::Environment::get().getMechanicsManager()->cleanupSummonedCreature(summoner, summon.second);
    }
}
