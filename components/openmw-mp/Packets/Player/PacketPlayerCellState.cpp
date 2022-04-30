#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketPlayerCellState.hpp"


mwmp::PacketPlayerCellState::PacketPlayerCellState(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_CELL_STATE;
    priority = IMMEDIATE_PRIORITY;
    reliability = RELIABLE_ORDERED;
}

void mwmp::PacketPlayerCellState::Packet(RakNet::BitStream *newBitstream, bool send)
{
    PlayerPacket::Packet(newBitstream, send);

    uint32_t count;

    if (send)
        count = static_cast<uint32_t>(player->cellStateChanges.size());

    RW(count, send);

    if (!send)
    {
        // Sanity check
        const static int minEntrySize = sizeof(CellState::type) + 1 + 1; /* Second value: Three 4-byte integers 100% > 1b
            Third value: there is no fixed length, but the compressed header (1b+4b)
            is 100% bigger than 1 byte. I'm just lazy to do maths.*/
        int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 8);
        if (count > estimatedMax)
        {
            LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketPlayerCellState] Too big cell state changes count: %d (est. max: %d)", count, estimatedMax);
            packetValid = false;
            return;
        }
        player->cellStateChanges.clear();
        player->cellStateChanges.resize(count);
    }

    for (auto &&cellState : player->cellStateChanges)
    {
        RW(cellState.type, send);
        RW(cellState.cell.mData, send, true);
        RW(cellState.cell.mName, send, true);
    }
}
