#include "Player.hpp"
#include "processors/ProcessorInitializer.hpp"
#include <RakPeer.h>
#include <Kbhit.h>

#include <components/misc/stringops.hpp>
#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/MWMPLog.hpp>
#include <components/openmw-mp/Version.hpp>
#include <components/openmw-mp/Packets/PacketPreInit.hpp>

#include <iostream>
#include <Script/Script.hpp>
#include <Script/API/TimerAPI.hpp>
#include <chrono>
#include <thread>

#include "Networking.hpp"
#include "MasterClient.hpp"
#include "Cell.hpp"
#include "CellController.hpp"
#include "processors/PlayerProcessor.hpp"
#include "processors/ActorProcessor.hpp"
#include "processors/ObjectProcessor.hpp"
#include "processors/WorldstateProcessor.hpp"

using namespace mwmp;
using namespace std;

Networking *Networking::sThis = 0;

static int currentMpNum = 0;
static bool dataFileEnforcementState = true;
static bool scriptErrorIgnoringState = false;

Networking::Networking(RakNet::RakPeerInterface *peer) : mclient(nullptr)
{
    sThis = this;
    this->peer = peer;
    players = Players::getPlayers();

    CellController::create();

    playerPacketController = new PlayerPacketController(peer);
    actorPacketController = new ActorPacketController(peer);
    objectPacketController = new ObjectPacketController(peer);
    worldstatePacketController = new WorldstatePacketController(peer);

    // Set send stream
    playerPacketController->SetStream(0, &bsOut);
    actorPacketController->SetStream(0, &bsOut);
    objectPacketController->SetStream(0, &bsOut);
    worldstatePacketController->SetStream(0, &bsOut);

    running = true;
    exitCode = 0;

    Script::Call<Script::CallbackIdentity("OnServerInit")>();

    serverPassword = TES3MP_DEFAULT_PASSW;

    ProcessorInitializer();
}

Networking::~Networking()
{
    Script::Call<Script::CallbackIdentity("OnServerExit")>(false);

    CellController::destroy();

    sThis = 0;
    delete playerPacketController;
    delete actorPacketController;
    delete objectPacketController;
    delete worldstatePacketController;
}

void Networking::setServerPassword(std::string password) noexcept
{
    serverPassword = password.empty() ? TES3MP_DEFAULT_PASSW : password;
}

bool Networking::isPassworded() const
{
    return serverPassword != TES3MP_DEFAULT_PASSW;
}

void Networking::processPlayerPacket(RakNet::Packet *packet)
{
    Player *player = Players::getPlayer(packet->guid);

    PlayerPacket *myPacket = playerPacketController->GetPacket(packet->data[0]);

    if (packet->data[0] == ID_HANDSHAKE)
    {
        myPacket->setPlayer(player);
        myPacket->Read();

        if (!myPacket->isPacketValid())
        {
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, "Invalid handshake packet from client at %s", packet->systemAddress.ToString());
            kickPlayer(player->guid);
            return;
        }

        if (player->isHandshaked())
        {
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Wrong handshake with client at %s", packet->systemAddress.ToString());
            kickPlayer(player->guid);
            return;
        }

        if (player->serverPassword != serverPassword)
        {
            if (isPassworded())
            {
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Wrong server password %s used by client at %s",
                    player->serverPassword.c_str(), packet->systemAddress.ToString());
                kickPlayer(player->guid);
                return;
            }
            else
            {
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Client at %s tried to join using password, despite the server not being passworded",
                    packet->systemAddress.ToString());
            }
        }
        player->setHandshake();
        return;
    }

    if (!player->isHandshaked())
    {
        player->incrementHandshakeAttempts();
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Have not completed handshake with client at %s", packet->systemAddress.ToString());
        LOG_APPEND(MWMPLog::LOG_WARN, "- Attempts so far: %i", player->getHandshakeAttempts());

        if (player->getHandshakeAttempts() > 20)
            kickPlayer(player->guid, false);
        else if (player->getHandshakeAttempts() > 5)
            kickPlayer(player->guid, true);

        return;
    }

    if (packet->data[0] == ID_LOADED)
    {
        player->setLoadState(Player::LOADED);

        unsigned short pid = Players::getPlayer(packet->guid)->getId();
        Script::Call<Script::CallbackIdentity("OnPlayerConnect")>(pid);

        if (player->getLoadState() == Player::KICKED) // kicked inside in OnPlayerConnect
        {
            playerPacketController->GetPacket(ID_USER_DISCONNECTED)->setPlayer(Players::getPlayer(packet->guid));
            playerPacketController->GetPacket(ID_USER_DISCONNECTED)->Send(false);
            Players::deletePlayer(packet->guid);
            return;
        }
    }
    else if (packet->data[0] == ID_PLAYER_BASEINFO)
    {
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Received ID_PLAYER_BASEINFO about %s", player->npc.mName.c_str());

        myPacket->setPlayer(player);
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
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Unhandled PlayerPacket with identifier %i has arrived", packet->data[0]);

}

