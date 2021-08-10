#include "miner/cpu/miner.h"

#include "hash.h"
#include "miner/common/constants.h"

#include <gmpxx.h>
#include <omp.h>

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

miner::cpu::CpuMiner::CpuMiner(const CpuMinerOptions &options) : options_(options)
{
}

miner::cpu::CpuMiner::~CpuMiner()
{
}

void miner::cpu::CpuMiner::mine_batch(std::vector<common::WorkItem> &items, uint32_t rarity, uint32_t key) const
{
    mpz_class key_(key);
    mpz_class rarity_(rarity);
    mpz_class threshold = P / rarity_;

    Sponge sponge;
    if (options_.num_threads > 0)
    {
        omp_set_num_threads(options_.num_threads);
    }

    #pragma omp parallel for default(none) shared(key_, threshold, items) private(sponge)
    for (auto &work_item : items)
    {
        mpz_class planet_hash;
        mpz_class x_ = internal::wrap_coordinate(work_item.x);
        mpz_class y_ = internal::wrap_coordinate(work_item.y);
        miner::cpu::mimc_hash(sponge, planet_hash.get_mpz_t(), x_.get_mpz_t(), y_.get_mpz_t(), key_.get_mpz_t());
        auto is_planet = miner::cpu::is_planet(planet_hash.get_mpz_t(), threshold.get_mpz_t());
        work_item.hash = planet_hash.get_str();
        work_item.is_planet = is_planet;
    }
}