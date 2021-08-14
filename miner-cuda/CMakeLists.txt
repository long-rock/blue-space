find_package(CUDA REQUIRED)

set(
    CU_SOURCE
    "lib/miner.cu"
)

# Work around cuda_add_library using plain signatures
set(CUDA_LIBRARIES PUBLIC ${CUDA_LIBRARIES})

set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};-use_fast_math)

find_package(CGBN REQUIRED)
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