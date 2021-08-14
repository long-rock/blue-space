#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace miner
{
namespace common
{

struct Coordinate
{
    int64_t x;
    int64_t y;

    Coordinate() : x(0), y(0)
    {
    }

    Coordinate(int64_t x, int64_t y) : x(x), y(y)
    {
    }

    Coordinate(const Coordinate &) = default;
    Coordinate(Coordinate &&) = default;
};

struct PlanetLocation
{
    Coordinate coordinate;
    std::string hash;

    PlanetLocation(Coordinate coordinate_, std::string hash_)
        : coordinate(std::move(coordinate_)), hash(std::move(hash_))
    {
    }
};

struct ChunkFootprint
{
    Coordinate bottom_left;
    uint32_t side_length;

    ChunkFootprint(Coordinate bottom_left_, uint32_t side_length_)
        : bottom_left(std::move(bottom_left_)), side_length(side_length_)
    {
    }
};

class Miner
{
  public:
    using Ptr = std::shared_ptr<Miner>;

    virtual ~Miner() = default;
    virtual void mine(const ChunkFootprint &chunk, uint32_t rarity, uint32_t key,
                      std::vector<PlanetLocation> &result) = 0;
};

} // namespace common
} // namespace miner