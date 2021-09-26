#include "application/rest.h"

#include <boost/core/ignore_unused.hpp>
#include <boost/json/src.hpp>
#include <boost/log/trivial.hpp>

using namespace miner::common;
using namespace application::rest;

namespace internal
{
boost::json::value planet_location_json(const PlanetLocation &planet)
{
    boost::json::object r;
    boost::json::object c;
    c["x"] = planet.coordinate.x;
    c["y"] = planet.coordinate.y;
    r["coords"] = c;
    r["hash"] = boost::json::string(planet.hash);
    return r;
}

} // namespace internal

Server::Server() : api_()
{
}

void Server::initialize(api::StatelessApi::Ptr api)
{
    api_ = api;
}

void Server::handle_request(const std::string &path, std::string_view request, const ResponseHandler &cb)
{
    boost::ignore_unused(path);
    if (request.size() == 0)
    {
        return;
    }

    boost::json::value request_json = boost::json::parse(std::string(request));

    auto root = request_json.as_object();
    auto chunk_footprint = root["chunkFootprint"].as_object();
    auto bottom_left = chunk_footprint["bottomLeft"].as_object();
    auto side_length = chunk_footprint["sideLength"].as_int64();
    auto x = bottom_left["x"].as_int64();
    auto y = bottom_left["y"].as_int64();
    auto planet_rarity = root["planetRarity"].as_int64();
    auto planet_hash_key = root["planetHashKey"].as_int64();

    auto planets = api_->mine_single(x, y, side_length, planet_rarity, planet_hash_key);

    boost::json::array planet_locations;
    for (auto &planet : planets)
    {
        auto planet_json = internal::planet_location_json(planet);
        planet_locations.emplace_back(planet_json);
    }

    boost::json::object response_root;
    response_root["planetLocations"] = planet_locations;
    response_root["chunkFootprint"] = chunk_footprint;

    std::string response = boost::json::serialize(response_root);
    cb(response);
}