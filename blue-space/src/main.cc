#include "application/application.h"

#include "miner/common/miner.h"
#include "miner/cpu/miner.h"
#ifdef HAS_CUDA_MINER
#include "miner/cuda/miner.h"
#endif

#include <CLI/CLI.hpp>
#include <boost/log/trivial.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>

#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

constexpr char version[] = STRINGIZE(BLUE_SPACE_VERSION);

class BlueSpace
{
  public:
    enum MinerType
    {
        Cpu,
        Cuda,
    };

    enum RunMode
    {
        Benchmark,
        Stateless
    };

    using timer_clock = std::chrono::steady_clock;

    bool parse_args(int argc, char **argv)
    {
        CLI::App app{"Blue Space miner for Dark Forest"};

        bool print_help = false;
        app.set_help_flag();
        app.add_flag("-h,--help", print_help, "Show help");

        bool print_version = false;
        app.add_flag("--version", print_version, "Show version");

        bool benchmark_mode = false;
        app.add_flag("--benchmark", benchmark_mode, "Run a simple benchmark");

        bool stateless_mode = false;
        app.add_flag("--stateless", stateless_mode, "Run stateless server");

        std::optional<uint32_t> http_port;
        app.add_option("--http-port", http_port, "Set the http port used by the rpc server");

        bool use_cpu_miner = false;
        app.add_flag("--cpu", use_cpu_miner, "Use CPU miner");

        std::optional<uint32_t> cpu_num_threads;
        app.add_option("--cpu-threads", cpu_num_threads, "Set the number of CPU threads used for mining");

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

        if (print_version)
        {
#if HAS_CUDA_MINER
            std::cout << version << "-cuda" << std::endl;
#else
            std::cout << version << std::endl;
#endif
            return false;
        }

        http_port_ = http_port.value_or(8888);

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
            miner::cpu::CpuMinerOptions options(cpu_num_threads.value_or(0));
            cpu_miner_options_ = options;
        }

        mine_rarity_ = mine_rarity.value_or(16384);
        mine_key_ = mine_key.value_or(420);
        mine_size_ = mine_size.value_or(256);

        if (benchmark_mode)
        {
            run_mode_ = RunMode::Benchmark;
        }
        else if (stateless_mode)
        {
            run_mode_ = RunMode::Stateless;
        }
        else
        {
            // default to stateless
            run_mode_ = RunMode::Stateless;
        }

        return true;
    }

    void run()
    {
        std::shared_ptr<miner::common::Miner> miner;
        if (miner_type_ == MinerType::Cpu)
        {
            BOOST_LOG_TRIVIAL(info) << "Use CPU miner";
            miner = std::make_shared<miner::cpu::CpuMiner>(cpu_miner_options_);
        }
        else if (miner_type_ == MinerType::Cuda)
        {
#if HAS_CUDA_MINER
            BOOST_LOG_TRIVIAL(info) << "Use CUDA miner, device id " << cuda_device_;
            miner = std::make_shared<miner::cuda::CudaMiner>(cuda_device_, cuda_miner_options_);
#endif
        }

        if (run_mode_ == RunMode::Benchmark)
        {
            run_benchmark(miner);
        }
        else if (run_mode_ == RunMode::Stateless)
        {
            application::Application::Options app_options;
            app_options.http_port = http_port_;
            application::Application app(app_options);
            app.initialize(miner);
            app.start();
        }
    }

    void run_benchmark(std::shared_ptr<miner::common::Miner> &miner)
    {
        int64_t half_size = static_cast<int64_t>(mine_size_) / 2;
        miner::common::Coordinate bottom_left(-half_size, -half_size);
        miner::common::ChunkFootprint chunk(bottom_left, mine_size_);
        std::vector<miner::common::PlanetLocation> result;

        auto start = timer_clock::now();
        BOOST_LOG_TRIVIAL(info) << "Mine batch size=" << mine_size_ << ", rarity=" << mine_rarity_
                                << ", key=" << mine_key_;
        miner->mine(chunk, mine_rarity_, mine_key_, result);
        auto end = timer_clock::now();
        auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (time_ms > 0)
        {
            double num_hashes = mine_size_ * mine_size_;
            double rate = num_hashes / (time_ms / 1000.0);
            BOOST_LOG_TRIVIAL(info) << "Mined " << num_hashes << " hashes in " << time_ms << " ms. Hash rate: " << rate
                                    << " H/s";
        }
    }

  private:
    uint32_t http_port_;
    MinerType miner_type_;
    miner::cpu::CpuMinerOptions cpu_miner_options_;
#if HAS_CUDA_MINER
    uint32_t cuda_device_;
    miner::cuda::CudaMinerOptions cuda_miner_options_;
#endif
    uint32_t mine_rarity_;
    uint32_t mine_key_;
    uint32_t mine_size_;
    RunMode run_mode_;
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
}