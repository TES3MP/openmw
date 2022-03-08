#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketPlayerSpellbook.hpp"

using namespace mwmp;

PacketPlayerSpellbook::PacketPlayerSpellbook(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_SPELLBOOK;
}

void PacketPlayerSpellbook::Packet(RakNet::BitStream *newBitstream, bool send)
{
    PlayerPacket::Packet(newBitstream, send);

    RW(player->spellbookChanges.action, send);

    uint32_t count;

    if (send)
        count = static_cast<uint32_t>(player->spellbookChanges.spells.size());

    RW(count, send);

    if (!send)
    {
        // Sanity check
        const static int minEntrySize = 1; // there is no fixed length, but the compressed header (1b+4b) is 100% bigger than 1 byte. i'm just lazy to do maths.
        int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 8);
        if (count > estimatedMax)
        {
            LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketPlayerSpellbook] Too big spellbook changes count: %d (est. max: %d)", count, estimatedMax);
            packetValid = false;
            return;
        }
        player->spellbookChanges.spells.clear();
        player->spellbookChanges.spells.resize(count);
    }

    for (auto &&spell : player->spellbookChanges.spells)
    {
        RW(spell.mId, send, true);
    }
}
