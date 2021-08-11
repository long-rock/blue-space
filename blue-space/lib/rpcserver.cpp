#include "application/rpcserver.h"

#include <boost/log/trivial.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/stream.h>
#include <rapidjson/writer.h>

#include <string_view>
#include <vector>

using namespace application;

void RpcServer::handle_request(std::string_view request, const ResponseHandler &cb)
{
    if (request.size() == 0)
    {
        return;
    }

    // rapidjson has no support for string_view
    rapidjson::GenericStringRef<char> request_ref(request.data(), request.size());
    rapidjson::Document doc;
    doc.Parse(request_ref);

    BOOST_LOG_TRIVIAL(info) << "handle request" << request;

    // int64_t rarity = 16384;
    // int64_t key = 420;
    // uint32_t chunk_size = 256;
    // int64_t x = -128;
    // int64_t y = -128;
    int64_t rarity = doc["planetRarity"].GetInt64();
    int64_t key = doc["planetHashKey"].GetInt64();

    auto req_chunk_footprint = doc["chunkFootprint"].GetObject();
    std::size_t chunk_size = req_chunk_footprint["sideLength"].GetUint();
    auto req_bottom_left = req_chunk_footprint["bottomLeft"].GetObject();
    int64_t x = req_bottom_left["x"].GetInt64();
    int64_t y = req_bottom_left["y"].GetInt64();

    std::vector<miner::common::WorkItem> batch;

    for (std::size_t i = 0; i < chunk_size; ++i)
    {
        for (std::size_t j = 0; j < chunk_size; ++j)
        {
            miner::common::WorkItem item;
            item.x = x + i;
            item.y = y + j;
            batch.push_back(item);
        }
    }

    miner_->mine_batch(batch, rarity, key);

    rapidjson::Document res(rapidjson::kObjectType);

    rapidjson::Value planets(rapidjson::kArrayType);
    auto &res_alloc = res.GetAllocator();

    rapidjson::Value chunk_footprint(rapidjson::kObjectType);
    chunk_footprint.AddMember("sideLength", chunk_size, res_alloc);

    rapidjson::Value bottom_left(rapidjson::kObjectType);
    bottom_left.AddMember("x", x, res_alloc);
    bottom_left.AddMember("y", y, res_alloc);
    chunk_footprint.AddMember("bottomLeft", bottom_left, res_alloc);

    res.AddMember("chunkFootprint", chunk_footprint, res_alloc);

    for (const auto &item: batch)
    {
        if (item.is_planet)
        {
            rapidjson::Value planet(rapidjson::kObjectType);
            rapidjson::Value coords(rapidjson::kObjectType);

            coords.AddMember("x", item.x, res_alloc);
            coords.AddMember("y", item.y, res_alloc);
            planet.AddMember("coords", coords, res_alloc);

            rapidjson::Value hash(item.hash, res_alloc);
            planet.AddMember("hash", hash, res_alloc);

            planets.PushBack(planet, res_alloc);
        }
    }

    res.AddMember("planetLocations", planets, res_alloc);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    res.Accept(writer);
    cb(buffer.GetString());
}