#include "PacketWorldKillCount.hpp"
#include <components/openmw-mp/NetworkMessages.hpp>

using namespace mwmp;

PacketWorldKillCount::PacketWorldKillCount(RakNet::RakPeerInterface *peer) : WorldstatePacket(peer)
{
    packetID = ID_WORLD_KILL_COUNT;
    orderChannel = CHANNEL_SYSTEM;
}

void PacketWorldKillCount::Packet(RakNet::BitStream *newBitstream, bool send)
{
    WorldstatePacket::Packet(newBitstream, send);

    uint32_t killChangesCount;

    if (send)
        killChangesCount = static_cast<uint32_t>(worldstate->killChanges.size());

    RW(killChangesCount, send);

    if (!send)
    {
        // Sanity check
        const static int minEntrySize = 1 + sizeof(Kill::number); /* 1st value: there is no fixed length,
            but the compressed header (1b+4b) is 100% bigger than 1 byte. i'm just lazy to do maths.*/
        int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 8);
        if (killChangesCount > estimatedMax)
        {
            LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketWorldKillCount] Too big kill changes count: %d (est. max: %d)", killChangesCount, estimatedMax);
            packetValid = false;
            return;
        }
        worldstate->killChanges.clear();
        worldstate->killChanges.resize(killChangesCount);
    }

    for (auto &&killChange : worldstate->killChanges)
    {
        RW(killChange.refId, send, true);
        RW(killChange.number, send);
    }
}
