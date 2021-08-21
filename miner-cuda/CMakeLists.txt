find_package(CUDA REQUIRED)

set(
    CU_SOURCE
    "lib/miner.cu"
)

# Work around cuda_add_library using plain signatures
set(CUDA_LIBRARIES PUBLIC ${CUDA_LIBRARIES})

set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};-use_fast_math;-lineinfo)

find_package(CGBN)

if(NOT CGBN_FOUND)
    message("Downloading CGBN from GitHub")
    include(FetchContent)
    FetchContent_Declare(CGBN
        # master commit as of july 16th 2021
        URL https://github.com/NVlabs/CGBN/tarball/2245cc145797d3405cb7d45ec1dec6acca8a7328
        URL_HASH SHA3_256=4365e3dea88f381abe1ef0f075fe2fdabe5a07ae21dd1de63bf42467c50d9e98
    )
    FetchContent_MakeAvailable(CGBN)
    set(CGBN_INCLUDE_DIR "${cgbn_SOURCE_DIR}/include")
    find_package(CGBN REQUIRED)
endif(NOT CGBN_FOUND)

find_package(Boost REQUIRED COMPONENTS log)

cuda_add_library(miner-cuda STATIC ${CU_SOURCE})
target_include_directories(miner-cuda PUBLIC "include")
target_link_libraries(miner-cuda PUBLIC miner-common PkgConfig::GMP Boost::log CGBN::CGBN)

if(BUILD_TEST)
    set(
        TEST_SOURCE
        "test/cuda_miner_test.cc"
    )
    add_executable(test-miner-cuda ${TEST_SOURCE})
    target_link_libraries(test-miner-cuda PRIVATE miner-cuda GTest::GTest GTest::Main)
    add_test(test-miner-cuda test-miner-cuda)
endif()

if(BUILD_BENCHMARK)
    set(
        BENCHMARK_SOURCE
        "test/cuda_miner_benchmark.cc"
    )
    add_executable(benchmark-miner-cuda ${BENCHMARK_SOURCE})
    target_link_libraries(benchmark-miner-cuda PRIVATE miner-cuda benchmark::benchmark benchmark::benchmark_main)
    add_test(benchmark-miner-cuda benchmark-miner-cuda)
endif()