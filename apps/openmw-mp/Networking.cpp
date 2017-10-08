//
// Created by koncord on 12.01.16.
//

#include "Players.hpp"
#include <Script/EventController.hpp>
#include "processors/ProcessorInitializer.hpp"
#include <RakPeer.h>
#include <Kbhit.h>

#include <components/misc/stringops.hpp>
#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/Log.hpp>
#include <components/openmw-mp/Version.hpp>
#include <components/openmw-mp/Packets/PacketPreInit.hpp>

#include <iostream>
//#include <Script/API/TimerAPI.hpp>
#include <chrono>
#include <thread>

#include "Networking.hpp"
#include "MasterClient.hpp"
#include "Cell.hpp"
#include "CellController.hpp"
#include "processors/PlayerProcessor.hpp"
#include "processors/ActorProcessor.hpp"
#include "processors/WorldProcessor.hpp"

using namespace mwmp;
using namespace std;

Networking *Networking::sThis = 0;

static int currentMpNum = 0;



Networking::Networking(RakNet::RakPeerInterface *peer) : mclient(nullptr)
{
    sThis = this;
    this->peer = peer;

    CellController::create();

    playerPacketController = new PlayerPacketController(peer);
    actorPacketController = new ActorPacketController(peer);
    worldPacketController = new WorldPacketController(peer);

    // Set send stream
    playerPacketController->SetStream(0, &bsOut);
    actorPacketController->SetStream(0, &bsOut);
    worldPacketController->SetStream(0, &bsOut);

    running = true;
    exitCode = 0;
    luaState.loadMods();

    serverPassword = TES3MP_DEFAULT_PASSW;

    ProcessorInitializer();
}

Networking::~Networking()
{
    luaState.getEventCtrl().Call<CoreEvent::ON_EXIT>(false);

    CellController::destroy();

    sThis = nullptr;
    delete playerPacketController;
    delete actorPacketController;
    delete worldPacketController;
}

void Networking::setServerPassword(std::string passw) noexcept
{
    serverPassword = passw.empty() ? TES3MP_DEFAULT_PASSW : passw;
}

bool Networking::isPassworded() const
{
    return serverPassword != TES3MP_DEFAULT_PASSW;
}

void Networking::processPlayerPacket(RakNet::Packet *packet)
{
    auto player = Players::getPlayerByGUID(packet->guid);

    PlayerPacket *myPacket = playerPacketController->GetPacket(packet->data[0]);

    if (packet->data[0] == ID_HANDSHAKE)
    {
        myPacket->setPlayer(player.get());
        myPacket->Read();

        if (player->isHandshaked())
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Wrong handshake with player %d, name: %s", player->getId(),
                               player->npc.mName.c_str());
            kickPlayer(player->guid);
            return;
        }

        if (player->passw != serverPassword)
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Wrong server password for player %d, name: %s (pass: %s)",
                               player->getId(), player->npc.mName.c_str(), player->passw.c_str());
            kickPlayer(player->guid);
            return;
        }
        player->setHandshake();
        return;
    }

    if (!player->isHandshaked())
    {
        LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Have not completed handshake with player %d", player->getId());
        //KickPlayer(player->guid);
        return;
    }

    if (packet->data[0] == ID_LOADED)
    {
        player->setLoadState(Player::LOADED);

        bool result = luaState.getEventCtrl().Call<CoreEvent::ON_PLAYER_CONNECT, bool>(player);

        if (!result)
        {
            playerPacketController->GetPacket(ID_USER_DISCONNECTED)->setPlayer(player.get());
            playerPacketController->GetPacket(ID_USER_DISCONNECTED)->Send(false);
            Players::deletePlayerByGUID(packet->guid);
            return;
        }
    }
    else if (packet->data[0] == ID_PLAYER_BASEINFO)
    {
        LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Received ID_PLAYER_BASEINFO about %s", player->npc.mName.c_str());

        myPacket->setPlayer(player.get());
        myPacket->Read();
        myPacket->Send(true);
    }

    if (player->getLoadState() == Player::NOTLOADED)
        return;
    else if (player->getLoadState() == Player::LOADED)
    {
        player->setLoadState(Player::POSTLOADED);
        newPlayer(packet->guid);
        return;
    }


    if (!PlayerProcessor::Process(*packet))
        LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Unhandled PlayerPacket with identifier %i has arrived", packet->data[0]);

}

