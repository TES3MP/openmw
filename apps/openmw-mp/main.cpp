#include <iostream>

#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream_buffer.hpp>

#include <components/files/configurationmanager.hpp>
#include <components/files/escape.hpp>
#include <components/settings/settings.hpp>
#include <components/version/version.hpp>

#include <components/openmw-mp/Log.hpp>
#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/Version.hpp>

#include <BitStream.h>
#include <MessageIdentifiers.h>
#include <RakPeer.h>
#include <RakPeerInterface.h>

#include "Player.hpp"
#include "Networking.hpp"
#include "MasterClient.hpp"
#include "Utils.hpp"

#ifdef ENABLE_BREAKPAD
#include <handler/exception_handler.h>
#endif

using namespace std;
using namespace mwmp;

#ifdef ENABLE_BREAKPAD
google_breakpad::ExceptionHandler *pHandler = 0;
#if defined(_WIN32)
bool DumpCallback(const wchar_t* _dump_dir,const wchar_t* _minidump_id,void* context,EXCEPTION_POINTERS* exinfo,MDRawAssertionInfo* assertion,bool success)
#elif defined(__linux)
bool DumpCallback(const google_breakpad::MinidumpDescriptor &md, void *context, bool success)
#endif
{
    // NO STACK USE, NO HEAP USE THERE !!!
    return success;
}

void breakpad(std::string pathToDump)
{
#ifdef _WIN32
    pHandler = new google_breakpad::ExceptionHandler(
            L"crashdumps\\",
            /*FilterCallback*/ 0,
            DumpCallback,
            0,
            google_breakpad::ExceptionHandler::HANDLER_ALL);
#else
    google_breakpad::MinidumpDescriptor md(pathToDump);
    pHandler = new google_breakpad::ExceptionHandler(
            md,
            /*FilterCallback*/ 0,
            DumpCallback,
            /*context*/ 0,
            true,
            -1
    );
#endif
}

void breakpad_close()
{
    delete pHandler;
}
#else
void breakpad(std::string pathToDump){}
void breakpad_close(){}
#endif

std::string loadSettings (Settings::Manager & settings)
{
    Files::ConfigurationManager mCfgMgr;
    // Create the settings manager and load default settings file
    const std::string localdefault = (mCfgMgr.getLocalPath() / "tes3mp-server-default.cfg").string();
    const std::string globaldefault = (mCfgMgr.getGlobalPath() / "tes3mp-server-default.cfg").string();

    // prefer local
    if (boost::filesystem::exists(localdefault))
        settings.loadDefault(localdefault);
    else if (boost::filesystem::exists(globaldefault))
        settings.loadDefault(globaldefault);
    else
        throw std::runtime_error ("No default settings file found! Make sure the file \"tes3mp-server-default.cfg\" was properly installed.");

    // load user settings if they exist
    const std::string settingspath = (mCfgMgr.getUserConfigPath() / "tes3mp-server.cfg").string();
    if (boost::filesystem::exists(settingspath))
        settings.loadUser(settingspath);

    return settingspath;
}

class Tee : public boost::iostreams::sink
{
public:
    Tee(std::ostream &stream, std::ostream &stream2)
            : out(stream), out2(stream2)
    {
    }

    std::streamsize write(const char *str, std::streamsize size)
    {
        out.write (str, size);
        out.flush();
        out2.write (str, size);
        out2.flush();
        return size;
    }

private:
    std::ostream &out;
    std::ostream &out2;
};

boost::program_options::variables_map launchOptions(int argc, char *argv[], Files::ConfigurationManager cfgMgr)
{
    namespace bpo = boost::program_options;
    bpo::variables_map variables;
    bpo::options_description desc;

    desc.add_options()
            ("resources", bpo::value<Files::EscapeHashString>()->default_value("resources"), "set resources directory")
            ("no-logs", bpo::value<bool>()->implicit_value(true)->default_value(false),
             "Do not write logs. Useful for daemonizing.");

    cfgMgr.readConfiguration(variables, desc, true);

    bpo::parsed_options valid_opts = bpo::command_line_parser(argc, argv).options(desc).allow_unregistered().run();

    bpo::store(valid_opts, variables);
    bpo::notify(variables);

    return variables;
}

