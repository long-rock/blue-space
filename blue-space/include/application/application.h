#pragma once

#include "server.h"
#include "rpcserver.h"

#include <memory>

namespace application
{
class Application
{
  public:
    Application(RpcServer::Ptr rpc);

    void start();

  private:
    RpcServer::Ptr rpc_;
    Server::Ptr server_;
};
} // namespace application