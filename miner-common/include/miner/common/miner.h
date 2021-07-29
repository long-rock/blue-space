#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace miner
{
namespace common
{

struct Planet
{
    int64_t x;
    int64_t y;
    std::string hash;
};

class Miner
{
  public:
    virtual ~Miner() = default;
    virtual std::vector<Planet> mine_batch(int64_t x, int64_t y, uint32_t size, uint32_t rarity,
                                           uint32_t key) const = 0;
};

} // namespace common
} // namespace miner