int main(int argc, char *argv[])
{
    Settings::Manager mgr;
    Files::ConfigurationManager cfgMgr;

    breakpad(boost::filesystem::path(cfgMgr.getLogPath()).string());

    loadSettings(mgr);

    auto variables = launchOptions(argc, argv, cfgMgr);

    auto version = Version::getOpenmwVersion(variables["resources"].as<Files::EscapeHashString>().toStdString());

    int logLevel = mgr.getInt("logLevel", "General");
    if (logLevel < Log::LOG_VERBOSE || logLevel > Log::LOG_FATAL)
        logLevel = Log::LOG_VERBOSE;

    // Some objects used to redirect cout and cerr
    // Scope must be here, so this still works inside the catch block for logging exceptions
    std::streambuf* cout_rdbuf = std::cout.rdbuf ();
    std::streambuf* cerr_rdbuf = std::cerr.rdbuf ();

    boost::iostreams::stream_buffer<Tee> coutsb;
    boost::iostreams::stream_buffer<Tee> cerrsb;

    std::ostream oldcout(cout_rdbuf);
    std::ostream oldcerr(cerr_rdbuf);

    boost::filesystem::ofstream logfile;

    if (!variables["no-logs"].as<bool>())
    {
        // Redirect cout and cerr to tes3mp server log

        logfile.open(boost::filesystem::path(
                cfgMgr.getLogPath() / "/tes3mp-server-" += Log::getFilenameTimestamp() += ".log"));

        coutsb.open(Tee(logfile, oldcout));
        cerrsb.open(Tee(logfile, oldcerr));

        std::cout.rdbuf(&coutsb);
        std::cerr.rdbuf(&cerrsb);
    }

    LOG_INIT(logLevel);

    int players = mgr.getInt("maximumPlayers", "General");
    string addr = mgr.getString("localAddress", "General");
    int port = mgr.getInt("port", "General");

    string passw = mgr.getString("password", "General");

    string plugin_home = mgr.getString("home", "Plugins");
    string moddir = Utils::convertPath(plugin_home + "/data");

    vector<string> plugins (Utils::split(mgr.getString("plugins", "Plugins"), ','));

    Utils::printVersion("TES3MP dedicated server", TES3MP_VERSION, version.mCommitHash, TES3MP_PROTO_VERSION);

    int code;

    RakNet::RakPeerInterface *peer = RakNet::RakPeerInterface::GetInstance();

    stringstream sstr;
    sstr << TES3MP_VERSION;
    sstr << TES3MP_PROTO_VERSION;
    sstr << version.mCommitHash;

    peer->SetIncomingPassword(sstr.str().c_str(), (int) sstr.str().size());

    if (RakNet::NonNumericHostString(addr.c_str()))
    {
        LOG_MESSAGE_SIMPLE(Log::LOG_ERROR, "You cannot use non-numeric addresses for the server.");
        return 1;
    }

    RakNet::SocketDescriptor sd((unsigned short) port, addr.c_str());

    //try
    {

        switch (peer->Startup((unsigned) players, &sd, 1))
        {
            case RakNet::RAKNET_STARTED:
                break;
            case RakNet::RAKNET_ALREADY_STARTED:
                throw runtime_error("Already started");
            case RakNet::INVALID_SOCKET_DESCRIPTORS:
                throw runtime_error("Incorrect port or address");
            case RakNet::INVALID_MAX_CONNECTIONS:
                throw runtime_error("Max players cannot be negative or 0");
            case RakNet::SOCKET_FAILED_TO_BIND:
            case RakNet::SOCKET_PORT_ALREADY_IN_USE:
            case RakNet::PORT_CANNOT_BE_ZERO:
                throw runtime_error("Failed to bind port");
            case RakNet::SOCKET_FAILED_TEST_SEND:
            case RakNet::SOCKET_FAMILY_NOT_SUPPORTED:
            case RakNet::FAILED_TO_CREATE_NETWORK_THREAD:
            case RakNet::COULD_NOT_GENERATE_GUID:
            case RakNet::STARTUP_OTHER_FAILURE:
                throw runtime_error("Cannot start server");
        }

        peer->SetMaximumIncomingConnections((unsigned short) (players));

        Networking networking(peer);
        networking.setServerPassword(passw);

        if (mgr.getBool("enabled", "MasterServer"))
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sharing server query info to master enabled.");
            string masterAddr = mgr.getString("address", "MasterServer");
            int masterPort = mgr.getInt("port", "MasterServer");
            int updateRate = mgr.getInt("rate", "MasterServer");

            networking.InitQuery(masterAddr, (unsigned short) masterPort);
            networking.getMasterClient()->SetMaxPlayers((unsigned) players);
            networking.getMasterClient()->SetUpdateRate((unsigned) updateRate);
            string hostname = mgr.getString("hostname", "General");
            networking.getMasterClient()->SetHostname(hostname);
            networking.getMasterClient()->SetRuleString("CommitHash", version.mCommitHash.substr(0, 10));

            networking.getMasterClient()->Start();
        }

        networking.postInit();

        code = networking.mainLoop();

        networking.getMasterClient()->Stop();
    }
    /*catch (std::exception &e)
    {
        LOG_MESSAGE_SIMPLE(Log::LOG_ERROR, e.what());
        throw; //fall through
    }*/

    RakNet::RakPeerInterface::DestroyInstance(peer);

    if (code == 0)
        LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Quitting peacefully.");
    else if (code == 1)
        LOG_MESSAGE_SIMPLE(Log::LOG_ERROR, "Forcibly shutting down because of error.");

    LOG_QUIT();

    if (!variables["no-logs"].as<bool>())
    {
        // Restore cout and cerr
        std::cout.rdbuf(cout_rdbuf);
        std::cerr.rdbuf(cerr_rdbuf);
    }


    breakpad_close();
    return code;
}
