#include "PacketPlayerSpellsActive.hpp"
#include <components/openmw-mp/NetworkMessages.hpp>

using namespace mwmp;

PacketPlayerSpellsActive::PacketPlayerSpellsActive(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_SPELLS_ACTIVE;
}

void PacketPlayerSpellsActive::Packet(RakNet::BitStream *newBitstream, bool send)
{
    PlayerPacket::Packet(newBitstream, send);

    RW(player->spellsActiveChanges.action, send);

    uint32_t count;

    if (send)
        count = static_cast<uint32_t>(player->spellsActiveChanges.activeSpells.size());

    RW(count, send);

    if (!send)
    {
        // Sanity check
        /* 1st and 4th values: there is no fixed length, but the compressed header (1b+4b) is 100% bigger than 1 byte. i'm just lazy to do maths.*/
        const static int minEntrySize = 1 + sizeof(ActiveSpell::timestampDay) + sizeof(ActiveSpell::timestampHour) + 1 + sizeof(uint64_t) + sizeof(uint32_t);
        // 5th - size of g (RakNet::GUID)
        int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 10); // 2 additional bits: isStackingSpell, isPlayer
        if (count > estimatedMax)
        {
            LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketPlayerSpellsActive] Too big active spells changes count: %d (est. max: %d)", count, estimatedMax);
            packetValid = false;
            return;
        }
        player->spellsActiveChanges.activeSpells.clear();
        player->spellsActiveChanges.activeSpells.resize(count);
    }

    for (auto&& activeSpell : player->spellsActiveChanges.activeSpells)
    {
        RW(activeSpell.id, send, true);
        RW(activeSpell.isStackingSpell, send);
        RW(activeSpell.timestampDay, send);
        RW(activeSpell.timestampHour, send);
        RW(activeSpell.params.mDisplayName, send, true);

        RW(activeSpell.caster.isPlayer, send);

        if (activeSpell.caster.isPlayer)
        {
            RW(activeSpell.caster.guid, send);
        }
        else
        {
            RW(activeSpell.caster.refId, send, true);
            RW(activeSpell.caster.refNum, send);
            RW(activeSpell.caster.mpNum, send);
        }

        uint32_t effectCount;

        if (send)
            effectCount = static_cast<uint32_t>(activeSpell.params.mEffects.size());

        RW(effectCount, send);

        if (effectCount > maxEffects)
        {
            return;
        }

        if (!send)
        {
            activeSpell.params.mEffects.clear();
            activeSpell.params.mEffects.resize(effectCount);
        }

        for (auto&& effect : activeSpell.params.mEffects)
        {
            RW(effect.mEffectId, send);
            RW(effect.mArg, send);
            RW(effect.mMagnitude, send);
            RW(effect.mDuration, send);
            RW(effect.mTimeLeft, send);
        }
    }
}
