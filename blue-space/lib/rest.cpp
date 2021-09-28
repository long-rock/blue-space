#include "application/rest.h"

#include <sstream>

#include <boost/core/ignore_unused.hpp>
#include <boost/log/trivial.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace miner::common;
using namespace application::rest;
namespace pt = boost::property_tree;

namespace internal
{
pt::ptree planet_location_json(const PlanetLocation &planet)
{
    pt::ptree r;
    r.put("coords.x", planet.coordinate.x);
    r.put("coords.y", planet.coordinate.y);
    r.put("hash", planet.hash);
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

    std::istringstream request_stream(std::string(request), std::ios_base::in);
    pt::ptree request_json;
    pt::read_json(request_stream, request_json);

    auto side_length = request_json.get<int64_t>("chunkFootprint.sideLength");
    auto x = request_json.get<int64_t>("chunkFootprint.bottomLeft.x");
    auto y = request_json.get<int64_t>("chunkFootprint.bottomLeft.y");
    auto planet_rarity = request_json.get<int64_t>("planetRarity");
    auto planet_hash_key = request_json.get<int64_t>("planetHashKey");

    auto planets = api_->mine_single(x, y, side_length, planet_rarity, planet_hash_key);

    pt::ptree response_json;
    response_json.put("chunkFootprint.sideLength", side_length);
    response_json.put("chunkFootprint.bottomLeft.x", x);
    response_json.put("chunkFootprint.bottomLeft.y", y);

    pt::ptree planets_json;
    for (auto &planet : planets)
    {
        auto p = internal::planet_location_json(planet);
        planets_json.push_back(std::make_pair("", p));
    }
    response_json.add_child("planetLocations", planets_json);

    std::ostringstream response_stream(std::ios_base::out);
    pt::write_json(response_stream, response_json);
    cb(response_stream.str());
}
