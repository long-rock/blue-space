#pragma once

#include "api/stateless.h"
#include "rpc/server.h"
#include "server.h"

#include "miner/common/miner.h"

#include <memory>

namespace application
{
class Application
{
  public:
    Application();

    void initialize(miner::common::Miner::Ptr miner);

    void start();

  private:
    api::StatelessApi::Ptr stateless_;
    rpc::Server::Ptr rpc_;
    Server::Ptr server_;
};
} // namespace application