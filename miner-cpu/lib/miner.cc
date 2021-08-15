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

    // realloc to appropriate bit size
    hash::realloc_mpz(key_.get_mpz_t());
    hash::realloc_mpz(rarity_.get_mpz_t());
    hash::realloc_mpz(threshold.get_mpz_t());

    if (options_.num_threads > 0)
    {
        BOOST_LOG_TRIVIAL(info) << "Set OMP CPU threads to " << options_.num_threads;
        omp_set_num_threads(options_.num_threads);
    }

    mpz_t x_, y_, planet_hash;
    hash::Sponge sponge;

#pragma omp parallel default(none)                                                                                     \
    shared(chunk, result, threshold, key_) private(sponge, x_, y_, planet_hash)
    {
        // initialize thread private variables once before loop
        hash::init_mpz(x_);
        hash::init_mpz(y_);
        hash::init_mpz(planet_hash);

#pragma omp for
        for (int64_t x = chunk.bottom_left.x; x < chunk.bottom_left.x + chunk.side_length; ++x)
        {
            hash::wrap_coordinate(x_, x);
            sponge.reset();
            sponge.inject(x_);
            sponge.mix(key_.get_mpz_t());
            sponge.save();
            for (int64_t y = chunk.bottom_left.y; y < chunk.bottom_left.y + chunk.side_length; ++y)
            {
                hash::wrap_coordinate(y_, y);
                sponge.restore();
                sponge.inject(y_);
                sponge.mix(key_.get_mpz_t());
                sponge.result(planet_hash);
                auto is_planet = miner::cpu::hash::is_planet(planet_hash, threshold.get_mpz_t());
                if (is_planet)
                {
                    Coordinate coord(x, y);
                    mpz_class hash_temp(planet_hash);
                    std::string hash = hash_temp.get_str();
                    PlanetLocation location(std::move(coord), std::move(hash));
#pragma omp critical
                    {
                        // std vector is not thread safe
                        result.push_back(location);
                    }
                }
            }
        }
    }
}