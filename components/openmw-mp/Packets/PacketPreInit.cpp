#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/MWMPLog.hpp>
#include "PacketPreInit.hpp"

mwmp::PacketPreInit::PacketPreInit(RakNet::RakPeerInterface *peer) : BasePacket(peer)
{
    packetID = ID_GAME_PREINIT;
}

void mwmp::PacketPreInit::Packet(RakNet::BitStream *bs, bool send)
{
    BasePacket::Packet(bs, send);

    const RakNet::BitSize_t packetSize = bs->GetNumberOfBytesUsed();
    uint32_t expectedPacketSize = BasePacket::headerSize() + sizeof(uint32_t);
    if (!send && expectedPacketSize > packetSize)
    {
        LOG_MESSAGE(MWMPLog::LOG_ERROR, "Wrong packet size %d when expected %d", packetSize, expectedPacketSize);
        packetValid = false;
        return;
    }

    uint32_t numberOfChecksums = checksums->size();
    RW(numberOfChecksums, send);

    if (numberOfChecksums > maxPlugins)
    {
        LOG_MESSAGE(MWMPLog::LOG_ERROR, "Wrong number of checksums %d when maximum is %d", numberOfChecksums, maxPlugins);
        packetValid = false;
        return;
    }

    struct NAS
    {
        uint32_t hashN;
        uint32_t strSize;
    };

    std::vector<NAS> NumberOfHashesAndStrSizes(numberOfChecksums);

    PluginContainer::const_iterator checksumIt = checksums->begin();

    for (auto &&nas : NumberOfHashesAndStrSizes)
    {
        if (send)
        {
            nas.strSize = checksumIt->first.size();
            nas.hashN = checksumIt++->second.size();
        }
        RW(nas, send);

        expectedPacketSize += nas.strSize + nas.hashN;

        if (nas.strSize > pluginNameMaxLength)
            LOG_MESSAGE(MWMPLog::LOG_ERROR, "Wrong string length %d when maximum length is %d",
                        nas.strSize,
                        pluginNameMaxLength);
        else if (nas.hashN > maxHashes)
            LOG_MESSAGE(MWMPLog::LOG_ERROR, "Wrong  number of hashes %d when maximum is %d", nas.hashN, maxHashes);
        else
            continue;
        packetValid = false;
        return;
    }

    if (!send && expectedPacketSize == packetSize) // server accepted plugin list via sending "empty" packet
        return;

    if (!send && expectedPacketSize > packetSize)
    {
        LOG_MESSAGE(MWMPLog::LOG_ERROR, "Wrong packet size %d when expected %d", packetSize, expectedPacketSize);
        packetValid = false;
        return;
    }

    checksums->resize(numberOfChecksums);

    auto numberOfHashesIt = NumberOfHashesAndStrSizes.cbegin();

    for (auto &&checksum : *checksums)
    {
        RW(checksum.first, send, false, numberOfHashesIt->strSize);

        checksum.second.resize(numberOfHashesIt->hashN);
        for (auto &&hash : checksum.second)
            RW(hash, send);
        ++numberOfHashesIt;
    }
}

void mwmp::PacketPreInit::setChecksums(mwmp::PacketPreInit::PluginContainer *checksums)
{
    this->checksums = checksums;
}
