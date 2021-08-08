#pragma once

#include "miner/common/miner.h"

#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>

namespace explorer
{
namespace internal
{
struct CoordinateHash
{
    std::size_t operator()(const miner::common::Coordinate &coord) const
    {
        return coord.x ^ coord.y;
    }
};

struct CoordinateEqual
{
    std::size_t operator()(const miner::common::Coordinate &c0, const miner::common::Coordinate &c1) const
    {
        return c0.x == c1.x && c0.y == c1.y;
    }
};
} // namespace internal

class Storage
{
  public:
    using ptr = std::shared_ptr<Storage>;

    virtual ~Storage() = default;

    virtual std::optional<miner::common::WorkItem> get(const miner::common::Coordinate &coord) const = 0;
    virtual void store(miner::common::WorkItem item) = 0;
};

class InMemoryStorage : public Storage
{
  public:
    InMemoryStorage();
    ~InMemoryStorage() = default;

    std::optional<miner::common::WorkItem> get(const miner::common::Coordinate &coord) const override;
    void store(miner::common::WorkItem item) override;

  private:
    using ExploredMap = std::unordered_map<miner::common::Coordinate, miner::common::WorkItem, internal::CoordinateHash, internal::CoordinateEqual>;
    ExploredMap explored_;
};

} // namespace explorer