void Networking::processActorPacket(RakNet::Packet *packet)
{
    Player *player = Players::getPlayer(packet->guid);

    if (!player->isHandshaked() || player->getLoadState() != Player::POSTLOADED)
        return;

    if (!ActorProcessor::Process(*packet, baseActorList))
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Unhandled ActorPacket with identifier %i has arrived", packet->data[0]);

}

void Networking::processObjectPacket(RakNet::Packet *packet)
{
    Player *player = Players::getPlayer(packet->guid);

    if (!player->isHandshaked() || player->getLoadState() != Player::POSTLOADED)
        return;

    if (!ObjectProcessor::Process(*packet, baseObjectList))
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Unhandled ObjectPacket with identifier %i has arrived", packet->data[0]);

}

void Networking::processWorldstatePacket(RakNet::Packet *packet)
{
    Player *player = Players::getPlayer(packet->guid);

    if (!player->isHandshaked() || player->getLoadState() != Player::POSTLOADED)
        return;

    if (!WorldstateProcessor::Process(*packet, baseWorldstate))
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Unhandled WorldstatePacket with identifier %i has arrived", packet->data[0]);

}

bool Networking::preInit(RakNet::Packet *packet, RakNet::BitStream &bsIn)
{
    if (packet->data[0] != ID_GAME_PREINIT)
    {
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "%s sent wrong first packet (ID_GAME_PREINIT was expected)",
                           packet->systemAddress.ToString());
        peer->CloseConnection(packet->systemAddress, true);
    }

    DEBUG_PRINTF("ID_GAME_PREINIT");
    PacketPreInit::PluginContainer dataFiles;

    PacketPreInit packetPreInit(peer);
    packetPreInit.SetReadStream(&bsIn);
    packetPreInit.setChecksums(&dataFiles);
    packetPreInit.Read();

    if (!packetPreInit.isPacketValid() || dataFiles.empty())
    {
        LOG_APPEND(MWMPLog::LOG_ERROR, "Invalid packetPreInit");
        peer->CloseConnection(packet->systemAddress, false); // close connection without notification
        return false;
    }

    auto dataFile = dataFiles.begin();
    if (samples.size() == dataFiles.size())
    {
        for (int i = 0; dataFile != dataFiles.end(); dataFile++, i++)
        {
            LOG_APPEND(MWMPLog::LOG_VERBOSE, "- %X\t%s", dataFile->second[0], dataFile->first.c_str());
            // Check if the filenames match, ignoring case
            if (Misc::StringUtils::ciEqual(samples[i].first, dataFile->first))
            {
                auto &hashList = samples[i].second;
                // Proceed if no checksums have been listed for this dataFile on the server
                if (hashList.empty())
                    continue;
                auto it = find(hashList.begin(), hashList.end(), dataFile->second[0]);
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

    // If the loop above was broken, then the client's data files do not match the server's
    if (dataFileEnforcementState && dataFile != dataFiles.end())
    {
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "%s was not allowed to connect due to incompatible data files", packet->systemAddress.ToString());
        packetPreInit.setChecksums(&samples);
        packetPreInit.Send(packet->systemAddress);
        peer->CloseConnection(packet->systemAddress, true);
    }
    else
    {
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "%s was allowed to connect", packet->systemAddress.ToString());
        PacketPreInit::PluginContainer tmp;
        packetPreInit.setChecksums(&tmp);
        packetPreInit.Send(packet->systemAddress);
        Players::newPlayer(packet->guid); // create player if connection allowed
        playerPacketController->SetStream(&bsIn, nullptr); // and request handshake
        playerPacketController->GetPacket(ID_HANDSHAKE)->RequestData(packet->guid);
        return true;
    }

    return false;
}

