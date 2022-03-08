#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketPlayerTopic.hpp"

using namespace mwmp;

PacketPlayerTopic::PacketPlayerTopic(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_TOPIC;
}

void PacketPlayerTopic::Packet(RakNet::BitStream *newBitstream, bool send)
{
    PlayerPacket::Packet(newBitstream, send);

    uint32_t count;

    if (send)
        count = static_cast<uint32_t>(player->topicChanges.size());

    RW(count, send);

    if (!send)
    {
        // Sanity check
        const static int minEntrySize = 1; // there is no fixed length, but the compressed header (1b+4b) is 100% bigger than 1 byte. i'm just lazy to do maths.
        int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 8);
        if (count > estimatedMax)
        {
            LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketPlayerTopic] Too big topic changes count: %d (est. max: %d)", count, estimatedMax);
            packetValid = false;
            return;
        }
        player->topicChanges.clear();
        player->topicChanges.resize(count);
    }

    for (auto &&topic : player->topicChanges)
    {
        RW(topic.topicId, send, true);
    }
}
