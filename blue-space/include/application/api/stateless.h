#pragma once

#include "miner/common/miner.h"

#include <memory>
#include <string>

namespace application::api
{

class StatelessApi
{
  public:
    using Ptr = std::shared_ptr<StatelessApi>;

    StatelessApi(miner::common::Miner::Ptr miner) : miner_(miner)
    {
    }

    std::vector<miner::common::PlanetLocation> mine_single(int64_t x, int64_t y, int64_t size, int64_t rarity,
                                                           int64_t key);

  private:
    miner::common::Miner::Ptr miner_;
};
} // namespace application::api