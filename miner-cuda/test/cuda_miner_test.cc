#include "miner/common/miner.h"
#include "miner/cuda/miner.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>

const int64_t DEFAULT_SIZE = 256;
const uint64_t RARITY = 16384;
const uint64_t KEY = 420;

TEST(CpuMinerTest, FindsPlanets)
{
    auto miner = miner::cuda::CudaMiner(0, miner::cuda::CudaMinerOptions());
    // values computed with darkforest-rs
    std::vector<miner::common::WorkItem> expected_planets = {
        {.x = -81,
         .y = 13,
         .is_planet = true,
         .hash = "940730834903647137929381122188788642093437120345760373357871377071349968"},
        {.x = -40,
         .y = -65,
         .is_planet = true,
         .hash = "571181713569563774710759574345761362877802556246966615401948128970981398"},
        {.x = -37,
         .y = -47,
         .is_planet = true,
         .hash = "119067170509170773348244389443884923635208868716853804680619018353327908"},
        {
            .x = 122,
            .y = 35,
            .is_planet = true,
            .hash = "580522171046983233959685904700229466777380943669338457542254606119083578",
        }};

    std::vector<miner::common::WorkItem> work_items;
    for (int64_t x = -DEFAULT_SIZE / 2; x < DEFAULT_SIZE / 2; x++)
    {
        for (int64_t y = -DEFAULT_SIZE / 2; y < DEFAULT_SIZE / 2; y++)
        {
            work_items.push_back(miner::common::WorkItem{.x = x, .y = y});
        }
    }

    miner.mine_batch(work_items, RARITY, KEY);

    uint32_t planet_count = 0;
    for (auto &item: work_items)
    {
        if (item.is_planet) {
            planet_count += 1;
        }
    }

    ASSERT_EQ(planet_count, expected_planets.size());

    // planets might be in different order than expected
    for (auto &planet : expected_planets)
    {
        auto computed_planet =
            std::find_if(work_items.begin(), work_items.end(), [&planet](const miner::common::WorkItem &p) {
                return p.x == planet.x && p.y == planet.y && p.hash == planet.hash;
            });
        ASSERT_NE(computed_planet, work_items.end());
    }
}
