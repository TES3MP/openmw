#include <stdexcept>
#include <iostream>
#include <string>

#include <components/openmw-mp/MWMPLog.hpp>
#include <components/openmw-mp/Utils.hpp>
#include <components/openmw-mp/Version.hpp>
#include <components/openmw-mp/Packets/PacketPreInit.hpp>

#include <components/esm/cellid.hpp>
#include <components/files/configurationmanager.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"

#include "../mwclass/npc.hpp"

#include "../mwmechanics/combat.hpp"
#include "../mwmechanics/npcstats.hpp"

#include "../mwstate/statemanagerimp.hpp"

#include "../mwworld/cellstore.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwworld/inventorystore.hpp"

#include <SDL_messagebox.h>
#include <RakSleep.h>
#include <iomanip>
#include <components/version/version.hpp>

#include "Networking.hpp"
#include "Main.hpp"
#include "processors/ProcessorInitializer.hpp"
#include "processors/PlayerProcessor.hpp"
#include "processors/ObjectProcessor.hpp"
#include "processors/ActorProcessor.hpp"
#include "processors/WorldstateProcessor.hpp"
#include "GUIController.hpp"
#include "CellController.hpp"

using namespace std;
using namespace mwmp;

string listDiscrepancies(PacketPreInit::PluginContainer checksums, PacketPreInit::PluginContainer checksumsResponse)
{
    std::ostringstream sstr;
    sstr << "Your plugins or their load order don't match the server's. A full comparison is included in your debug window and latest log file. In short, the following discrepancies have been found:\n\n";

    int discrepancyCount = 0;

    for (size_t fileIndex = 0; fileIndex < checksums.size() || fileIndex < checksumsResponse.size(); fileIndex++)
    {
        if (fileIndex >= checksumsResponse.size())
        {
            discrepancyCount++;

            if (discrepancyCount > 1)
                sstr << "\n";

            string clientFilename = checksums.at(fileIndex).first;

            sstr << fileIndex << ": ";
            sstr << clientFilename << " is past the number of plugins used by the server";
        }
        else if (fileIndex >= checksums.size())
        {
            discrepancyCount++;

            if (discrepancyCount > 1)
                sstr << "\n";

            string serverFilename = checksumsResponse.at(fileIndex).first;

            sstr << fileIndex << ": ";
            sstr << serverFilename << " is completely missing from the client but required by the server";
        }
        else
        {
            string clientFilename = checksums.at(fileIndex).first;
            string serverFilename = checksumsResponse.at(fileIndex).first;

            string clientChecksum = Utils::intToHexStr(checksums.at(fileIndex).second.at(0));

            bool filenameMatches = false;
            bool checksumMatches = false;
            string eligibleChecksums = "";

            if (Misc::StringUtils::ciEqual(clientFilename, serverFilename))
                filenameMatches = true;

            if (checksumsResponse.at(fileIndex).second.size() > 0)
            {
                for (size_t checksumIndex = 0; checksumIndex < checksumsResponse.at(fileIndex).second.size(); checksumIndex++)
                {
                    string serverChecksum = Utils::intToHexStr(checksumsResponse.at(fileIndex).second.at(checksumIndex));

                    if (checksumIndex != 0)
                        eligibleChecksums = eligibleChecksums + " or ";

                    eligibleChecksums = eligibleChecksums + serverChecksum;

                    if (Misc::StringUtils::ciEqual(clientChecksum, serverChecksum))
                    {
                        checksumMatches = true;
                        break;
                    }
                }
            }
            else
                checksumMatches = true;

            if (!filenameMatches || !checksumMatches)
            {
                discrepancyCount++;

                if (discrepancyCount > 1)
                    sstr << "\n";

                sstr << fileIndex << ": ";

                if (!filenameMatches)
                    sstr << clientFilename << " doesn't match " << serverFilename;

                if (!filenameMatches && !checksumMatches)
                    sstr << ", ";

                if (!checksumMatches)
                    sstr << "checksum " << clientChecksum << " doesn't match " << eligibleChecksums;
            }
        }
    }

    return sstr.str();
}

