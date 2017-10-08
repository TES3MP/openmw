//
// Created by koncord on 21.04.17.
//

#ifndef NEWMASTERPROTO_MASTERSERVER_HPP
#define NEWMASTERPROTO_MASTERSERVER_HPP

#include <thread>
#include <chrono>
#include <RakPeerInterface.h>
#include <components/openmw-mp/Master/MasterData.hpp>
#include <extern/sol/sol.hpp>
#include <mutex>

class MasterServer
{
public:
    struct SServer : QueryData
    {
        std::chrono::steady_clock::time_point lastUpdate;
    };
    typedef std::map<RakNet::SystemAddress, SServer> ServerMap;
    //typedef ServerMap::const_iterator ServerCIter;
    typedef ServerMap::iterator ServerIter;

    explicit MasterServer(const std::string &luaScript);
    ~MasterServer();

    void Start();
    void Stop(bool wait = false);
    bool isRunning();
    void Wait();

    ServerMap* GetServers();
    void luaStuff(std::function<void(sol::state &)> f);

    void ban(const std::string &addr);
    void unban(const std::string &addr);

private:
    void Thread();

private:
    std::thread tMasterThread;
    RakNet::RakPeerInterface* peer;
    RakNet::SocketDescriptor sockdescr;
    ServerMap servers;
    bool run;
    std::map<RakNet::RakNetGUID, std::chrono::steady_clock::time_point> pendingACKs;
    sol::state state;
    std::mutex luaMutex;
    std::mutex banMutex;
};


#endif //NEWMASTERPROTO_MASTERSERVER_HPP
