#pragma once

#include "miner/common/miner.h"

#include <jsonrpc-lean/server.h>

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace application::rpc
{
class Server
{
  public:
    using Ptr = std::shared_ptr<Server>;
    using ResponseHandler = std::function<void(const std::string &)>;

    Server();
    void initialize();
    void register_method(const std::string &name, jsonrpc::MethodWrapper::Method method);

    void handle_request(const std::string &path, std::string_view request, const ResponseHandler &cb);

  private:
    jsonrpc::Server rpc_server_;
    jsonrpc::JsonFormatHandler json_format_handler_;
};
} // namespace application