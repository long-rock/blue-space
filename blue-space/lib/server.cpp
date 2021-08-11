#include "application/server.h"

#include "listener.h"
#include "session.h"

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <boost/log/trivial.hpp>

#include <memory>
#include <string_view>

using namespace application;

using tcp = boost::asio::ip::tcp;

Server::Server(std::shared_ptr<RpcServer> rpc) : rpc_(rpc)
{
}

void Server::start()
{
    auto ioc = std::make_shared<boost::asio::io_context>();
    auto address = boost::asio::ip::make_address("0.0.0.0");
    unsigned short port = 8888;

    BOOST_LOG_TRIVIAL(info) << "Starting http server on " << address << ":" << port;

    Listener::Options listener_options(tcp::endpoint{address, port});

    listener_ = std::make_shared<Listener>(ioc, listener_options);

    listener_->initialize();
    listener_->start();

    auto on_new_session = [wp = weak_from_this()](const Session::Ptr &session) {
        session->set_request_handler([wp](std::string_view request, Session::Ptr session) {
            if (auto self = wp.lock())
            {
                self->handle_request(request, session);
            }
        });
    };

    listener_->set_new_session_handler(std::move(on_new_session));

    ioc->run();
}

void Server::handle_request(std::string_view request, Session::Ptr session)
{
    rpc_->handle_request(request, [&](const std::string &response) {
        session->send_response(response);
    });
}