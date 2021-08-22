# Blue Space: a Dark Forest miner

This is a a miner/explorer for the [Dark Forest game](https://zkga.me).
It includes a CPU miner implemented using GMP, and a GPU miner implemented in CUDA.

## Building

You need the following dependencies to build `blue-space`. You can install them
with your distribution favourite package manager.

 * [CMake](https://cmake.org/)
 * [The GNU MP Bignum library](https://gmplib.org/), a.k.a `libgmp`
 * Boost.Log
 * [Google Test](https://github.com/google/googletest)
 * [Google Benchmark](https://github.com/google/benchmark)
 * [CLI11 1.9](https://github.com/CLIUtils/CLI11).
   You can download the header to a location on your file system and set `CLI11_DIR`.
   If this is not installed, CMake will download it.
 * [Json Rpc Lean](https://github.com/uskr/jsonrpc-lean). Download the source
   on your file system, then set the `JSONRPCLEAN_INCLUDE_DIR` configuration
   variable to the path to jsonrpc-lean `include/` directory.
   If this is not installed, CMake will download it.

To build the GPU miner, you need the following additional dependencies.

 * [The CUDA Toolkit](https://developer.nvidia.com/cuda-toolkit). Follows for example [this step-by-step tutorial](https://cloud.google.com/compute/docs/gpus/install-drivers-gpu).
 * [CGBN](https://github.com/NVlabs/CGBN). You can download the source
    to a location on your file system and set `CGBN_INCLUDE_DIR`,
    otherwise if this is not installed, CMake will download it for you.

On Ubuntu 21.04 you can install all dependencies with:

    sudo apt install -y \
        build-essential \
        cmake \
        pkg-config \
        rapidjson-dev \
        libgmp-dev \
        libboost-container-dev \
        libboost-thread-dev \
        libboost-log-dev \
        libgtest-dev \ # optional
        libbenchmark-dev # optional

After you have all dependencies in place, it's time to compile `blue-space`.

Start by downloading the source to your file system.

    git clone https://github.com/long-rock/blue-space.git

Configure `cmake`, run the following from the `blue-space` directory.

    cmake \
        -S . \ # the source location
        -B buildir \ # the build directory
        -DCMAKE_BUILD_TYPE=Release \ # make release
        -DBUILD_TEST=OFF \ # build tests, requires gtest and benchmark
        -DCUDA_MINER=ON \ # enable cuda miner
        -DCUDA_HOST_COMPILER=/usr/bin/g++-10 \ # only needed if you have g++ 11 on your system
        -DCMAKE_CUDA_ARCHITECTURES=35 \ # cuda architecture, depends on your gpu

Finally, build `blue-space`.

    cmake --build builddir

The `blue-space` binary is `builddir/blue-space/blue-space`.

## Running

You can run `blue-space` in either CPU or CUDA mode. At the moment, you can only
run a short benchmark.

For benchmarking your CPU:

    blue-space \
        --benchmark \ # benchmark mode
        --cpu \ # use cpu miner
        --size 65536 \ # how many coordinates to mine in a batch
        --cpu-threads 8 # how many cpu threads

For benchmarking your CUDA:

    blue-space \
        --benchmark \ # benchmark mode
        --cuda \ # use cuda miner
        --size 65536 \ # how many coordinates to mine in a batch
        --cuda-device 0 \ # cuda device, 0 if you only have 1 GPU
        --cuda-thread-work-size 16 \ # how many coords per gpu thread
        --cuda-block-size 32 \ # how many threads in a block

You need to tune the `--size`, `--cuda-thread-work-size`, and
`--cuda-block-size` parameters to your GPU.

## License

Licensed under the GNU General Public License, Version 3.