find_path(GMP_INCLUDE_DIRS NAMES gmpxx.h)
find_library(GMP_LIBRARY NAMES gmp libgmp)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP
    DEFAULT_MSG
    GMP_LIBRARY
    GMP_INCLUDE_DIRS)
mark_as_advanced(GMP_INCLUDE_DIRS GMP_LIBRARY)
if (GMP_FOUND)
    add_library(GMP::GMP UNKNOWN IMPORTED)
    set_target_properties(GMP::GMP PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${GMP_INCLUDE_DIRS}")
    set_target_properties(GMP::GMP PROPERTIES IMPORTED_LOCATION "${GMP_LIBRARY}")
endif (GMP_FOUND)
