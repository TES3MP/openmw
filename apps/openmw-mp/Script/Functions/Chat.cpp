#include "Chat.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/MWMPLog.hpp>

#include <apps/openmw-mp/Script/ScriptFunctions.hpp>
#include <apps/openmw-mp/Networking.hpp>

void ChatFunctions::SendMessage(unsigned short pid, const char *message, bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    Player *player;
    GET_PLAYER(pid, player,);

    player->chatMessage = message;

    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "System: %s", message);

    mwmp::PlayerPacket *packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_CHAT_MESSAGE);
    packet->setPlayer(player);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}

void ChatFunctions::CleanChatForPid(unsigned short pid)
{
    Player *player;
    GET_PLAYER(pid, player,);

    player->chatMessage.clear();

    mwmp::PlayerPacket *packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_CHAT_MESSAGE);
    packet->setPlayer(player);

    packet->Send(false);
}

void ChatFunctions::CleanChat()
{
    for (auto player : *Players::getPlayers())
    {
        player.second->chatMessage.clear();

        mwmp::PlayerPacket *packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_CHAT_MESSAGE);
        packet->setPlayer(player.second);

        packet->Send(false);
    }
}
