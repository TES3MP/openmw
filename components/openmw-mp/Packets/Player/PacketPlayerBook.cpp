#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketPlayerBook.hpp"

using namespace mwmp;

PacketPlayerBook::PacketPlayerBook(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_BOOK;
}

void PacketPlayerBook::Packet(RakNet::BitStream *newBitstream, bool send)
{
    PlayerPacket::Packet(newBitstream, send);

    uint32_t count;

    if (send)
        count = static_cast<uint32_t>(player->bookChanges.size());

    RW(count, send);

    if (!send)
    {
        // Sanity check
        const static int minEntrySize = 1; // there is no fixed length, but the compressed header (1b+4b) is 100% bigger than 1 byte. i'm just lazy to do maths.
        int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 8);
        if (count > estimatedMax)
        {
            LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketPlayerBook] Too big book changes count: %d (est. max: %d)", count, estimatedMax);
            packetValid = false;
            return;
        }
        player->bookChanges.clear();
        player->bookChanges.resize(count);
    }

    for (auto &&book : player->bookChanges)
    {
        RW(book.bookId, send, true);
    }
}
