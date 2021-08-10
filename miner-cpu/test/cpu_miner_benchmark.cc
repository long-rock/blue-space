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

    std::vector<miner::common::WorkItem> work_items;
    for (int64_t x = -DEFAULT_SIZE / 2; x < DEFAULT_SIZE / 2; x++)
    {
        for (int64_t y = -DEFAULT_SIZE / 2; x < DEFAULT_SIZE / 2; x++)
        {
            work_items.push_back(miner::common::WorkItem{.x = x, .y = y});
        }
    }

    for (auto _ : state)
    {
        miner.mine_batch(work_items, RARITY, KEY);
    }
}

BENCHMARK(cpu_miner_benchmark);