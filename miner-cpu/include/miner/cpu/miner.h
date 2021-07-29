#pragma once

#include "miner/common/miner.h"

#include <cstdint>
#include <string>
#include <vector>

namespace miner
{
namespace cpu
{

class CpuMiner : public common::Miner
{
  public:
    CpuMiner();
    ~CpuMiner();
    std::vector<common::Planet> mine_batch(int64_t x, int64_t y, uint32_t size, uint32_t rarity,
                                           uint32_t key) const override;
};

} // namespace cpu
} // namespace miner