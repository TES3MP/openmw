#ifndef GAME_MWMECHANICS_NPCSTATS_H
#define GAME_MWMECHANICS_NPCSTATS_H

#include "creaturestats.hpp"
#include <components/esm/refid.hpp>
#include <components/esm3/loadskil.hpp>
#include <map>
#include <set>
#include <string>
#include <vector>

<<<<<<< HEAD
/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include <time.h>
/*
    End of tes3mp addition
*/

#include "creaturestats.hpp"

=======
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
namespace ESM
{
    struct Class;
    struct NpcStats;
}

namespace MWMechanics
{
    /// \brief Additional stats for NPCs

    class NpcStats : public CreatureStats
    {
        int mDisposition;
        std::map<ESM::RefId, SkillValue> mSkills; // SkillValue.mProgress used by the player only

        int mReputation;
        int mCrimeId;

<<<<<<< HEAD
            /*
                Start of tes3mp addition

                Add a variable used to track the time of the most recent crime by a player
            */
            time_t mCrimeTime = time(0);
            /*
                End of tes3mp addition
            */

            // ----- used by the player only, maybe should be moved at some point -------
            int mBounty;
            int mWerewolfKills;
            /// Used only for the player and for NPC's with ranks, modified by scripts; other NPCs have maximum one faction defined in their NPC record
            std::map<std::string, int> mFactionRank;
            std::set<std::string> mExpelled;
            std::map<std::string, int> mFactionReputation;
            int mLevelProgress; // 0-10
            std::vector<int> mSkillIncreases; // number of skill increases for each attribute (resets after leveling up)
            std::vector<int> mSpecIncreases; // number of skill increases for each specialization (accumulates throughout the entire game)
            std::set<std::string> mUsedIds;
            // ---------------------------------------------------------------------------
=======
        // ----- used by the player only, maybe should be moved at some point -------
        int mBounty;
        int mWerewolfKills;
        /// Used only for the player and for NPC's with ranks, modified by scripts; other NPCs have maximum one faction
        /// defined in their NPC record
        std::map<ESM::RefId, int> mFactionRank;
        std::set<ESM::RefId> mExpelled;
        std::map<ESM::RefId, int> mFactionReputation;
        int mLevelProgress; // 0-10
        std::map<ESM::Attribute::AttributeID, int>
            mSkillIncreases; // number of skill increases for each attribute (resets after leveling up)
        std::vector<int> mSpecIncreases; // number of skill increases for each specialization (accumulates throughout
                                         // the entire game)
        std::set<ESM::RefId> mUsedIds;
        // ---------------------------------------------------------------------------
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        /// Countdown to getting damage while underwater
        float mTimeToStartDrowning;

        bool mIsWerewolf;

    public:
        NpcStats();

        int getBaseDisposition() const;
        void setBaseDisposition(int disposition);

        int getReputation() const;
        void setReputation(int reputation);

        int getCrimeId() const;
        void setCrimeId(int id);

        const SkillValue& getSkill(ESM::RefId id) const;
        SkillValue& getSkill(ESM::RefId id);
        void setSkill(ESM::RefId id, const SkillValue& value);

        int getFactionRank(const ESM::RefId& faction) const;
        const std::map<ESM::RefId, int>& getFactionRanks() const;

        /// Join this faction, setting the initial rank to 0.
        void joinFaction(const ESM::RefId& faction);
        /// Sets the rank in this faction to a specified value, if such a rank exists.
        void setFactionRank(const ESM::RefId& faction, int value);

        const std::set<ESM::RefId>& getExpelled() const { return mExpelled; }
        bool getExpelled(const ESM::RefId& factionID) const;
        void expell(const ESM::RefId& factionID);
        void clearExpelled(const ESM::RefId& factionID);

        bool isInFaction(const ESM::RefId& faction) const;

        float getSkillProgressRequirement(ESM::RefId id, const ESM::Class& class_) const;

        void useSkill(ESM::RefId id, const ESM::Class& class_, int usageType = -1, float extraFactor = 1.f);
        ///< Increase skill by usage.

        void increaseSkill(ESM::RefId id, const ESM::Class& class_, bool preserveProgress, bool readBook = false);

        int getLevelProgress() const;

        int getLevelupAttributeMultiplier(ESM::Attribute::AttributeID attribute) const;

<<<<<<< HEAD
            /*
                Start of tes3mp addition

                Useful methods for setting player stats
            */
            void setLevelProgress(int value);
            int getSkillIncrease(int attribute) const;
            void setSkillIncrease(int attribute, int value);
            /*
                End of tes3mp addition
            */

            /*
                Start of tes3mp addition

                Make it possible to get and set the time of the last crime witnessed by the NPC,
                used to stop combat with a player after that player dies and is resurrected
            */
            time_t getCrimeTime();
            void setCrimeTime(time_t crimeTime);
            /*
                End of tes3mp addition
            */

            int getLevelupAttributeMultiplier(int attribute) const;
=======
        int getSkillIncreasesForSpecialization(int spec) const;
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        void levelUp();

        void updateHealth();
        ///< Calculate health based on endurance and strength.
        ///  Called at character creation.

        void flagAsUsed(const ESM::RefId& id);
        ///< @note Id must be lower-case

        bool hasBeenUsed(const ESM::RefId& id) const;
        ///< @note Id must be lower-case

        int getBounty() const;

        void setBounty(int bounty);

        int getFactionReputation(const ESM::RefId& faction) const;

        void setFactionReputation(const ESM::RefId& faction, int value);

        bool hasSkillsForRank(const ESM::RefId& factionId, int rank) const;

        bool isWerewolf() const;

        void setWerewolf(bool set);

        int getWerewolfKills() const;

        /// Increments mWerewolfKills by 1.
        void addWerewolfKill();

        float getTimeToStartDrowning() const;
        /// Sets time left for the creature to drown if it stays underwater.
        /// @param time value from [0,20]
        void setTimeToStartDrowning(float time);

        void writeState(ESM::CreatureStats& state) const;
        void writeState(ESM::NpcStats& state) const;

        void readState(const ESM::CreatureStats& state);
        void readState(const ESM::NpcStats& state);

        const std::map<ESM::RefId, SkillValue>& getSkills() const { return mSkills; }
    };
}

#endif
