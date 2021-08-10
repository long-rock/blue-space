find_package(PkgConfig)
pkg_check_modules(PC_CGBN QUIET CGBN)

find_path(CGBN_INCLUDE_DIR
  NAMES cgbn.h
  PATHS ${PC_CGBN_INCLUDE_DIRS}
  PATH_SUFFIXES cgbn
)
mark_as_advanced(CGBN_FOUND CGBN_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CGBN
  FOUND_VAR CGBN_FOUND
  REQUIRED_VARS
    CGBN_INCLUDE_DIR
)

if(CGBN_FOUND)
    set(CGBN_INCLUDE_DIRS ${CGBN_INCLUDE_DIR})
endif()

if(CGBN_FOUND AND NOT TARGET CGBN::CGBN)
    add_library(CGBN::CGBN INTERFACE IMPORTED)
    set_target_properties(CGBN::CGBN PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CGBN_INCLUDE_DIR}"
    )
endif()