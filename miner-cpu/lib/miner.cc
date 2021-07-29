#include "miner/cpu/miner.h"

#include "hash.h"
#include "miner/common/constants.h"

#include <gmpxx.h>

#include <memory>
#include <vector>

using namespace miner::common;
using namespace miner::cpu;

namespace internal
{

mpz_class wrap_coordinate(int64_t c)
{
    if (c >= 0)
    {
        return mpz_class(c);
    }
    mpz_class t(-c);
    return P - t;
}

} // namespace internal

miner::cpu::CpuMiner::CpuMiner()
{
}

miner::cpu::CpuMiner::~CpuMiner()
{
}

std::vector<Planet> miner::cpu::CpuMiner::mine_batch(int64_t x, int64_t y, uint32_t size, uint32_t rarity,
                                                     uint32_t key) const
{
    std::vector<Planet> planets;
    mpz_class key_(key);
    mpz_class rarity_(rarity);
    mpz_class threshold = P / rarity_;

    Sponge sponge;
#pragma omp parallel for collapse(2) default(none) shared(key_, planets, x, y, size, threshold) private(sponge)
    for (int64_t xi = x; xi < x + size; xi++)
    {
        for (int64_t yi = y; yi < y + size; yi++)
        {
            mpz_class planet_hash;
            mpz_class x_ = internal::wrap_coordinate(xi);
            mpz_class y_ = internal::wrap_coordinate(yi);
            miner::cpu::mimc_hash(sponge, planet_hash.get_mpz_t(), x_.get_mpz_t(), y_.get_mpz_t(), key_.get_mpz_t());
            if (miner::cpu::is_planet(planet_hash.get_mpz_t(), threshold.get_mpz_t()))
            {
                Planet planet{
                    .x = xi,
                    .y = yi,
                    .hash = planet_hash.get_str(),
                };
                planets.push_back(planet);
            }
        }
    }
    return planets;
}