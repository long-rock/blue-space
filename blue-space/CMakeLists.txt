find_package(Boost REQUIRED)
find_package(JsonRpcLean REQUIRED)

set(
    LIB_CPP_SOURCE
    "lib/api/stateless.cpp"
    "lib/rpc/stateless.cpp"
    "lib/rpc/server.cpp"
    "lib/session.cpp"
    "lib/listener.cpp"
    "lib/server.cpp"
    "lib/application.cpp"
)
add_library(application STATIC ${LIB_CPP_SOURCE})
target_include_directories(application PUBLIC "include")
target_compile_definitions(application PUBLIC RAPIDJSON_HAS_STDSTRING=1)
target_link_libraries(application PUBLIC miner-common miner-cpu explorer JsonRpcLean::Server)

if(CUDA_MINER)
    target_compile_definitions(application PUBLIC HAS_CUDA_MINER)
    target_link_libraries(application PUBLIC miner-cuda)
endif()

set(
    EXE_CPP_SOURCE
    "src/main.cc"
)
add_executable(blue-space ${EXE_CPP_SOURCE})
target_link_libraries(blue-space PUBLIC application)
