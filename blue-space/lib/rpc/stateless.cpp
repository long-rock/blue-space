#include "application/rpc/stateless.h"
#include "application/rpc/method.h"

#include <boost/log/trivial.hpp>
#include <jsonrpc-lean/dispatcher.h>

using namespace application::rpc::stateless;

void application::rpc::stateless::register_methods(rpc::Server::Ptr server, api::StatelessApi::Ptr api)
{
    server->register_method("stateless_mine", Method<MineSingleRequest, api::StatelessApi>(api));
}

MineSingleRequest::MineSingleRequest(api::StatelessApi::Ptr api) : api_(std::move(api))
{
}

namespace {
    jsonrpc::Value::Struct item_to_json(const miner::common::WorkItem &item)
    {
        jsonrpc::Value::Struct r;
        r["x"] = item.x;
        r["y"] = item.y;
        r["hash"] = item.hash;
        return r;
    }
}

outcome::result<jsonrpc::Value::Struct> MineSingleRequest::execute(const jsonrpc::Request::Parameters &params)
{

    // int64_t x, int64_t y, int64_t size, int64_t rarity, int64_t key
    if (params.size() != 5)
    {
        throw jsonrpc::InvalidParametersFault("invalid parameters");
    }

    auto &x_param = params[0];
    if (!(x_param.IsInteger64() || x_param.IsInteger32()))
    {
        throw jsonrpc::InvalidParametersFault("parameter [x] must be an integer");
    }

    auto &y_param = params[1];
    if (!(y_param.IsInteger64() || y_param.IsInteger32()))
    {
        throw jsonrpc::InvalidParametersFault("parameter [y] must be an integer");
    }

    auto &size_param = params[2];
    if (!(size_param.IsInteger64() || size_param.IsInteger32()))
    {
        throw jsonrpc::InvalidParametersFault("parameter [size] must be an integer");
    }

    auto &rarity_param = params[3];
    if (!(rarity_param.IsInteger64() || rarity_param.IsInteger32()))
    {
        throw jsonrpc::InvalidParametersFault("parameter [rarity] must be an integer");
    }

    auto &key_param = params[4];
    if (!(key_param.IsInteger64() || key_param.IsInteger32()))
    {
        throw jsonrpc::InvalidParametersFault("parameter [key] must be an integer");
    }

    auto x = x_param.AsInteger64();
    auto y = y_param.AsInteger64();
    auto size = size_param.AsInteger64();
    auto rarity = rarity_param.AsInteger64();
    auto key = key_param.AsInteger64();

    auto planets = api_->mine_single(x, y, size, rarity, key);

    jsonrpc::Value::Struct result;
    jsonrpc::Value::Array json_planets;
    for (auto &planet: planets)
    {
        auto json_planet = item_to_json(planet);
        json_planets.push_back(json_planet);
    }
    result["planets"] = json_planets;
    return outcome::success(result);
}
