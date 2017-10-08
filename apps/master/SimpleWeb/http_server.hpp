#pragma once

#include "base_server.hpp"

namespace SimpleWeb {

    template <class socket_type>
    class Server : public ServerBase<socket_type> {};

    using HTTP = asio::ip::tcp::socket;

    template <>
    class Server<HTTP> : public ServerBase<HTTP> {
    public:
        Server() noexcept : ServerBase<HTTP>::ServerBase(80) {}

    protected:
        void accept() override {
            auto session = std::make_shared<Session>(create_connection(*io_service));

            acceptor->async_accept(*session->connection->socket, [this, session](const error_code &ec) {
                auto lock = session->connection->handler_runner->continue_lock();
                if(!lock)
                    return;

                // Immediately start accepting a new connection (unless io_service has been stopped)
                if(ec != asio::error::operation_aborted)
                    this->accept();

                if(!ec) {
                    asio::ip::tcp::no_delay option(true);
                    error_code ec;
                    session->connection->socket->set_option(option, ec);

                    this->read_request_and_content(session);
                }
                else if(this->on_error)
                    this->on_error(session->request, ec);
            });
        }
    };
} // namespace SimpleWeb
