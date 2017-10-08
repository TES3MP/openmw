#include <iostream>
#include <Kbhit.h>
#include <RakSleep.h>
#include <extern/sol/sol.hpp>
#include "MasterServer.hpp"
#include "RestServer.hpp"
#include "AdminRest.hpp"

using namespace RakNet;
using namespace std;

unique_ptr<RestServer> restServer;
shared_ptr<MasterServer> masterServer;
unique_ptr<AdminRest> restAdminServer;

int main(int argc, char* argv[])
{
    if (argc != 2)
        return 1;

    string luaScript(argv[1]);

    masterServer = make_shared<MasterServer>(luaScript);
    masterServer->luaStuff([](sol::state &state)
    {
        sol::table config = state["config"];
        sol::object restPort = config["restPort"];
        if (restPort.get_type() != sol::type::number)
            throw runtime_error("config.restPort is not correct");


        restServer = make_unique<RestServer>(restPort.as<unsigned short>(), masterServer->GetServers());

        sol::object restAdminCert = config["restAdminCert"];
        if (restAdminCert.get_type() != sol::type::string)
            throw runtime_error("config.restAdminCert is not correct");

        sol::object restAdminKey = config["restAdminKey"];
        if (restAdminKey.get_type() != sol::type::string)
            throw runtime_error("config.restAdminKey is not correct");

        sol::object restAdminVerifyFile = config["restAdminVerifyFile"];
        if (restAdminVerifyFile.get_type() != sol::type::string)
            throw runtime_error("config.restAdminVerifyFile is not correct");

        sol::object restAdminPort = config["restAdminPort"];
        if (restAdminPort.get_type() != sol::type::number)
            throw runtime_error("config.restAdminPort is not correct");

        restAdminServer = make_unique<AdminRest>(restAdminCert.as<string>(), restAdminKey.as<string>(),
                                                 restAdminVerifyFile.as<string>(), restAdminPort.as<unsigned short>(), masterServer);
    });

    auto onExit = [](int /*sig*/){
        restServer->stop();
        restAdminServer->stop();
        masterServer->luaStuff([](sol::state &state) {
            sol::protected_function func = state["OnExit"];
            if (func.valid())
                func.call();
        });
        masterServer->Stop(false);
        masterServer->Wait();
    };

    signal(SIGINT, onExit);
    signal(SIGTERM, onExit);

    masterServer->Start();
    restServer->start();
    restAdminServer->start();
    masterServer->Wait();

    return 0;
}
