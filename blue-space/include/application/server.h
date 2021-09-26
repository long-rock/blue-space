#pragma once

#include "rpc/server.h"
#include "rest.h"

#include <memory>
#include <string_view>

namespace application
{

class Listener;
class Session;

class Server : public std::enable_shared_from_this<Server>
{
  public:
    using Ptr = std::shared_ptr<Server>;

    Server(rpc::Server::Ptr rpc, rest::Server::Ptr rest);

    void start(unsigned short port);

  private:
    void handle_request(const std::string &path, std::string_view request, std::shared_ptr<Session> session);

    rpc::Server::Ptr rpc_;
    rest::Server::Ptr rest_;
    std::shared_ptr<Listener> listener_;
};
} // namespace application