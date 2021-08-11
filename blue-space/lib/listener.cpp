#include "listener.h"

#include "session.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <boost/log/trivial.hpp>

namespace beast = boost::beast;

using namespace application;

Listener::Listener(std::shared_ptr<boost::asio::io_context> ioc, Options options) : ioc_(ioc), options_(options)
{
}

void Listener::initialize()
{
    beast::error_code ec;

    acceptor_ = std::make_unique<Acceptor>(*ioc_, options_.endpoint);
    acceptor_->set_option(boost::asio::socket_base::reuse_address(true), ec);
    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "failed to set beast reuse address to true";
        return;
    }
}

void Listener::set_new_session_handler(NewSessionHandler &&handler)
{
    on_new_session_ = std::make_unique<NewSessionHandler>(std::move(handler));
}

void Listener::start()
{
    BOOST_ASSERT(acceptor_);
    if (!acceptor_->is_open())
    {
        BOOST_LOG_TRIVIAL(error) << "starting acceptor that is not open";
        return;
    }
    accept_one();
}

void Listener::accept_one()
{
    new_session_ = std::make_shared<Session>(*ioc_);

    auto on_accept = [wp = weak_from_this()](beast::error_code ec) {
        BOOST_LOG_TRIVIAL(debug) << "accepted connection";
        if (auto self = wp.lock())
        {
            if (not ec)
            {
                BOOST_LOG_TRIVIAL(debug) << "starting new session";
                if (self->on_new_session_)
                {
                    (*self->on_new_session_)(self->new_session_);
                }
                self->new_session_->start();
            }

            if (self->acceptor_->is_open())
            {
                BOOST_LOG_TRIVIAL(debug) << "accepting new connection";
                self->accept_one();
            }
        }
    };

    acceptor_->async_accept(new_session_->socket(), std::move(on_accept));
}