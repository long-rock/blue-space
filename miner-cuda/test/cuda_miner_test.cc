#include "miner/common/miner.h"
#include "miner/cuda/miner.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>

const int64_t DEFAULT_SIZE = 256;
const uint64_t RARITY = 16384;
const uint64_t KEY = 420;

using namespace miner::common;
using namespace miner::cuda;

TEST(CpuMinerTest, FindsPlanets)
{
    auto options = CudaMinerOptions();
    //options.thread_work_size = 16;
    options.thread_work_size = 128;
    options.block_size = 8;
    //options.thread_work_size = 256;
    auto miner = miner::cuda::CudaMiner(0, options);
    // values computed with darkforest-rs
    std::vector<PlanetLocation> expected_planets = {
        PlanetLocation(Coordinate(-81, 13), "940730834903647137929381122188788642093437120345760373357871377071349968"),
        PlanetLocation(Coordinate(-40, -65), "571181713569563774710759574345761362877802556246966615401948128970981398"),
        PlanetLocation(Coordinate(-37, -47), "119067170509170773348244389443884923635208868716853804680619018353327908"),
        PlanetLocation(Coordinate(122, 35), "580522171046983233959685904700229466777380943669338457542254606119083578"),
    };

    ChunkFootprint chunk(Coordinate(-DEFAULT_SIZE / 2, -DEFAULT_SIZE / 2), DEFAULT_SIZE);
    std::vector<PlanetLocation> planets;

    miner.mine(chunk, RARITY, KEY, planets);

    ASSERT_EQ(expected_planets.size(), planets.size());

    // planets might be in different order than expected
    for (auto &planet : expected_planets)
    {
        auto computed_planet = std::find_if(planets.begin(), planets.end(), [&planet](const PlanetLocation &p) {
            return p.coordinate.x == planet.coordinate.x && p.coordinate.y == planet.coordinate.y &&
                   p.hash == planet.hash;
        });
        ASSERT_NE(computed_planet, planets.end());
    }
}
