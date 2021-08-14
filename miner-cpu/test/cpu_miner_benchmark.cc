#include "miner/common/miner.h"
#include "miner/cpu/miner.h"

#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstdint>
#include <iostream>

const int64_t DEFAULT_SIZE = 256;
const uint64_t RARITY = 16384;
const uint64_t KEY = 420;

static void cpu_miner_benchmark(benchmark::State &state)
{
    auto miner = miner::cpu::CpuMiner(miner::cpu::CpuMinerOptions());

    std::vector<miner::common::PlanetLocation> planets;
    miner::common::Coordinate bottom_left(-128, 128);
    miner::common::ChunkFootprint chunk(bottom_left, 256);

    for (auto _ : state)
    {
        miner.mine(chunk, RARITY, KEY, planets);
    }
}

BENCHMARK(cpu_miner_benchmark);