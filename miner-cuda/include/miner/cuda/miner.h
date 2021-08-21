#pragma once

#include "miner/common/miner.h"

#include <cstdint>
#include <string>
#include <vector>

namespace miner
{
namespace cuda
{

struct CudaMinerOptions
{
    uint32_t thread_work_size;
    uint32_t block_size;

    CudaMinerOptions() : thread_work_size(64), block_size(16)
    {
    }
    CudaMinerOptions(uint32_t ws, uint32_t bs) : thread_work_size(ws), block_size(bs)
    {
    }
};

class CudaMiner : public common::Miner
{
  public:
    CudaMiner(int device, const CudaMinerOptions &options);
    ~CudaMiner();
    void mine(const miner::common::ChunkFootprint &chunk, uint32_t rarity, uint32_t key,
              std::vector<miner::common::PlanetLocation> &result);

  private:
    void initialize();

    int device_;
    CudaMinerOptions options_;
    bool initialized_;
};

} // namespace cuda
} // namespace miner