#pragma once

#include "session.h"

#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace application
{
class Listener : public std::enable_shared_from_this<Listener>
{
  public:
    using Ptr = std::shared_ptr<Listener>;
    using NewSessionHandler = std::function<void(const Session::Ptr &session)>;

    using Acceptor = boost::asio::ip::tcp::acceptor;
    using Endpoint = boost::asio::ip::tcp::endpoint;


    struct Options
    {
        Endpoint endpoint{};

        Options()
        {
            endpoint.address(boost::asio::ip::address_v4::any());
            endpoint.port(0);
        }

        Options(const Endpoint &ep) : endpoint(ep)
        {
        }
    };

    Listener(std::shared_ptr<boost::asio::io_context> ioc, Options options);

    void initialize();
    void set_new_session_handler(NewSessionHandler &&handler);
    void start();

  private:
    void accept_one();

    std::shared_ptr<boost::asio::io_context> ioc_;
    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<Session> new_session_;
    std::unique_ptr<NewSessionHandler> on_new_session_;
    Options options_;
};
} // namespace application