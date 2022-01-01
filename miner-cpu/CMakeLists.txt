set(
    CPP_SOURCE
    "lib/hash.cc"
    "lib/miner.cc"
)
add_library(miner-cpu STATIC ${CPP_SOURCE})
target_include_directories(miner-cpu PUBLIC "include")
find_package(Boost REQUIRED COMPONENTS log)
target_compile_definitions(miner-cpu PUBLIC BOOST_LOG_DYN_LINK) 
target_link_libraries(miner-cpu PUBLIC miner-common PkgConfig::GMP Boost::log)

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    target_link_libraries(miner-cpu PUBLIC OpenMP::OpenMP_CXX)
endif()

if(BUILD_TEST)
    set(
        TEST_SOURCE
        "test/cpu_miner_test.cc"
    )
    add_executable(test-miner-cpu ${TEST_SOURCE})
    target_link_libraries(test-miner-cpu PRIVATE miner-cpu GTest::GTest GTest::Main)
    add_test(test-miner-cpu test-miner-cpu)
endif()

if(BUILD_BENCHMARK)
    set(
        BENCHMARK_SOURCE
        "test/cpu_miner_benchmark.cc"
    )
    add_executable(benchmark-miner-cpu ${BENCHMARK_SOURCE})
    target_link_libraries(benchmark-miner-cpu PRIVATE miner-cpu benchmark::benchmark benchmark::benchmark_main)
    add_test(benchmark-miner-cpu benchmark-miner-cpu)
endif()