#pragma once

#include "storage.h"

#include "miner/common/miner.h"

#include <cstdint>
#include <optional>

namespace explorer
{
class Explorer
{
  public:
    Explorer(Storage::ptr storage, miner::common::Coordinate origin);
    virtual ~Explorer() = default;
    void set_origin(miner::common::Coordinate coord);
    miner::common::Coordinate origin() const
    {
        return origin_;
    }
    Storage::ptr storage() const {
        return storage_;
    }
    virtual std::optional<miner::common::Coordinate> next() = 0;

  private:
    Storage::ptr storage_;
    miner::common::Coordinate origin_;
};

class SpiralExplorer : public Explorer
{
  public:
    SpiralExplorer(Storage::ptr storage, miner::common::Coordinate origin);
    std::optional<miner::common::Coordinate> next() override;

  private:
    miner::common::Coordinate next_coordinate();
    miner::common::Coordinate current_;
};

} // namespace explorer