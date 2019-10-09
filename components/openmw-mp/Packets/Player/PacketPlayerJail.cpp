#include "PacketPlayerJail.hpp"
#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/MWMPLog.hpp>

using namespace mwmp;

PacketPlayerJail::PacketPlayerJail(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_JAIL;
}

void PacketPlayerJail::Packet(RakNet::BitStream *bs, bool send)
{
    PlayerPacket::Packet(bs, send);

    RW(player->jailDays, send);
    RW(player->ignoreJailTeleportation, send);
    RW(player->ignoreJailSkillIncreases, send);
    RW(player->jailProgressText, send, true);
    RW(player->jailEndText, send, true);
}
