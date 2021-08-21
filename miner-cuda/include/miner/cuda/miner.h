#pragma once

#include "miner/common/miner.h"

#include <cstdint>
#include <string>
#include <vector>

namespace miner
{
namespace cuda
{

enum class ThreadsPerItem
{
  TPI_4 = 4,
  TPI_8 = 8,
  TPI_16 = 16,
  TPI_32 = 32,
};

struct CudaMinerOptions
{
    uint32_t thread_work_size;
    uint32_t block_size;
    ThreadsPerItem threads_per_item;

    CudaMinerOptions() : thread_work_size(64), block_size(16), threads_per_item(ThreadsPerItem::TPI_16)
    {
    }

    CudaMinerOptions(uint32_t ws, uint32_t bs, ThreadsPerItem tpi) : thread_work_size(ws), block_size(bs), threads_per_item(tpi)
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