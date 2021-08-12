#pragma once

#include "application/api/stateless.h"
#include "application/rpc/server.h"

#include "miner/common/miner.h"

#include <boost/outcome/outcome.hpp>

#include <jsonrpc-lean/dispatcher.h>

namespace outcome = BOOST_OUTCOME_V2_NAMESPACE;

namespace application::rpc::stateless
{

void register_methods(rpc::Server::Ptr server, api::StatelessApi::Ptr api);

class MineSingleRequest
{
  public:
    MineSingleRequest(api::StatelessApi::Ptr api);
    outcome::result<jsonrpc::Value::Struct> execute(const jsonrpc::Request::Parameters &params);

  private:
    api::StatelessApi::Ptr api_;
};
} // namespace application::rpc::stateless