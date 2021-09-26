#pragma once

#include "application/api/stateless.h"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace application::rest
{
class Server
{
  public:
    using Ptr = std::shared_ptr<Server>;
    using ResponseHandler = std::function<void(const std::string &)>;

    Server();

    void initialize(api::StatelessApi::Ptr api);
    void handle_request(const std::string &path, std::string_view request, const ResponseHandler &cb);

  private:
    api::StatelessApi::Ptr api_;
};
} // namespace application::rest