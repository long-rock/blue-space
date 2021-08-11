#pragma once

#include "rpcserver.h"

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

    Server(RpcServer::Ptr rpc);

    void start();

  private:
    void handle_request(std::string_view request, std::shared_ptr<Session> session);

    RpcServer::Ptr rpc_;
    std::shared_ptr<Listener> listener_;
};
} // namespace application