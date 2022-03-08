#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketPlayerJournal.hpp"

using namespace mwmp;

PacketPlayerJournal::PacketPlayerJournal(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_JOURNAL;
}

void PacketPlayerJournal::Packet(RakNet::BitStream *newBitstream, bool send)
{
    PlayerPacket::Packet(newBitstream, send);

    uint32_t count;

    if (send)
        count = static_cast<uint32_t>(player->journalChanges.size());

    RW(count, send);

    if (!send)
    {
        // Sanity check
        /* 2nd value: there is no fixed length, but the compressed header (1b+4b) is 100% bigger than 1 byte. I'm just lazy to do maths.*/
        const static int minEntrySize = sizeof(JournalItem::type) + 1 + sizeof(JournalItem::index);
        int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 8);
        if (count > estimatedMax)
        {
            LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketPlayerJournal] Too big journal changes count: %d (est. max: %d)", count, estimatedMax);
            packetValid = false;
            return;
        }
        player->journalChanges.clear();
        player->journalChanges.resize(count);
    }

    for (auto &&journalItem : player->journalChanges)
    {
        RW(journalItem.type, send);
        RW(journalItem.quest, send, true);
        RW(journalItem.index, send);

        if (journalItem.type == JournalItem::ENTRY)
        {
            RW(journalItem.actorRefId, send, true);

            RW(journalItem.hasTimestamp, send);

            if (journalItem.hasTimestamp)
            {
                RW(journalItem.timestamp.daysPassed, send);
                RW(journalItem.timestamp.month, send);
                RW(journalItem.timestamp.day, send);
            }
        }
    }
}
