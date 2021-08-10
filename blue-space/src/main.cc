#include "explorer/explorer.h"
#include "explorer/storage.h"

#include "miner/common/miner.h"
#include "miner/cpu/miner.h"
#ifdef HAS_CUDA_MINER
#include "miner/cuda/miner.h"
#endif

#include <CLI/CLI.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>

class BlueSpace
{
  public:
    enum MinerType
    {
        Cpu,
        Cuda,
    };

    using timer_clock = std::chrono::steady_clock;

    bool parse_args(int argc, char **argv)
    {
        CLI::App app{"Blue Space miner for Dark Forest"};

        bool print_help = false;
        app.set_help_flag();
        app.add_flag("-h,--help", print_help, "Show help");

        bool benchmark_mode = false;
        app.add_flag("--benchmark", benchmark_mode, "Run a simple benchmark");

        bool use_cpu_miner = false;
        app.add_flag("--cpu", use_cpu_miner, "Use CPU miner");

        bool use_cuda_miner = false;
#if HAS_CUDA_MINER
        app.add_flag("--cuda", use_cuda_miner, "Use CUDA miner");

        std::optional<uint32_t> cuda_device;
        app.add_option("--cuda-device", cuda_device, "Set the CUDA device to use");

        std::optional<uint32_t> cuda_thread_work_size;
        app.add_option("--cuda-thread-work-size", cuda_thread_work_size,
                       "Set the number of items processed by each CUDA thread");

        std::optional<uint32_t> cuda_block_size;
        app.add_option("--cuda-block-size", cuda_block_size, "Set the size of each CUDA block");
#endif

        std::optional<uint32_t> mine_rarity;
        app.add_option("--rarity", mine_rarity, "Set planet rarity threshold");

        std::optional<uint32_t> mine_key;
        app.add_option("--key", mine_key, "Set mining key");

        std::optional<uint32_t> mine_size;
        app.add_option("--size", mine_size, "Set the number of coordinates to mine");

        app.parse(argc, argv);

        if (print_help)
        {
            std::cout << app.help() << std::endl;
            return false;
        }

        if (use_cuda_miner)
        {
            miner_type_ = MinerType::Cuda;
#if HAS_CUDA_MINER
            cuda_device_ = cuda_device.value_or(0);
            miner::cuda::CudaMinerOptions options(cuda_thread_work_size.value_or(16), cuda_block_size.value_or(32));
            cuda_miner_options_ = options;
#endif
        }
        else
        {
            // fallback to cpu miner
            miner_type_ = MinerType::Cpu;
        }

        mine_rarity_ = mine_rarity.value_or(16384);
        mine_key_ = mine_key.value_or(420);
        mine_size_ = mine_size.value_or(256);

        benchmark_mode_ = benchmark_mode;

        return true;
    }

    void run()
    {
        std::unique_ptr<miner::common::Miner> miner;
        if (miner_type_ == MinerType::Cpu)
        {
            std::cout << "Using CPU miner" << std::endl;
            miner = std::make_unique<miner::cpu::CpuMiner>();
        }
        else if (miner_type_ == MinerType::Cuda)
        {
#if HAS_CUDA_MINER
            std::cout << "Using CUDA miner" << std::endl;
            miner = std::make_unique<miner::cuda::CudaMiner>(cuda_device_, cuda_miner_options_);
#endif
        }

        if (benchmark_mode_)
        {
            run_benchmark(miner);
        }
    }

    void run_benchmark(std::unique_ptr<miner::common::Miner> &miner)
    {
        miner::common::Coordinate origin(0, 0);
        auto storage = std::make_shared<explorer::InMemoryStorage>();
        auto explorer = std::make_shared<explorer::SpiralExplorer>(storage, origin);

        std::vector<miner::common::WorkItem> batch;

        for (std::size_t i = 0; i < mine_size_; ++i)
        {
            // just crash if there's no value
            auto next = explorer->next().value();
            batch.push_back(miner::common::WorkItem{.x = next.x, .y = next.y, .is_planet = false, .hash = ""});
        }

        auto start = timer_clock::now();
        std::cout << "Starting: size=" << mine_size_ << ", rarity=" << mine_rarity_ << ", key=" << mine_key_
                  << std::endl;
        miner->mine_batch(batch, mine_rarity_, mine_key_);
        for (auto &item : batch)
        {
            storage->store(item);
        }
        auto end = timer_clock::now();
        auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (time_ms > 0)
        {
            auto rate = mine_size_ / time_ms;
            std::cout << "Mining rate:\t" << (rate * 1000.0) << " H/s" << std::endl;
        }
    }

  private:
    MinerType miner_type_;
#if HAS_CUDA_MINER
    uint32_t cuda_device_;
    miner::cuda::CudaMinerOptions cuda_miner_options_;
#endif
    uint32_t mine_rarity_;
    uint32_t mine_key_;
    uint32_t mine_size_;
    bool benchmark_mode_;
};

const uint64_t RARITY = 16384;
const uint64_t KEY = 420;

int main(int argc, char **argv)
{
    try
    {
        BlueSpace cli;
        if (!cli.parse_args(argc, argv))
        {
            return 0;
        }

        cli.run();

        return 0;
    }
    catch (const std::invalid_argument &exc)
    {
        std::cerr << "Error: " << exc.what() << std::endl;
        return 1;
    }
    /*
    miner::common::Coordinate origin(0, 0);
    auto storage = std::make_shared<explorer::FileStorage>("/tmp/explorer.db");
    auto explorer = std::make_shared<explorer::SpiralExplorer>(storage, origin);

    std::size_t batch_size = 256 * 256 * 4;
    std::vector<miner::common::WorkItem> batch;

    for (std::size_t i = 0; i < batch_size; ++i)
    {
        // just crash if there's no value
        auto next = explorer->next().value();
        batch.push_back(miner::common::WorkItem{.x = next.x, .y = next.y, .is_planet = false, .hash = ""});
    }

#ifdef HAS_CUDA_MINER
    miner::cuda::CudaMinerOptions options;
    miner::cuda::CudaMiner miner(0, options);
#else
    miner::cpu::CpuMiner miner;
#endif

    auto start = std::chrono::steady_clock::now();
    miner.mine_batch(batch, RARITY, KEY);
    for (auto &item : batch)
    {
        storage->store(item);
    }
    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    auto rate = (batch_size * 1000.0) / duration;
    std::cout << "Mine " << batch_size << " hashes in " << duration << " ms (" << rate << " H/s)" << std::endl;

    for (std::size_t i = 0; i < batch.size(); ++i)
    {
        auto p = batch[i];
        if (p.is_planet)
        {
            std::cout << "H(" << p.x << ", " << p.y << ") = " << p.hash << std::endl;
        }
    }
    return 0;
    */
}