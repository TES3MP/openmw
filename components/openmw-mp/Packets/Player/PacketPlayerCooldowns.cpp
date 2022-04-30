#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketPlayerCooldowns.hpp"

using namespace mwmp;

PacketPlayerCooldowns::PacketPlayerCooldowns(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_COOLDOWNS;
}

void PacketPlayerCooldowns::Packet(RakNet::BitStream *newBitstream, bool send)
{
    PlayerPacket::Packet(newBitstream, send);

    uint32_t count;

    if (send)
        count = static_cast<uint32_t>(player->cooldownChanges.size());

    RW(count, send);

    if (!send)
    {
        // Sanity check
        const static int minEntrySize = 1 + sizeof(SpellCooldown::startTimestampDay) + sizeof(SpellCooldown::startTimestampHour); /* First value: there is no fixed length,
            but the compressed header (1b+4b) is 100% bigger than 1 byte. I'm just lazy to do maths.*/
        int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 8);
        if (count > estimatedMax)
        {
            LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketPlayerCooldowns] Too big cooldown changes count: %d (est. max: %d)", count, estimatedMax);
            packetValid = false;
            return;
        }
        player->cooldownChanges.clear();
        player->cooldownChanges.resize(count);
    }

    for (auto &&spell : player->cooldownChanges)
    {
        RW(spell.id, send, true);
        RW(spell.startTimestampDay, send);
        RW(spell.startTimestampHour, send);
    }
}