void Networking::update(RakNet::Packet *packet, RakNet::BitStream &bsIn)
{
    if (playerPacketController->ContainsPacket(packet->data[0]))
    {
        playerPacketController->SetStream(&bsIn, nullptr);
        processPlayerPacket(packet);
    }
    else if (actorPacketController->ContainsPacket(packet->data[0]))
    {
        actorPacketController->SetStream(&bsIn, 0);
        processActorPacket(packet);
    }
    else if (objectPacketController->ContainsPacket(packet->data[0]))
    {
        objectPacketController->SetStream(&bsIn, 0);
        processObjectPacket(packet);
    }
    else if (worldstatePacketController->ContainsPacket(packet->data[0]))
    {
        worldstatePacketController->SetStream(&bsIn, 0);
        processWorldstatePacket(packet);
    }
    else
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Unhandled RakNet packet with identifier %i has arrived", packet->data[0]);
}

void Networking::newPlayer(RakNet::RakNetGUID guid)
{
    playerPacketController->GetPacket(ID_PLAYER_BASEINFO)->RequestData(guid);
    playerPacketController->GetPacket(ID_PLAYER_STATS_DYNAMIC)->RequestData(guid);
    playerPacketController->GetPacket(ID_PLAYER_POSITION)->RequestData(guid);
    playerPacketController->GetPacket(ID_PLAYER_CELL_CHANGE)->RequestData(guid);
    playerPacketController->GetPacket(ID_PLAYER_EQUIPMENT)->RequestData(guid);

    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Sending info about other players to %lu", guid.g);

    for (TPlayers::iterator pl = players->begin(); pl != players->end(); pl++) //sending other players to new player
    {
        // If we are iterating over the new player, don't send the packets below
        if (pl->first == guid) continue;

        // If an invalid key makes it into the Players map, ignore it
        else if (pl->first == RakNet::UNASSIGNED_CRABNET_GUID) continue;

        // if player not fully connected
        else if (pl->second == nullptr) continue;

        // If we are iterating over a player who has inputted their name, proceed
        else if (pl->second->getLoadState() == Player::POSTLOADED)
        {
            playerPacketController->GetPacket(ID_PLAYER_BASEINFO)->setPlayer(pl->second);
            playerPacketController->GetPacket(ID_PLAYER_STATS_DYNAMIC)->setPlayer(pl->second);
            playerPacketController->GetPacket(ID_PLAYER_ATTRIBUTE)->setPlayer(pl->second);
            playerPacketController->GetPacket(ID_PLAYER_SKILL)->setPlayer(pl->second);
            playerPacketController->GetPacket(ID_PLAYER_POSITION)->setPlayer(pl->second);
            playerPacketController->GetPacket(ID_PLAYER_CELL_CHANGE)->setPlayer(pl->second);
            playerPacketController->GetPacket(ID_PLAYER_EQUIPMENT)->setPlayer(pl->second);

            playerPacketController->GetPacket(ID_PLAYER_BASEINFO)->Send(guid);
            playerPacketController->GetPacket(ID_PLAYER_STATS_DYNAMIC)->Send(guid);
            playerPacketController->GetPacket(ID_PLAYER_ATTRIBUTE)->Send(guid);
            playerPacketController->GetPacket(ID_PLAYER_SKILL)->Send(guid);
            playerPacketController->GetPacket(ID_PLAYER_POSITION)->Send(guid);
            playerPacketController->GetPacket(ID_PLAYER_CELL_CHANGE)->Send(guid);
            playerPacketController->GetPacket(ID_PLAYER_EQUIPMENT)->Send(guid);
        }
    }

    LOG_APPEND(MWMPLog::LOG_WARN, "- Done");

}

void Networking::disconnectPlayer(RakNet::RakNetGUID guid)
{
    Player *player = Players::getPlayer(guid);
    if (!player)
        return;
    Script::Call<Script::CallbackIdentity("OnPlayerDisconnect")>(player->getId());

    playerPacketController->GetPacket(ID_USER_DISCONNECTED)->setPlayer(player);
    playerPacketController->GetPacket(ID_USER_DISCONNECTED)->Send(true);
    Players::deletePlayer(guid);
}

