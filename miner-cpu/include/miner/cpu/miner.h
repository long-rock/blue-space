#pragma once

#include "miner/common/miner.h"

#include <cstdint>
#include <string>
#include <vector>

namespace miner
{
namespace cpu
{

struct CpuMinerOptions
{
  uint32_t num_threads;

  CpuMinerOptions() : num_threads(0) {}
  CpuMinerOptions(uint32_t n) : num_threads(n) {}
};

class CpuMiner : public common::Miner
{
  public:
    CpuMiner(const CpuMinerOptions &options);
    ~CpuMiner();
    void mine_batch(std::vector<common::WorkItem> &items, uint32_t rarity, uint32_t key) const override;
  private:
    CpuMinerOptions options_;
};

} // namespace cpu
} // namespace miner