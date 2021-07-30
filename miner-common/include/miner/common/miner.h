#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace miner
{
namespace common
{

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