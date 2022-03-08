#include "PacketPlayerEquipment.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>

using namespace mwmp;

PacketPlayerEquipment::PacketPlayerEquipment(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_EQUIPMENT;
}

void PacketPlayerEquipment::Packet(RakNet::BitStream *newBitstream, bool send)
{
    PlayerPacket::Packet(newBitstream, send);

    RW(player->exchangeFullInfo, send);

    if (player->exchangeFullInfo)
    {
        for (auto &&equipmentItem : player->equipmentItems)
        {
            ExchangeItemInformation(equipmentItem, send);
        }
    }
    else
    {
        uint32_t count;
        if (send)
            count = static_cast<uint32_t>(player->equipmentIndexChanges.size());

        RW(count, send);

        if (!send)
        {
            // Sanity check
            const static int minEntrySize = sizeof(int) + 1 + sizeof(Item::count) + sizeof(Item::charge) + sizeof(Item::enchantmentCharge);
            /* Second value: there is no fixed length,
                but the compressed header (1b+4b) is 100% bigger than 1 byte. I'm just lazy to do maths.*/
            int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 8);
            if (count > estimatedMax)
            {
                LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketPlayerEquipment] Too big equipment changes count: %d (est. max: %d)", count, estimatedMax);
                packetValid = false;
                return;
            }
            player->equipmentIndexChanges.clear();
            player->equipmentIndexChanges.resize(count);
        }

        for (auto &&equipmentIndex : player->equipmentIndexChanges)
        {
            RW(equipmentIndex, send);
            ExchangeItemInformation(player->equipmentItems[equipmentIndex], send);
        }
    }
}

void PacketPlayerEquipment::ExchangeItemInformation(Item &item, bool send)
{
    RW(item.refId, send, true);
    RW(item.count, send);
    RW(item.charge, send);
    RW(item.enchantmentCharge, send);
}