string listComparison(PacketPreInit::PluginContainer checksums, PacketPreInit::PluginContainer checksumsResponse,
                      bool full = false)
{
    std::ostringstream sstr;
    size_t pluginNameLen1 = 0;
    size_t pluginNameLen2 = 0;
    for (const auto &checksum : checksums)
        if (pluginNameLen1 < checksum.first.size())
            pluginNameLen1 = checksum.first.size();

    for (const auto &checksum : checksums)
        if (pluginNameLen2 < checksum.first.size())
            pluginNameLen2 = checksum.first.size();

    Utils::printWithWidth(sstr, "Your current plugins are:", pluginNameLen1 + 16);
    sstr << "To join this server, use:\n";

    Utils::printWithWidth(sstr, "name", pluginNameLen1 + 2);
    Utils::printWithWidth(sstr, "hash", 14);
    Utils::printWithWidth(sstr, "name", pluginNameLen2 + 2);
    sstr << "hash\n";

    for (size_t i = 0; i < checksums.size() || i < checksumsResponse.size(); i++)
    {
        string plugin;
        unsigned val;

        if (i < checksums.size())
        {
            plugin = checksums.at(i).first;
            val = checksums.at(i).second[0];

            Utils::printWithWidth(sstr, plugin, pluginNameLen1 + 2);
            Utils::printWithWidth(sstr, Utils::intToHexStr(val), 14);
        }
        else
            Utils::printWithWidth(sstr, "", pluginNameLen1 + 16);

        if (i < checksumsResponse.size())
        {
            Utils::printWithWidth(sstr, checksumsResponse[i].first, pluginNameLen2 + 2);
            if (checksumsResponse[i].second.size() > 0)
            {
                if (full)
                    for (size_t j = 0; j < checksumsResponse[i].second.size(); j++)
                        Utils::printWithWidth(sstr, Utils::intToHexStr(checksumsResponse[i].second[j]), 14);
                else
                    sstr << Utils::intToHexStr(checksumsResponse[i].second[0]);
            }
            else
                sstr << "any";
        }

        sstr << "\n";
    }

    return sstr.str();
}

Networking::Networking(): peer(RakNet::RakPeerInterface::GetInstance()), playerPacketController(peer),
    actorPacketController(peer), objectPacketController(peer), worldstatePacketController(peer)
{

    RakNet::SocketDescriptor sd;
    sd.port=0;
    auto b = peer->Startup(1, &sd, 1);
    RakAssert(b==RakNet::CRABNET_STARTED);

    playerPacketController.SetStream(0, &bsOut);
    actorPacketController.SetStream(0, &bsOut);
    objectPacketController.SetStream(0, &bsOut);
    worldstatePacketController.SetStream(0, &bsOut);

    connected = 0;
    ProcessorInitializer();
}

Networking::~Networking()
{
    peer->Shutdown(100);
    peer->CloseConnection(peer->GetSystemAddressFromIndex(0), true, 0);
    RakNet::RakPeerInterface::DestroyInstance(peer);
}

void Networking::update()
{
    RakNet::Packet *packet;
    std::string errmsg = "";

    for (packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
    {
        switch (packet->data[0])
        {
            case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Another client has disconnected.");
                break;
            case ID_REMOTE_CONNECTION_LOST:
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Another client has lost connection.");
                break;
            case ID_REMOTE_NEW_INCOMING_CONNECTION:
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Another client has connected.");
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Our connection request has been accepted.");
                break;
            case ID_NEW_INCOMING_CONNECTION:
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "A connection is incoming.");
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                errmsg = "The server is full.";
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                errmsg = "We have been disconnected.";
                break;
            case ID_CONNECTION_LOST:
                errmsg = "Connection lost.";
                break;
            default:
                receiveMessage(packet);
                //LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Message with identifier %i has arrived.", packet->data[0]);
                break;
        }
    }

    if (!errmsg.empty())
    {
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, errmsg.c_str());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "tes3mp", errmsg.c_str(), 0);
        MWBase::Environment::get().getStateManager()->requestQuit();
    }
}

void Networking::connect(const std::string &ip, unsigned short port, std::vector<string> &content, Files::Collections &collections)
{
    RakNet::SystemAddress master;
    master.SetBinaryAddress(ip.c_str());
    master.SetPortHostOrder(port);
    std::string errmsg = "";

    stringstream sstr;
    sstr << TES3MP_VERSION;
    sstr << TES3MP_PROTO_VERSION;
    sstr << Version::getOpenmwVersion(Main::getResDir()).mCommitHash;

    if (peer->Connect(master.ToString(false), master.GetPort(), sstr.str().c_str(), (int) sstr.str().size(), 0, 0, 3, 500, 0) != RakNet::CONNECTION_ATTEMPT_STARTED)
        errmsg = "Connection attempt failed.\n";

    bool queue = true;
    while (queue)
    {
        for (RakNet::Packet *packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
        {
            switch (packet->data[0])
            {
                case ID_CONNECTION_ATTEMPT_FAILED:
                {
                    errmsg = "Connection failed.\n"
                            "Either the IP address is wrong or a firewall on either system is blocking\n"
                            "UDP packets on the port you have chosen.";
                    queue = false;
                    break;
                }
                case ID_INVALID_PASSWORD:
                {
                    errmsg = "Version mismatch!\nYour client is on version " TES3MP_VERSION "\n"
                        "Please make sure the server is on the same version.";
                    queue = false;
                    break;
                }
                case ID_INCOMPATIBLE_PROTOCOL_VERSION:
                {
                    errmsg = "Network protocol mismatch!\nMake sure your client is really on the same version\n"
                        "as the server you are trying to connect to.";
                    queue = false;
                    break;
                }
                case ID_CONNECTION_REQUEST_ACCEPTED:
                {
                    serverAddr = packet->systemAddress;
                    BaseClientPacketProcessor::SetServerAddr(packet->systemAddress);

                    connected = true;
                    queue = false;

                    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Received ID_CONNECTION_REQUESTED_ACCEPTED from %s",
                                       serverAddr.ToString());

                    break;
                }
                case ID_DISCONNECTION_NOTIFICATION:
                    throw runtime_error("ID_DISCONNECTION_NOTIFICATION.\n");
                case ID_CONNECTION_BANNED:
                    throw runtime_error("You have been banned from this server.\n");
                case ID_CONNECTION_LOST:
                    throw runtime_error("ID_CONNECTION_LOST.\n");
                default:
                    LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Connection message with identifier %i has arrived in initialization.",
                                       packet->data[0]);
            }
        }
    }

    if (!errmsg.empty())
    {
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, errmsg.c_str());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "tes3mp", errmsg.c_str(), 0);
    }
    else
        preInit(content, collections);

    getLocalPlayer()->guid = peer->GetMyGUID();
}

