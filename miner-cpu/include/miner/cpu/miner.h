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
    void mine_batch(std::vector<common::WorkItem> &items, uint32_t rarity, uint32_t key) const override;
};

} // namespace cpu
} // namespace miner