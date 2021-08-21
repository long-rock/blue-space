# Blue Space: a Dark Forest miner

This is a a miner/explorer for the [Dark Forest game](https://zkga.me).
It includes a CPU miner implemented using GMP, and a GPU miner implemented in CUDA.

## Installation

We provide binary packages for Ubuntu 20.04, [you can find them in the Releases
section](https://github.com/long-rock/blue-space/releases).

The CUDA version requires you to have CUDA and the Nvidia drivers installed.

## Usage

Blue Space can be used in two modes:

 * `--benchmark`: run a simple benchmark and exit. This is used for tuning the CUDA
    parameters,
 * `--stateless`: run the explorer web server waiting requests from the game.

To toggle between CPU or GPU mode, use the `--cpu` or `--cuda` flags. Note that
the `--cuda` flag is available only if you downloaded the CUDA version of Blue Space.

You can get a list of all available flags with `blue-space --help`.

## Tuning the CUDA miner

The CUDA miner is _very_ sensitive to the parameters you pass to it. The parameters
optimal value is dependant on your card, so it's difficult to provide sensible
defaults. When tuning the miner, remember to also change the `--size` and then
change it in the remote explorer plugin as well.

The CUDA related flags are:

 * `--cuda-device`: If you have multiple cards, change between them by passing the
    card index. You can find the card index in Nvidia X Settings. By default this
    flag is `0`.
 * `--cuda-thread-work-size`: the number of locations explored by each CUDA thread.
 * `--cuda-block-size`: the number of _virtual_ threads in each block. See next option.
 * `--cuda-threads-per-item`: the number of threads used for each number. The default
    value of `8` should always be the optimal one. The effective number of threads in
    a block is given by: `block-size * threads-per-item`.

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

## License

Licensed under the GNU General Public License, Version 3.