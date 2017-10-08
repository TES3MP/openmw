//
// Created by koncord on 04.09.17.
//

#pragma once

#include "SimpleWeb/https_server.hpp"
#include "MasterServer.hpp"

typedef SimpleWeb::Server<SimpleWeb::HTTPS> HttpsServer;

class AdminRest
{
public:
    AdminRest(const std::string &cert, const std::string &key, const std::string &verifyFile, unsigned short port, std::shared_ptr<MasterServer> master);
    void start();
    void stop();

private:
    HttpsServer httpServer;
    std::shared_ptr<MasterServer> master;
    std::thread thr;
};


