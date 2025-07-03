# - Try to find libbacktrace
# Once done this will define
#  LIBBACKTRACE_FOUND - System has libbacktrace
#  LIBBACKTRACE_INCLUDE_DIRS - The libbacktrace include directories
#  LIBBACKTRACE_LIBRARIES - The libraries needed to use libbacktrace

find_path(LIBBACKTRACE_INCLUDE_DIR NAMES backtrace.h)
set(LIBBACKTRACE_INCLUDE_DIRS ${LIBBACKTRACE_INCLUDE_DIR})

find_library(LIBBACKTRACE_LIBRARY NAMES backtrace)
set(LIBBACKTRACE_LIBRARIES ${LIBBACKTRACE_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibBacktrace DEFAULT_MSG
                                  LIBBACKTRACE_LIBRARY LIBBACKTRACE_INCLUDE_DIR)

if(LIBBACKTRACE_FOUND)
    add_library(libbacktrace::libbacktrace UNKNOWN IMPORTED)
    set_target_properties(libbacktrace::libbacktrace PROPERTIES
        IMPORTED_LOCATION ${LIBBACKTRACE_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${LIBBACKTRACE_INCLUDE_DIR}
    )
endif()

mark_as_advanced(LIBBACKTRACE_INCLUDE_DIR LIBBACKTRACE_LIBRARY)
