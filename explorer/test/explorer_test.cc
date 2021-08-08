#include "explorer/explorer.h"
#include "explorer/storage.h"

#include "miner/common/miner.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <vector>
#include <tuple>

using namespace explorer;
using namespace miner::common;

TEST(SpiralExplorerTest, NextExploresASpiral)
{
    Coordinate origin(-1, -2);
    auto storage = std::make_shared<InMemoryStorage>();
    SpiralExplorer explorer(storage, origin);

    std::vector<std::tuple<int64_t, int64_t>> expected = {
        {-1, -2}, // origin
        {-1, -1}, // move one up
        {0, -1}, // move right
        {0, -2}, // move down
        {0, -3}, // move down
        {-1, -3}, // move left
        {-2, -3}, // move left
        {-2, -2}, // move up
        {-2, -1}, // move up
        {-2, 0}, // move up
        {-1, 0}, // move right
        {0, 0}, // move right
        {1, 0}, // move right
    };

    for (auto &coord: expected) {
        auto next = explorer.next().value();
        ASSERT_EQ(next.x, std::get<0>(coord));
        ASSERT_EQ(next.y, std::get<1>(coord));
    }
}

TEST(SpiralExplorerTest, SkipsAlreadyExplored)
{
    Coordinate origin(-1, -2);
    auto storage = std::make_shared<InMemoryStorage>();
    SpiralExplorer explorer(storage, origin);

    WorkItem item {
        .x = origin.x,
        .y = origin.y,
        .is_planet = false,
        .hash = ""
    };
    storage->store(item);
    auto next = explorer.next().value();
    // skip origin, since it's already explored
    ASSERT_EQ(next.x, -1);
    ASSERT_EQ(next.y, -1);
}