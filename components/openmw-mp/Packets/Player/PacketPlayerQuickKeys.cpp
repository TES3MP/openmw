#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketPlayerQuickKeys.hpp"

using namespace mwmp;

PacketPlayerQuickKeys::PacketPlayerQuickKeys(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_QUICKKEYS;
}

void PacketPlayerQuickKeys::Packet(RakNet::BitStream *newBitstream, bool send)
{
    PlayerPacket::Packet(newBitstream, send);

    uint32_t count;

    if (send)
        count = static_cast<uint32_t>(player->quickKeyChanges.size());

    RW(count, send);

    if (!send)
    {
        // Sanity check
        const static int minEntrySize = sizeof(QuickKey::type) + sizeof(QuickKey::slot);
        int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 8);
        if (count > estimatedMax)
        {
            LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketPlayerQuickKeys] Too big quick key changes count: %d (est. max: %d)", count, estimatedMax);
            packetValid = false;
            return;
        }
        player->quickKeyChanges.clear();
        player->quickKeyChanges.resize(count);
    }

    for (auto &&quickKey : player->quickKeyChanges)
    {
        RW(quickKey.type, send);
        RW(quickKey.slot, send);

        if (quickKey.type != QuickKey::UNASSIGNED)
            RW(quickKey.itemId, send);
    }
}
