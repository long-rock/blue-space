#include "application/api/stateless.h"

using namespace miner::common;
using namespace application::api;

std::vector<PlanetLocation> StatelessApi::mine_single(int64_t x, int64_t y, int64_t size, int64_t rarity, int64_t key)
{
    std::vector<PlanetLocation> planets;
    ChunkFootprint chunk(Coordinate(x, y), size);

    miner_->mine(chunk, rarity, key, planets);

    return planets;
}