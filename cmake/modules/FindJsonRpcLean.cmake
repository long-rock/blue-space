find_package(PkgConfig)
pkg_check_modules(PC_JSON_RPC_LEAN QUIET JsonRpcLean)

find_path(JSONRPCLEAN_INCLUDE_DIR
  NAMES server.h
  PATHS ${PC_JSON_RPC_LEAN_INCLUDE_DIRS}
  PATH_SUFFIXES jsonrpc-lean
)
mark_as_advanced(JSONRPCLEAN_FOUND JSONRPCLEAN_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(JsonRpcLean
  FOUND_VAR JSONRPCLEAN_FOUND
  REQUIRED_VARS
    JSONRPCLEAN_INCLUDE_DIR
)

if(JSON_RPC_LEAN_FOUND)
    set(JSONRPCLEAN_INCLUDE_DIRS ${JSONRPCLEAN_INCLUDE_DIR})
endif()

if(JSONRPCLEAN_FOUND AND NOT TARGET JsonRpcLean::Server)
    add_library(JsonRpcLean::Server INTERFACE IMPORTED)
    set_target_properties(JsonRpcLean::Server PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${JSONRPCLEAN_INCLUDE_DIR}"
    )
endif()