PlayerPacketController *Networking::getPlayerPacketController() const
{
    return playerPacketController;
}

ActorPacketController *Networking::getActorPacketController() const
{
    return actorPacketController;
}

ObjectPacketController *Networking::getObjectPacketController() const
{
    return objectPacketController;
}

WorldstatePacketController *Networking::getWorldstatePacketController() const
{
    return worldstatePacketController;
}

BaseActorList *Networking::getReceivedActorList()
{
    return &baseActorList;
}

BaseObjectList *Networking::getReceivedObjectList()
{
    return &baseObjectList;
}

BaseWorldstate *Networking::getReceivedWorldstate()
{
    return &baseWorldstate;
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
    Script::Call<Script::CallbackIdentity("OnMpNumIncrement")>(currentMpNum);
    return currentMpNum;
}

bool Networking::getDataFileEnforcementState()
{
    return dataFileEnforcementState;
}

void Networking::setDataFileEnforcementState(bool state)
{
    dataFileEnforcementState = state;
}

bool Networking::getScriptErrorIgnoringState()
{
    return scriptErrorIgnoringState;
}

void Networking::setScriptErrorIgnoringState(bool state)
{
    scriptErrorIgnoringState = state;
}

const Networking &Networking::get()
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

void Networking::stopServer(int code)
{
    running = false;
    exitCode = code;
}

int Networking::mainLoop()
{
    RakNet::Packet *packet;

    while (running)
    {
        if (kbhit() && getch() == '\n')
            break;
        for (packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
        {
            if (getMasterClient()->Process(packet))
                continue;

            switch (packet->data[0])
            {
                case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Client at %s has disconnected", packet->systemAddress.ToString());
                    break;
                case ID_REMOTE_CONNECTION_LOST:
                    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Client at %s has lost connection", packet->systemAddress.ToString());
                    break;
                case ID_REMOTE_NEW_INCOMING_CONNECTION:
                    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Client at %s has connected", packet->systemAddress.ToString());
                    break;
                case ID_CONNECTION_REQUEST_ACCEPTED:    // client to server
                {
                    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Our connection request has been accepted");
                    break;
                }
                case ID_NEW_INCOMING_CONNECTION:
                    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "A connection is incoming from %s", packet->systemAddress.ToString());
                    break;
                case ID_NO_FREE_INCOMING_CONNECTIONS:
                    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "The server is full");
                    break;
                case ID_DISCONNECTION_NOTIFICATION:
                    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN,  "Client at %s has disconnected", packet->systemAddress.ToString());
                    disconnectPlayer(packet->guid);
                    break;
                case ID_CONNECTION_LOST:
                    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Client at %s has lost connection", packet->systemAddress.ToString());
                    disconnectPlayer(packet->guid);
                    break;
                case ID_SND_RECEIPT_ACKED:
                case ID_CONNECTED_PING:
                case ID_UNCONNECTED_PING:
                    break;
                default:
                {
                    RakNet::BitStream bsIn(&packet->data[1], packet->length, false);
                    bsIn.IgnoreBytes((unsigned int) RakNet::RakNetGUID::size()); // Ignore GUID from received packet


                    if (Players::doesPlayerExist(packet->guid))
                        update(packet, bsIn);
                    else
                        preInit(packet, bsIn);
                    break;
                }
            }
        }
        TimerAPI::Tick();
        this_thread::sleep_for(chrono::milliseconds(1));
    }

    TimerAPI::Terminate();
    return exitCode;
}

void Networking::kickPlayer(RakNet::RakNetGUID guid, bool sendNotification)
{
    peer->CloseConnection(guid, sendNotification);
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

unsigned short Networking::getPort() const
{
    return peer->GetMyBoundAddress().GetPort();
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
    Script::Call<Script::CallbackIdentity("OnRequestDataFileList")>();
    Script::Call<Script::CallbackIdentity("OnServerPostInit")>();
}

PacketPreInit::PluginContainer &Networking::getSamples()
{
    return samples;
}
