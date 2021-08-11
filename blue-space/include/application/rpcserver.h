#pragma once

#include "miner/common/miner.h"

#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace application
{
class RpcServer
{
  public:
    using Ptr = std::shared_ptr<RpcServer>;
    using ResponseHandler = std::function<void(const std::string &)>;

    RpcServer(miner::common::Miner::Ptr miner) : miner_(miner) {}

    void handle_request(std::string_view request, const ResponseHandler &cb);
  private:
    miner::common::Miner::Ptr miner_;
};
} // namespace application