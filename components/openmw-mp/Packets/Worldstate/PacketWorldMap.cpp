#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/TimedLog.hpp>
#include "PacketWorldMap.hpp"

using namespace mwmp;

PacketWorldMap::PacketWorldMap(RakNet::RakPeerInterface *peer) : WorldstatePacket(peer)
{
    packetID = ID_WORLD_MAP;
}

void PacketWorldMap::Packet(RakNet::BitStream *newBitstream, bool send)
{
    WorldstatePacket::Packet(newBitstream, send);

    uint32_t changesCount;

    if (send)
        changesCount = static_cast<uint32_t>(worldstate->mapTiles.size());

    RW(changesCount, send);

    if (!send)
    {
        // Sanity check
        const static int minEntrySize = sizeof(MapTile::x) + sizeof(MapTile::y) + sizeof(uint32_t); // latest - image data size
        int estimatedMax = bs.GetNumberOfUnreadBits() / (minEntrySize * 8);
        if (changesCount > estimatedMax)
        {
            LOG_MESSAGE(TimedLog::LOG_ERROR, "[PacketWorldMap] Too big map changes count: %d (est. max: %d)", changesCount, estimatedMax);
            packetValid = false;
            return;
        }
        worldstate->mapTiles.clear();
        worldstate->mapTiles.resize(changesCount);
    }

    for (auto &&mapTile : worldstate->mapTiles)
    {
        RW(mapTile.x, send);
        RW(mapTile.y, send);

        uint32_t imageDataSize;

        if (send)
            imageDataSize = static_cast<uint32_t>(mapTile.imageData.size());

        RW(imageDataSize, send);

        if (imageDataSize > mwmp::maxImageDataSize)
        {
            LOG_MESSAGE_SIMPLE(TimedLog::LOG_ERROR, "Processed invalid ID_WORLD_MAP packet where tile %i, %i had an imageDataSize of %i",
                mapTile.x, mapTile.y, imageDataSize);
            LOG_APPEND(TimedLog::LOG_ERROR, "- The packet was ignored after that point");
            return;
        }

        if (!send)
        {
            mapTile.imageData.clear();
            mapTile.imageData.resize(imageDataSize);
        }

        for (auto &&imageChar : mapTile.imageData)
        {
            RW(imageChar, send);
        }
    }
}