void Networking::processActorPacket(RakNet::Packet *packet)
{
    auto player = Players::getPlayerByGUID(packet->guid);

    if (!player->isHandshaked() || player->getLoadState() != Player::POSTLOADED)
        return;

    if (!ActorProcessor::Process(*packet, baseActorList))
        LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Unhandled ActorPacket with identifier %i has arrived", packet->data[0]);

}

void Networking::processWorldPacket(RakNet::Packet *packet)
{
    auto player = Players::getPlayerByGUID(packet->guid);

    if (!player->isHandshaked() || player->getLoadState() != Player::POSTLOADED)
        return;

    if (!WorldProcessor::Process(*packet, baseEvent))
        LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Unhandled WorldPacket with identifier %i has arrived", packet->data[0]);

}

bool Networking::update(RakNet::Packet *packet)
{
    auto player = Players::getPlayerByGUID(packet->guid);

    RakNet::BitStream bsIn(&packet->data[1], packet->length, false);

    bsIn.IgnoreBytes((unsigned int) RakNet::RakNetGUID::size()); // Ignore GUID from received packet

    if (player == nullptr)
    {
        if (packet->data[0] == ID_GAME_PREINIT)
        {
            DEBUG_PRINTF("ID_GAME_PREINIT");
            PacketPreInit::PluginContainer plugins;

            PacketPreInit packetPreInit(peer);
            packetPreInit.SetReadStream(&bsIn);
            packetPreInit.setChecksums(&plugins);
            packetPreInit.Read();

            auto plugin = plugins.begin();
            if (samples.size() == plugins.size())
            {
                for (int i = 0; plugin != plugins.end(); plugin++, i++)
                {
                    LOG_APPEND(Log::LOG_VERBOSE, "- %X\t%s", plugin->second[0], plugin->first.c_str());
                    // Check if the filenames match, ignoring case
                    if (Misc::StringUtils::ciEqual(samples[i].first, plugin->first))
                    {
                        auto &hashList = samples[i].second;
                        // Proceed if no checksums have been listed for this plugin on the server
                        if (hashList.empty())
                            continue;
                        auto it = find(hashList.begin(), hashList.end(), plugin->second[0]);
                        // Break the loop if the client's checksum isn't among those accepted by
                        // the server
                        if (it == hashList.end())
                            break;
                    }
                    else // name is incorrect
                        break;
                }
            }
            RakNet::BitStream bs;
            packetPreInit.SetSendStream(&bs);

            // If the loop above was broken, then the client's plugins do not match the server's
            if (plugin != plugins.end())
            {
                packetPreInit.setChecksums(&samples);
                packetPreInit.Send(packet->systemAddress);
                peer->CloseConnection(packet->systemAddress, true);
            }
            else
            {
                PacketPreInit::PluginContainer tmp;
                packetPreInit.setChecksums(&tmp);
                packetPreInit.Send(packet->systemAddress);
            }
            return false;
        }

        playerPacketController->SetStream(&bsIn, 0);

        playerPacketController->GetPacket(ID_HANDSHAKE)->RequestData(packet->guid);
        player = Players::addPlayer(packet->guid);
        return false;
    }

    if (playerPacketController->ContainsPacket(packet->data[0]))
    {
        playerPacketController->SetStream(&bsIn, 0);
        processPlayerPacket(packet);
        return true;
    }

    if (actorPacketController->ContainsPacket(packet->data[0]))
    {
        actorPacketController->SetStream(&bsIn, 0);
        processActorPacket(packet);
        return true;
    }

    if (worldPacketController->ContainsPacket(packet->data[0]))
    {
        worldPacketController->SetStream(&bsIn, 0);
        processWorldPacket(packet);
        return true;
    }

    LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Unhandled RakNet packet with identifier %i has arrived", packet->data[0]);
    return false;
}

