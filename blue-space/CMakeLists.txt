find_package(Boost REQUIRED)
find_package(JsonRpcLean)

if(NOT JSON_RPC_LEAN_FOUND)
    message("Downloading JsonRpcLean from GitHub")
    include(FetchContent)
    FetchContent_Declare(
      JsonRpcLean
      GIT_REPOSITORY https://github.com/uskr/jsonrpc-lean.git
      GIT_TAG        e23500425fce7d3752e030ae0eeff71e974edd73 # v1.1.0
    )
    FetchContent_MakeAvailable(JsonRpcLean)
    set(JSONRPCLEAN_INCLUDE_DIR "${jsonrpclean_SOURCE_DIR}/include")
    find_package(JsonRpcLean REQUIRED)
endif()

# find_package CONFIG will make the CLI11:CLI11 target available if it succeeds
find_package(CLI11 1.9 CONFIG)
if(NOT CLI11_FOUND)
    message("Downloading CLI11 from GitHub")
    include(FetchContent)
    FetchContent_Declare(CLI11_EXTERNAL
        GIT_REPOSITORY    https://github.com/CLIUtils/CLI11.git
        GIT_TAG           5cb3efabce007c3a0230e4cc2e27da491c646b6c # v1.9.1
    )
    # FetchContent will automatically make the CLI11:CLI11 target available
    FetchContent_MakeAvailable(CLI11_EXTERNAL)
endif()

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
target_compile_definitions(application PUBLIC BLUE_SPACE_VERSION=${CMAKE_PROJECT_VERSION})
target_include_directories(application PUBLIC "include")
target_compile_definitions(application PUBLIC RAPIDJSON_HAS_STDSTRING=1)
target_link_libraries(application PUBLIC miner-common miner-cpu JsonRpcLean::Server)

if(CUDA_MINER)
    target_compile_definitions(application PUBLIC HAS_CUDA_MINER)
    target_link_libraries(application PUBLIC miner-cuda)
endif()

set(
    EXE_CPP_SOURCE
    "src/main.cc"
)
add_executable(blue-space ${EXE_CPP_SOURCE})
target_link_libraries(blue-space PUBLIC application CLI11::CLI11)

install(TARGETS blue-space RUNTIME DESTINATION bin)