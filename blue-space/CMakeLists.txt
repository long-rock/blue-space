set(
    CPP_SOURCE
    "src/main.cc"
)
add_executable(blue-space ${CPP_SOURCE})
target_link_libraries(blue-space PUBLIC miner-common miner-cpu explorer)

if(CUDA_MINER)
    target_compile_definitions(blue-space PUBLIC HAS_CUDA_MINER)
    target_link_libraries(blue-space PUBLIC miner-cuda)
endif()
