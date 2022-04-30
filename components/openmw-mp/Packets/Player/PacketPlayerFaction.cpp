#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketPlayerFaction.hpp"

using namespace mwmp;

PacketPlayerFaction::PacketPlayerFaction(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_FACTION;
}

void PacketPlayerFaction::Packet(RakNet::BitStream *newBitstream, bool send)
{
    PlayerPacket::Packet(newBitstream, send);

    RW(player->factionChanges.action, send);

    uint32_t count;

    if (send)
        count = static_cast<uint32_t>(player->factionChanges.factions.size());

    RW(count, send);

    if (!send)
    {
        // Sanity check
        const static int minEntrySize = 1; /* there is no fixed length,
            but the compressed header (1b+4b) is 100% bigger than 1 byte. I'm just lazy to do maths.*/
        int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 9); // Additional bit: isExpelled
        if (count > estimatedMax)
        {
            LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketPlayerFaction] Too big faction changes count: %d (est. max: %d)", count, estimatedMax);
            packetValid = false;
            return;
        }
        player->factionChanges.factions.clear();
        player->factionChanges.factions.resize(count);
    }

    for (auto &&faction : player->factionChanges.factions)
    {
        RW(faction.factionId, send, true);

        if (player->factionChanges.action == FactionChanges::RANK)
            RW(faction.rank, send);

        if (player->factionChanges.action == FactionChanges::EXPULSION)
            RW(faction.isExpelled, send);

        if (player->factionChanges.action == FactionChanges::REPUTATION)
            RW(faction.reputation, send);
    }
}