void Networking::newPlayer(RakNet::RakNetGUID guid)
{
    playerPacketController->GetPacket(ID_PLAYER_BASEINFO)->RequestData(guid);
    playerPacketController->GetPacket(ID_PLAYER_STATS_DYNAMIC)->RequestData(guid);
    playerPacketController->GetPacket(ID_PLAYER_POSITION)->RequestData(guid);
    playerPacketController->GetPacket(ID_PLAYER_CELL_CHANGE)->RequestData(guid);
    playerPacketController->GetPacket(ID_PLAYER_EQUIPMENT)->RequestData(guid);

    LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Sending info about other players to %lu", guid.g);

    Players::for_each([this, &guid](auto pl) //sending other players to new player
    {
        // If we are iterating over the new player, don't send the packets below
        if (pl->guid == guid) return;

        // If an invalid key makes it into the Players map, ignore it
        else if (pl->guid == RakNet::UNASSIGNED_RAKNET_GUID) return;

        // If we are iterating over a player who has inputted their name, proceed
        else if (pl->getLoadState() == Player::POSTLOADED)
        {
            playerPacketController->GetPacket(ID_PLAYER_BASEINFO)->setPlayer(pl.get());
            playerPacketController->GetPacket(ID_PLAYER_STATS_DYNAMIC)->setPlayer(pl.get());
            playerPacketController->GetPacket(ID_PLAYER_ATTRIBUTE)->setPlayer(pl.get());
            playerPacketController->GetPacket(ID_PLAYER_SKILL)->setPlayer(pl.get());
            playerPacketController->GetPacket(ID_PLAYER_POSITION)->setPlayer(pl.get());
            playerPacketController->GetPacket(ID_PLAYER_CELL_CHANGE)->setPlayer(pl.get());
            playerPacketController->GetPacket(ID_PLAYER_EQUIPMENT)->setPlayer(pl.get());

            playerPacketController->GetPacket(ID_PLAYER_BASEINFO)->Send(guid);
            playerPacketController->GetPacket(ID_PLAYER_STATS_DYNAMIC)->Send(guid);
            playerPacketController->GetPacket(ID_PLAYER_ATTRIBUTE)->Send(guid);
            playerPacketController->GetPacket(ID_PLAYER_SKILL)->Send(guid);
            playerPacketController->GetPacket(ID_PLAYER_POSITION)->Send(guid);
            playerPacketController->GetPacket(ID_PLAYER_CELL_CHANGE)->Send(guid);
            playerPacketController->GetPacket(ID_PLAYER_EQUIPMENT)->Send(guid);
        }
    });

    LOG_APPEND(Log::LOG_WARN, "- Done");

}

void Networking::disconnectPlayer(RakNet::RakNetGUID guid)
{
    auto player = Players::getPlayerByGUID(guid);
    if (player == nullptr)
        return;

    luaState.getEventCtrl().Call<CoreEvent::ON_PLAYER_DISCONNECT>(player);

    playerPacketController->GetPacket(ID_USER_DISCONNECTED)->setPlayer(player.get());
    playerPacketController->GetPacket(ID_USER_DISCONNECTED)->Send(true);
    Players::deletePlayerByGUID(guid);
}

PlayerPacketController *Networking::getPlayerPacketController() const
{
    return playerPacketController;
}

ActorPacketController *Networking::getActorPacketController() const
{
    return actorPacketController;
}

WorldPacketController *Networking::getWorldPacketController() const
{
    return worldPacketController;
}

BaseActorList *Networking::getLastActorList()
{
    return &baseActorList;
}

BaseEvent *Networking::getLastEvent()
{
    return &baseEvent;
}

int Networking::getCurrentMpNum()
{
    return currentMpNum;
}

void Networking::setCurrentMpNum(int value)
{
    currentMpNum = value;
}

int Networking::incrementMpNum()
{
    currentMpNum++;
    luaState.getEventCtrl().Call<CoreEvent::ON_MP_REFNUM>(currentMpNum);
    return currentMpNum;
}

Networking &Networking::get()
{
    return *sThis;
}


Networking *Networking::getPtr()
{
    return sThis;
}

RakNet::SystemAddress Networking::getSystemAddress(RakNet::RakNetGUID guid)
{
    return peer->GetSystemAddressFromGuid(guid);
}

