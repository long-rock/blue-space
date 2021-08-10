#pragma once

#include "miner/common/miner.h"

#include <cstdint>
#include <string>
#include <vector>

namespace miner
{
namespace cuda
{

class CudaMiner : public common::Miner
{
  public:
    CudaMiner(int device);
    ~CudaMiner();
    void mine_batch(std::vector<common::WorkItem> &items, uint32_t rarity, uint32_t key) const override;

  private:
    int device_;
};

} // namespace cuda
} // namespace miner