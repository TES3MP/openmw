#ifndef GAME_MWMECHANICS_ACTIVESPELLS_H
#define GAME_MWMECHANICS_ACTIVESPELLS_H

#include <functional>
#include <list>
#include <queue>
#include <string>
#include <variant>
#include <vector>

#include <components/esm3/activespells.hpp>

#include "../mwworld/ptr.hpp"
#include "../mwworld/timestamp.hpp"

#include "spellcasting.hpp"

namespace ESM
{
    struct Enchantment;
    struct Spell;
}

namespace MWMechanics
{
    /// \brief Lasting spell effects
    ///
    /// \note The name of this class is slightly misleading, since it also handles lasting potion
    /// effects.
    class ActiveSpells
    {
    public:
        using ActiveEffect = ESM::ActiveEffect;
        class ActiveSpellParams
        {
            ESM::RefId mId;
            std::vector<ActiveEffect> mEffects;
            std::string mDisplayName;
            int mCasterActorId;
            int mSlot;
            ESM::ActiveSpells::EffectType mType;
            int mWorsenings;
            MWWorld::TimeStamp mNextWorsening;

            ActiveSpellParams(const ESM::ActiveSpells::ActiveSpellParams& params);

            ActiveSpellParams(const ESM::Spell* spell, const MWWorld::Ptr& actor, bool ignoreResistances = false);

            ActiveSpellParams(const MWWorld::ConstPtr& item, const ESM::Enchantment* enchantment, int slotIndex,
                const MWWorld::Ptr& actor);

            ActiveSpellParams(const ActiveSpellParams& params, const MWWorld::Ptr& actor);

            ESM::ActiveSpells::ActiveSpellParams toEsm() const;

<<<<<<< HEAD
            TIterator begin() const;

            TIterator end() const;

            void update(float duration) const;

        private:

            mutable TContainer mSpells;
            mutable MagicEffects mEffects;
            mutable bool mSpellsChanged;

            /*
                Start of tes3mp addition

                Track the actorId corresponding to these ActiveSpells
            */
            int mActorId;
            /*
                End of tes3mp addition
            */

            void rebuildEffects() const;

            /// Add any effects that are in "from" and not in "addTo" to "addTo"
            void mergeEffects(std::vector<ActiveEffect>& addTo, const std::vector<ActiveEffect>& from);

            double timeToExpire (const TIterator& iterator) const;
            ///< Returns time (in in-game hours) until the spell pointed to by \a iterator
            /// expires.

            const TContainer& getActiveSpells() const;
=======
            friend class ActiveSpells;
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        public:
            ActiveSpellParams(const CastSpell& cast, const MWWorld::Ptr& caster);

            const ESM::RefId& getId() const { return mId; }

            const std::vector<ActiveEffect>& getEffects() const { return mEffects; }
            std::vector<ActiveEffect>& getEffects() { return mEffects; }

<<<<<<< HEAD
            /*
                Start of tes3mp addition

                Add a separate addSpell() with a timestamp argument
            */
            void addSpell (const std::string& id, bool stack, std::vector<ActiveEffect> effects,
                           const std::string& displayName, int casterActorId, MWWorld::TimeStamp timestamp, bool sendPacket = true);
            /*
                End of tes3mp addition
            */

            /// Removes the active effects from this spell/potion/.. with \a id
            void removeEffects (const std::string& id);

            /*
                Start of tes3mp addition

                Remove the spell with a certain ID and a certain timestamp, useful
                when there are stacked spells with the same ID
            */
            bool removeSpellByTimestamp(const std::string& id, MWWorld::TimeStamp timestamp);
            /*
                End of tes3mp addition
            */

            /// Remove all active effects with this effect id
            void purgeEffect (short effectId);
=======
            ESM::ActiveSpells::EffectType getType() const { return mType; }

            int getCasterActorId() const { return mCasterActorId; }
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

            int getWorsenings() const { return mWorsenings; }

            const std::string& getDisplayName() const { return mDisplayName; }

            // Increments worsenings count and sets the next timestamp
            void worsen();

<<<<<<< HEAD
            /*
                Start of tes3mp addition

                Allow the purging of an effect for a specific arg (attribute or skill)
            */
            void purgeEffectByArg(short effectId, int effectArg);
            /*
                End of tes3mp addition
            */

            /*
                Start of tes3mp addition

                Make it easy to get an effect's duration
            */
            float getEffectDuration(short effectId, std::string sourceId);
            /*
                End of tes3mp addition
            */

            /// Remove all spells
            void clear();
=======
            bool shouldWorsen() const;
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

            void resetWorsenings();
        };

        typedef std::list<ActiveSpellParams> Collection;
        typedef Collection::const_iterator TIterator;

        void readState(const ESM::ActiveSpells& state);
        void writeState(ESM::ActiveSpells& state) const;

        TIterator begin() const;

<<<<<<< HEAD
            /*
                Start of tes3mp addition

                Make it possible to set and get the actorId for these ActiveSpells
            */
            int getActorId() const;
            void setActorId(int actorId);
            /*
                End of tes3mp addition
            */

=======
        TIterator end() const;

        void update(const MWWorld::Ptr& ptr, float duration);

    private:
        using ParamsPredicate = std::function<bool(const ActiveSpellParams&)>;
        using EffectPredicate = std::function<bool(const ActiveSpellParams&, const ESM::ActiveEffect&)>;
        using Predicate = std::variant<ParamsPredicate, EffectPredicate>;

        struct IterationGuard
        {
            ActiveSpells& mActiveSpells;

            IterationGuard(ActiveSpells& spells);
            ~IterationGuard();
        };

        std::list<ActiveSpellParams> mSpells;
        std::vector<ActiveSpellParams> mQueue;
        std::queue<Predicate> mPurges;
        bool mIterating;

        void addToSpells(const MWWorld::Ptr& ptr, const ActiveSpellParams& spell);

        bool applyPurges(const MWWorld::Ptr& ptr, std::list<ActiveSpellParams>::iterator* currentSpell = nullptr,
            std::vector<ActiveEffect>::iterator* currentEffect = nullptr);

    public:
        ActiveSpells();

        /// Add lasting effects
        ///
        /// \brief addSpell
        /// \param id ID for stacking purposes.
        ///
        void addSpell(const ActiveSpellParams& params);

        /// Bypasses resistances
        void addSpell(const ESM::Spell* spell, const MWWorld::Ptr& actor);

        /// Removes the active effects from this spell/potion/.. with \a id
        void removeEffects(const MWWorld::Ptr& ptr, const ESM::RefId& id);

        /// Remove all active effects with this effect id
        void purgeEffect(const MWWorld::Ptr& ptr, int effectId, int effectArg = -1);

        void purge(EffectPredicate predicate, const MWWorld::Ptr& ptr);
        void purge(ParamsPredicate predicate, const MWWorld::Ptr& ptr);

        /// Remove all effects that were cast by \a casterActorId
        void purge(const MWWorld::Ptr& ptr, int casterActorId);

        /// Remove all spells
        void clear(const MWWorld::Ptr& ptr);

        bool isSpellActive(const ESM::RefId& id) const;
        ///< case insensitive

        void skipWorsenings(double hours);

        void unloadActor(const MWWorld::Ptr& ptr);
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
    };
}

#endif
