#pragma once

#include <cstdint>
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
};

struct WorkItem
{
    int64_t x;
    int64_t y;
    bool is_planet;
    std::string hash;
};

class Miner
{
  public:
    virtual ~Miner() = default;
    virtual void mine_batch(std::vector<WorkItem> &items, uint32_t rarity, uint32_t key) const = 0;
};

} // namespace common
} // namespace miner