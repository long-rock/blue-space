#include "miner/cpu/miner.h"

#include "hash.h"
#include "miner/common/constants.h"

#include <boost/assert.hpp>
#include <boost/log/trivial.hpp>
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

CpuMiner::CpuMiner(const CpuMinerOptions &options) : options_(options)
{
}

CpuMiner::~CpuMiner()
{
}

void CpuMiner::mine(const common::ChunkFootprint &chunk, uint32_t rarity, uint32_t key,
                    std::vector<common::PlanetLocation> &result)
{
    mpz_class key_(key);
    mpz_class rarity_(rarity);
    mpz_class threshold = P / rarity_;

    Sponge sponge;
    if (options_.num_threads > 0)
    {
        BOOST_LOG_TRIVIAL(info) << "Set OMP CPU threads to " << options_.num_threads;
        omp_set_num_threads(options_.num_threads);
    }

    miner::cpu::Sponge();
#pragma omp parallel for default(none) shared(chunk, result, threshold, key_) private(sponge)
    for (int64_t x = chunk.bottom_left.x; x < chunk.bottom_left.x + chunk.side_length; ++x)
    {
        mpz_class x_ = internal::wrap_coordinate(x);
        sponge.reset();
        sponge.inject(x_.get_mpz_t());
        sponge.mix(key_.get_mpz_t());
        sponge.save();
        for (int64_t y = chunk.bottom_left.y; y < chunk.bottom_left.y + chunk.side_length; ++y)
        {
            mpz_class planet_hash;
            sponge.restore();
            mpz_class y_ = internal::wrap_coordinate(y);
            sponge.inject(y_.get_mpz_t());
            sponge.mix(key_.get_mpz_t());
            sponge.result(planet_hash.get_mpz_t());
            auto is_planet = miner::cpu::is_planet(planet_hash.get_mpz_t(), threshold.get_mpz_t());
            if (is_planet)
            {
                Coordinate coord(x, y);
                std::string hash = planet_hash.get_str();
                PlanetLocation location(std::move(coord), std::move(hash));
                result.push_back(location);
            }
        }
    }
}