#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketPlayerInventory.hpp"

using namespace mwmp;

PacketPlayerInventory::PacketPlayerInventory(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_INVENTORY;
}

void PacketPlayerInventory::Packet(RakNet::BitStream *newBitstream, bool send)
{
    PlayerPacket::Packet(newBitstream, send);

    RW(player->inventoryChanges.action, send);

    uint32_t count;

    if (send)
        count = static_cast<uint32_t>(player->inventoryChanges.items.size());

    RW(count, send);

    if (!send)
    {
        // Sanity check
        /* 1st and 5th values: there is no fixed length, but the compressed header (1b+4b) is 100% bigger than 1 byte. I'm just lazy to do maths.*/
        const static int minEntrySize = 1 + sizeof(Item::count) + sizeof(Item::charge) + sizeof(Item:: enchantmentCharge) + 1;
        int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 8);
        if (count > estimatedMax)
        {
            LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketPlayerInventory] Too big inventory changes count: %d (est. max: %d)", count, estimatedMax);
            packetValid = false;
            return;
        }
        player->inventoryChanges.items.clear();
        player->inventoryChanges.items.resize(count);
    }

    for (auto &&item : player->inventoryChanges.items)
    {
        RW(item.refId, send, true);
        RW(item.count, send);
        RW(item.charge, send);
        RW(item.enchantmentCharge, send);
        RW(item.soul, send, true);
    }
}