PacketPreInit::PluginContainer Networking::getPluginListSample()
{
    PacketPreInit::PluginContainer pls;
    unsigned id = 0;
    while (true)
    {
        unsigned field = 0;
        auto name = luaState.getEventCtrl().Call<CoreEvent::ON_REQUEST_PLUGIN_LIST, string>(id, field++);
        if (name.size() == 0)
            break;
        PacketPreInit::HashList hashList;
        while (true)
        {
            auto hash = luaState.getEventCtrl().Call<CoreEvent::ON_REQUEST_PLUGIN_LIST, string>(id, field++);
            if (hash.size() == 0)
                break;
            hashList.push_back((unsigned)stoul(hash));
        }
        pls.push_back({name, hashList});
        id++;
    }
    return pls;
}

void Networking::stopServer(int code)
{
    running = false;
    exitCode = code;
}

int Networking::mainLoop()
{
    RakNet::Packet *packet;

    auto &timerCtrl = luaState.getTimerCtrl();

    while (running)
    {
        bool updated = false;
        if (kbhit() && getch() == '\n')
            break;
        for (packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
        {
            if (getMasterClient()->Process(packet))
                continue;

            switch (packet->data[0])
            {
                case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                    LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Client at %s has disconnected", packet->systemAddress.ToString());
                    break;
                case ID_REMOTE_CONNECTION_LOST:
                    LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Client at %s has lost connection", packet->systemAddress.ToString());
                    break;
                case ID_REMOTE_NEW_INCOMING_CONNECTION:
                    LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Client at %s has connected", packet->systemAddress.ToString());
                    break;
                case ID_CONNECTION_REQUEST_ACCEPTED:    // client to server
                {
                    LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Our connection request has been accepted");
                    break;
                }
                case ID_NEW_INCOMING_CONNECTION:
                    LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "A connection is incoming from %s", packet->systemAddress.ToString());
                    break;
                case ID_NO_FREE_INCOMING_CONNECTIONS:
                    LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "The server is full");
                    break;
                case ID_DISCONNECTION_NOTIFICATION:
                    LOG_MESSAGE_SIMPLE(Log::LOG_WARN,  "Client at %s has disconnected", packet->systemAddress.ToString());
                    disconnectPlayer(packet->guid);
                    break;
                case ID_CONNECTION_LOST:
                    LOG_MESSAGE_SIMPLE(Log::LOG_WARN, "Client at %s has lost connection", packet->systemAddress.ToString());
                    disconnectPlayer(packet->guid);
                    break;
                case ID_SND_RECEIPT_ACKED:
                case ID_CONNECTED_PING:
                case ID_UNCONNECTED_PING:
                    break;
                default:
                    updated = update(packet);
                    break;
            }
        }
        timerCtrl.tick();
        if (updated)
        {
            // fixme: not good to call for_each every frame. Maybe for_each_Updated will be better
            Players::for_each([](shared_ptr<Player> pl) {
                pl->update();
            });
        }
        this_thread::sleep_for (chrono::milliseconds(1));
    }

    timerCtrl.terminate();
    return exitCode;
}

void Networking::kickPlayer(RakNet::RakNetGUID guid)
{
    peer->CloseConnection(guid, true);
}

void Networking::banAddress(const char *ipAddress)
{
    peer->AddToBanList(ipAddress);
}

void Networking::unbanAddress(const char *ipAddress)
{
    peer->RemoveFromBanList(ipAddress);
}

unsigned short Networking::numberOfConnections() const
{
    return peer->NumberOfConnections();
}

unsigned int Networking::maxConnections() const
{
    return peer->GetMaximumIncomingConnections();
}

int Networking::getAvgPing(RakNet::AddressOrGUID addr) const
{
    return peer->GetAveragePing(addr);
}

MasterClient *Networking::getMasterClient()
{
    return mclient;
}

void Networking::InitQuery(std::string queryAddr, unsigned short queryPort)
{
    mclient = new MasterClient(peer, queryAddr, queryPort);
}

void Networking::postInit()
{
    luaState.getEventCtrl().Call<CoreEvent::ON_POST_INIT>();
    samples = getPluginListSample();
    if (mclient)
    {
        for (auto plugin : samples)
        {
            if (!plugin.second.empty())
                mclient->PushPlugin({plugin.first, plugin.second[0]});
            else
                mclient->PushPlugin({plugin.first, 0});
        }
    }
}