void Networking::preInit(std::vector<std::string> &content, Files::Collections &collections)
{
    PacketPreInit::PluginContainer checksums;
    vector<string>::const_iterator it(content.begin());
    for (int idx = 0; it != content.end(); ++it, ++idx)
    {
        boost::filesystem::path filename(*it);
        const Files::MultiDirCollection& col = collections.getCollection(filename.extension().string());
        if (col.doesExist(*it))
        {
            PacketPreInit::HashList hashList;
            unsigned crc32 = Utils::crc32Checksum(col.getPath(*it).string());
            hashList.push_back(crc32);
            checksums.push_back(make_pair(*it, hashList));

            LOG_APPEND(MWMPLog::LOG_WARN, "idx: %d\tchecksum: %X\tfile: %s\n", idx, crc32, col.getPath(*it).string().c_str());
        }
        else
            throw std::runtime_error("Plugin doesn't exist.");
    }

    PacketPreInit packetPreInit(peer);
    RakNet::BitStream bs;
    RakNet::RakNetGUID guid;
    packetPreInit.setChecksums(&checksums);
    packetPreInit.setGUID(guid);
    packetPreInit.SetSendStream(&bs);
    packetPreInit.Send(serverAddr);

    PacketPreInit::PluginContainer checksumsResponse;
    bool done = false;
    while (!done)
    {
        RakNet::Packet *packet = peer->Receive();
        if (!packet)
        {
            RakSleep(500);
            continue;
        }

        RakNet::BitStream bsIn(&packet->data[0], packet->length, false);
        unsigned char packetId;
        bsIn.Read(packetId);
        switch(packetId)
        {
            case ID_DISCONNECTION_NOTIFICATION:
            case ID_CONNECTION_LOST:
                done = true;
                break;
            case ID_GAME_PREINIT:
                bsIn.IgnoreBytes((unsigned) RakNet::RakNetGUID::size());
                packetPreInit.setChecksums(&checksumsResponse);
                packetPreInit.Packet(&bsIn, false);
                done = true;
                break;
        }

        peer->DeallocatePacket(packet);
    }

    if (!checksumsResponse.empty()) // something wrong
    {
        std::string errmsg = listDiscrepancies(checksums, checksumsResponse);

        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, listDiscrepancies(checksums, checksumsResponse).c_str());
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, listComparison(checksums, checksumsResponse, true).c_str());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "tes3mp", errmsg.c_str(), 0);
        connected = false;
    }
}

void Networking::receiveMessage(RakNet::Packet *packet)
{
    if (packet->length < 2)
        return;

    if (playerPacketController.ContainsPacket(packet->data[0]))
    {
        if (!PlayerProcessor::Process(*packet))
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Unhandled PlayerPacket with identifier %i has arrived", packet->data[0]);
    }
    else if (actorPacketController.ContainsPacket(packet->data[0]))
    {
        if (!ActorProcessor::Process(*packet, actorList))
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Unhandled ActorPacket with identifier %i has arrived", packet->data[0]);
    }
    else if (objectPacketController.ContainsPacket(packet->data[0]))
    {
        if (!ObjectProcessor::Process(*packet, objectList))
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Unhandled ObjectPacket with identifier %i has arrived", packet->data[0]);
    }
    else if (worldstatePacketController.ContainsPacket(packet->data[0]))
    {
        if (!WorldstateProcessor::Process(*packet, worldstate))
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_WARN, "Unhandled WorldstatePacket with identifier %i has arrived", packet->data[0]);
    }
}

PlayerPacket *Networking::getPlayerPacket(RakNet::MessageID id)
{
    return playerPacketController.GetPacket(id);
}

ActorPacket *Networking::getActorPacket(RakNet::MessageID id)
{
    return actorPacketController.GetPacket(id);
}

ObjectPacket *Networking::getObjectPacket(RakNet::MessageID id)
{
    return objectPacketController.GetPacket(id);
}

WorldstatePacket *Networking::getWorldstatePacket(RakNet::MessageID id)
{
    return worldstatePacketController.GetPacket(id);
}

LocalPlayer *Networking::getLocalPlayer()
{
    return mwmp::Main::get().getLocalPlayer();
}

ActorList *Networking::getActorList()
{
    return &actorList;
}

ObjectList *Networking::getObjectList()
{
    return &objectList;
}

Worldstate *Networking::getWorldstate()
{
    return &worldstate;
}

bool Networking::isConnected()
{
    return connected;
}
