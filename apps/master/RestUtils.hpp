//
// Created by koncord on 04.09.17.
//

#pragma once

#include <string>
#include "SimpleWeb/base_server.hpp"

static std::string response201 = "HTTP/1.1 201 Created\r\nContent-Length: 7\r\n\r\nCreated";
static std::string response202 = "HTTP/1.1 202 Accepted\r\nContent-Length: 8\r\n\r\nAccepted";
static std::string response400 = "HTTP/1.1 400 Bad Request\r\nContent-Length: 11\r\n\r\nbad request";

static std::string response403 = "HTTP/1.1 403 Forbidden\r\nContent-Length: 9\r\n\r\nForbidden";
static std::string response500 = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 21\r\n\r\nInternal Server Error";

template <class Protocol>
inline void ResponseStr(std::shared_ptr<typename SimpleWeb::ServerBase<Protocol>::Response> response,
                        std::string content, std::string type = "", std::string code = "200 OK")
{
    *response << "HTTP/1.1 " << code << "\r\n";
    if (!type.empty())
        *response << "Content-Type: " << type <<"\r\n";
    *response << "Content-Length: " << content.length() << "\r\n\r\n" << content;
}
