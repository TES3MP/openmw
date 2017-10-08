//
// Created by koncord on 04.09.17.
//

#include "AdminRest.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "RestUtils.hpp"

using namespace std;
using namespace chrono;
using namespace boost::property_tree;


AdminRest::AdminRest(const std::string &cert, const std::string &key, const std::string &verifyFile,
                     unsigned short port, std::shared_ptr<MasterServer> master) : httpServer(cert, key, verifyFile), master(master)
{
    httpServer.config.port = port;
}

void AdminRest::start()
{
    static const string AdminArea = "^/api/admin?";

    httpServer.resource[AdminArea]["POST"] = [this](auto response, auto request) {
        cout << request->method << endl;
        cout << request->path << endl;
        cout << request->http_version << endl;

        for (auto &header : request->header)
            cout << header.first << ": " << header.second << endl;

        string resp;
        master->luaStuff([&request, &response, &resp](sol::state &state) {
            sol::protected_function func = state["OnAdminRequest"];

            sol::protected_function_result result = func.call(request->remote_endpoint_address, request->content.string());
            if (result.valid())
                *response << result.get<string>();
            else
            {
                cerr << "Error: " << result.get<string>() << endl;
                *response << response500;
            }
        });

    };

    /*httpServer.on_error = [](auto request, const boost::system::error_code& err)
    {
        std::cerr << "Error: " << err.message() << " " << err.category().name() << std::endl;
    };*/

    httpServer.default_resource["GET"] = [](auto response, auto /*request*/) {
        cout << "Default request" << endl;
        *response << response400;
    };

    thr = thread([this](){httpServer.start();});
}

void AdminRest::stop()
{
    httpServer.stop();
    if(thr.joinable())
        thr.join();
}
