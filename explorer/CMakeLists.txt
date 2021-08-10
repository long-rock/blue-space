set(
    CPP_SOURCE
    "lib/storage.cc"
    "lib/explorer.cc"
)
add_library(explorer STATIC ${CPP_SOURCE})
target_include_directories(explorer PUBLIC "include")
target_link_libraries(explorer PUBLIC miner-common)

if(BUILD_TEST)
    set(
        TEST_SOURCE
        "test/explorer_test.cc"
    )
    add_executable(test-explorer ${TEST_SOURCE})
    target_link_libraries(test-explorer PRIVATE explorer GTest::GTest GTest::Main)
    add_test(test-explorer test-explorer)
endif()