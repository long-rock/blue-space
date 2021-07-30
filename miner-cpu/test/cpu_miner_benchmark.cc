#include "miner/common/miner.h"
#include "miner/cpu/miner.h"

#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstdint>

const int64_t DEFAULT_SIZE = 256;
const uint64_t RARITY = 16384;
const uint64_t KEY = 420;

static void cpu_miner_benchmark(benchmark::State &state) {
    auto miner = miner::cpu::CpuMiner();
    // values computed with darkforest-rs
    std::vector<miner::common::Planet> expected_planets = {
        // NOLINTNEXTLINE
        {.x = -81, .y = 13, .hash = "940730834903647137929381122188788642093437120345760373357871377071349968"},
        // NOLINTNEXTLINE
        {.x = -40, .y = -65, .hash = "571181713569563774710759574345761362877802556246966615401948128970981398"},
        // NOLINTNEXTLINE
        {.x = -37, .y = -47, .hash = "119067170509170773348244389443884923635208868716853804680619018353327908"},
        // NOLINTNEXTLINE
        {.x = 122, .y = 35, .hash = "580522171046983233959685904700229466777380943669338457542254606119083578"}};

    for (auto _: state) {
        miner.mine_batch(-DEFAULT_SIZE / 2, -DEFAULT_SIZE / 2, DEFAULT_SIZE, RARITY, KEY);
    }
}

BENCHMARK(cpu_miner_benchmark);