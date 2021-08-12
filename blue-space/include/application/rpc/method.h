#pragma once

#include <boost/log/trivial.hpp>
#include <jsonrpc-lean/request.h>

#include <memory>

namespace application::rpc
{
template <class RequestType, class Api> class Method
{
  public:
    explicit Method(const std::shared_ptr<Api> &api) : api_(api)
    {
    }

    jsonrpc::Value operator()(const jsonrpc::Request::Parameters &params)
    {
        auto api = api_.lock();
        if (!api)
        {
            throw jsonrpc::Fault("api not available");
        }

        RequestType request(api);

        auto &&result = request.execute(params);

        if (!result)
        {
            throw jsonrpc::Fault(result.error().message());
        }

        return result.value();
    }

  private:
    std::weak_ptr<Api> api_;
};
} // namespace application::rpc