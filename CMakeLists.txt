cmake_minimum_required (VERSION 3.8)

option ( BUILD_PYTHON                 "Build the python module for the library"                               ON )
option ( BUILD_GUI                    "Build the GUI"                                                         ON )
option ( BUILD_SHARED_LIBS            "Build shared libraries"                                                ON )
option (USE_SALOME "use salome" ON)
option (USE_OTFMI "use otfmi" ON)
option (USE_OTMORRIS "use otmorris" ON)
option (USE_SPHINX "use sphinx" ON)
option (USE_COTIRE "Use cotire for unity builds" OFF)
option (USE_PARAVIEW "use ParaView" ON)

# By default, build in Release mode. Must appear before project() command
if ( NOT DEFINED CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

project (Persalys)

if (POLICY CMP0020)
  cmake_policy (SET CMP0020 NEW)
endif ()

string ( TOLOWER ${CMAKE_PROJECT_NAME} PACKAGE_NAME )

# set module dir to find custom scripts
list ( APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )


if (USE_COTIRE)
  if (POLICY CMP0058)
    # Ninja requires custom command byproducts to be explicit
    cmake_policy (SET CMP0058 NEW)
  endif()
  include (cotire)
endif ()

include (GNUInstallDirs)

# add library with unity build support, inspired by https://github.com/onqtam/ucm
macro (persalys_add_library name)
  if (USE_COTIRE)
    set (orig_target ${name}_orig)
    # add regular target with another name
    add_library (${orig_target} ${ARGN})
    set_target_properties (${orig_target} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
    # set unity target name with the original name
    set_target_properties (${orig_target} PROPERTIES COTIRE_UNITY_TARGET_NAME ${name})
    # add unity build target
    cotire (${orig_target})
    set_target_properties (${name} PROPERTIES EXCLUDE_FROM_ALL 0 EXCLUDE_FROM_DEFAULT_BUILD 0)
    set_target_properties (${name} PROPERTIES OUTPUT_NAME ${name})
  else ()
    add_library (${name} ${ARGN})
  endif ()
endmacro ()

find_package (OpenTURNS 1.15 CONFIG REQUIRED)
message (STATUS "Found OpenTURNS: ${OPENTURNS_ROOT_DIR} (found version \"${OPENTURNS_VERSION_STRING}\")")

if (NOT BUILD_SHARED_LIBS)
  list (APPEND PERSALYS_DEFINITIONS "-DPERSALYS_STATIC -DPYINTERP_STATIC -DPYCONSOLE_STATIC")
endif ()

find_package (Boost COMPONENTS regex filesystem locale REQUIRED)
if (TARGET Boost::regex)
  set (Boost_LIBRARIES Boost::regex Boost::filesystem Boost::locale)
endif ()
include_directories (${Boost_INCLUDE_DIRS})

find_package (PythonInterp 3.2)
find_package (PythonLibs ${PYTHON_VERSION_STRING} EXACT REQUIRED)

if (BUILD_GUI)

  find_package (Qt5Widgets CONFIG)
  find_package (Qt5Test CONFIG)
  set (QT_LIBRARIES ${Qt5Widgets_LIBRARIES} ${Qt5Test_LIBRARIES})
  set (CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${CMAKE_CXX_FLAGS}")
  include_directories (${Qt5Widgets_INCLUDE_DIRS} ${Qt5Test_INCLUDE_DIRS})

  find_package (Qwt 6 REQUIRED)

  if (USE_PARAVIEW)
    find_package (ParaView 5.8 CONFIG)
    if (ParaView_FOUND)
      message(STATUS "Found ParaView: ${ParaView_DIR} (found version \"${ParaView_VERSION}\")")
      list (APPEND PERSALYS_DEFINITIONS -DPERSALYS_HAVE_PARAVIEW)
    endif ()
  endif ()

endif ()

if (USE_SALOME)
  find_package (SalomeKERNEL CONFIG)
  find_package (Qt5WebEngine CONFIG)
  find_package (Qt5WebEngineWidgets CONFIG)
  find_package (SalomeGUI CONFIG)
  find_package (SalomeYACS CONFIG)
  if (SalomeYACS_FOUND)
    list (APPEND PERSALYS_DEFINITIONS ${YACS_DEFINITIONS})
    list (APPEND PERSALYS_DEFINITIONS -DPERSALYS_HAVE_YACS)
    include_directories (${YACS_INCLUDE_DIRS})
  endif ()
  find_package (Py2cpp CONFIG)
  find_package (ydefx CONFIG)

  find_package (AdaoCppLayer)
  if (AdaoCppLayer_FOUND)
    list (APPEND PERSALYS_DEFINITIONS -DPERSALYS_HAVE_ADAO_CPP_LAYER)
    include_directories (${AdaoCppLayer_INCLUDE_DIRS})
  endif ()
endif ()

if (USE_OTFMI)
  include (FindPythonModule)
  find_python_module (otfmi)
  if (OTFMI_FOUND)
    list (APPEND PERSALYS_DEFINITIONS -DPERSALYS_HAVE_OTFMI)
    list (APPEND PERSALYS_DEFINITIONS -DOTFMI_VERSION_STRING="${OTFMI_VERSION_STRING}")
  endif ()
endif ()

if (USE_OTMORRIS)
  find_package (OTMORRIS 0.6 CONFIG)
  if (OTMORRIS_FOUND)
     message (STATUS "Found OTMORRIS: ${OTMORRIS_DIR} (found version \"${OTMORRIS_VERSION}\")")
    list (APPEND PERSALYS_DEFINITIONS -DPERSALYS_HAVE_OTMORRIS)
    list (APPEND PERSALYS_DEFINITIONS -DOTMORRIS_VERSION_STRING="${OTMORRIS_VERSION}")
    include_directories (${OTMORRIS_INCLUDE_DIRS})
  endif ()
endif ()

set ( PERSALYS_LIBRARY_PATH lib${LIB_SUFFIX} )
set ( PERSALYS_INCLUDE_PATH include )
set ( PERSALYS_SWIG_INCLUDE_PATH ${PERSALYS_INCLUDE_PATH}/${PACKAGE_NAME}/swig )
set ( PERSALYS_DATA_PATH share )
set ( PERSALYS_EXAMPLE_PATH ${PERSALYS_DATA_PATH}/${PACKAGE_NAME}/examples )
set ( PERSALYS_CONFIG_CMAKE_PATH lib${LIB_SUFFIX}/cmake/persalys )
set ( DOC_INSTALL_PATH share/${PACKAGE_NAME}/doc )

# beware swig<3.0 does not like c++11: https://github.com/swig/swig/issues/73
set (CMAKE_CXX_STANDARD 11)

list (APPEND PERSALYS_DEFINITIONS -DDOCUMENTATION_INSTALL_PATH="${DOC_INSTALL_PATH}")

if (BUILD_PYTHON)
  find_package (SWIG)
  if (SWIG_FOUND)
    include (${SWIG_USE_FILE})
  endif ()

  include (FindPythonModule)

  if (USE_SPHINX)
    find_program (SPHINX_EXECUTABLE NAMES sphinx-build DOC "Sphinx Documentation Builder (sphinx-doc.org)")
    if (SPHINX_EXECUTABLE)
      find_python_module (sphinx 1.1)
      find_python_module (numpydoc)
    endif ()
    if (NOT SPHINX_EXECUTABLE OR NOT SPHINX_FOUND OR NOT NUMPYDOC_FOUND OR NOT OTMORRIS_FOUND)
      set (SPHINX_FOUND FALSE)
      message (STATUS "Disabling documentation (requires sphinx/numpydoc/otmorris)")
    endif ()
  endif ()

  if (PYTHONINTERP_FOUND)
    if (DEFINED PYTHON_SITE_PACKAGES)
      set (PERSALYS_PYTHON_MODULE_PATH ${PYTHON_SITE_PACKAGES})

    else ()
      execute_process ( COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(plat_specific=True, prefix='${CMAKE_INSTALL_PREFIX}'))"
                        OUTPUT_VARIABLE _ABS_PYTHON_MODULE_PATH
                        RESULT_VARIABLE _PYTHON_pythonlib_result
                        OUTPUT_STRIP_TRAILING_WHITESPACE )

      if (_PYTHON_pythonlib_result)
        message (SEND_ERROR "Could not run ${PYTHON_EXECUTABLE}")
      endif ()

      get_filename_component (_ABS_PYTHON_MODULE_PATH ${_ABS_PYTHON_MODULE_PATH} ABSOLUTE)
      file (RELATIVE_PATH PERSALYS_PYTHON_MODULE_PATH ${CMAKE_INSTALL_PREFIX} ${_ABS_PYTHON_MODULE_PATH})
    endif ()

    if (IS_ABSOLUTE ${PERSALYS_PYTHON_MODULE_PATH})
      set (PYTHON_MODULE_PATH ${PERSALYS_PYTHON_MODULE_PATH})
    else ()
      set (PYTHON_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/${PERSALYS_PYTHON_MODULE_PATH})
    endif ()

    set (PERSALYS_PYTHON${PYTHON_VERSION_MAJOR}_MODULE_PATH ${PERSALYS_PYTHON_MODULE_PATH})
  endif ( PYTHONINTERP_FOUND )
endif ()


# WARNING: This is where variables without OPENTURNS_ prefix are defined !
foreach ( _var PYTHON_MODULE_PATH LIBRARY_PATH INCLUDE_PATH CONFIG_CMAKE_PATH)
  if ( NOT IS_ABSOLUTE PERSALYS_${_var} )
    set ( PERSALYS_${_var} ${CMAKE_INSTALL_PREFIX}/${PERSALYS_${_var}} )
  endif ()
  set ( ${_var} ${PERSALYS_${_var}} )
endforeach ( _var )

# RPATH settings
set (CMAKE_INSTALL_RPATH ${LIBRARY_PATH} ${OTMORRIS_LIBRARY_DIRS})
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
if (POLICY CMP0042)
  # MACOSX_RPATH is enabled by default
  cmake_policy(SET CMP0042 NEW)
endif()
if (POLICY CMP0078)
  # UseSWIG generates standard target names
  cmake_policy(SET CMP0078 NEW)
endif()
if (POLICY CMP0086)
  # UseSWIG honors SWIG_MODULE_NAME via -module flag
  cmake_policy(SET CMP0086 NEW)
endif()

# Some useful macros to ease CMakeLists.txt file writing
set ( SOURCEFILES "" CACHE INTERNAL "List of source files to compile" )
macro ( ot_add_source_file FILENAME )
  set ( sf ${SOURCEFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME} )
  set ( SOURCEFILES ${sf} CACHE INTERNAL "List of source files to compile" )
endmacro ( ot_add_source_file )

set ( HEADERFILES "" CACHE INTERNAL "List of header files to install" )
macro ( ot_install_header_file FILENAME )
  set ( hf ${HEADERFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}/${FILENAME} )
  set ( HEADERFILES ${hf} CACHE INTERNAL "List of header files to install" )
endmacro ( ot_install_header_file )

set ( SWIGFILES "" CACHE INTERNAL "List of SWIG files to install" )
macro ( ot_install_swig_file FILENAME )
  set ( hf ${SWIGFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME} )
  set ( SWIGFILES ${hf} CACHE INTERNAL "List of SWIG files to install" )
endmacro ( ot_install_swig_file )

set ( INTERNAL_INCLUDE_DIRS "" CACHE INTERNAL "List of directories with header files needed for build" )
macro ( ot_add_current_dir_to_include_dirs )
  set ( inc_dirs ${INTERNAL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )
  set ( INTERNAL_INCLUDE_DIRS ${inc_dirs} CACHE INTERNAL "List of directories with header files needed for build" )
endmacro ( ot_add_current_dir_to_include_dirs )


set ( CPACK_PACKAGE_NAME          ${PACKAGE_NAME} )
set ( CPACK_PACKAGE_VERSION_MAJOR 9              )
set ( CPACK_PACKAGE_VERSION_MINOR 0dev              )
set ( CPACK_PACKAGE_VERSION_PATCH                )
set ( CPACK_SOURCE_GENERATOR      "TGZ;TBZ2"     )
set ( CPACK_BINARY_STGZ           OFF CACHE BOOL "STGZ")
set ( CPACK_BINARY_TBZ2           ON  CACHE BOOL "TBZ2")
set ( CPACK_BINARY_TGZ            ON  CACHE BOOL "TGZ")
set ( CPACK_BINARY_TZ             OFF CACHE BOOL "TZ")
set ( CPACK_SOURCE_IGNORE_FILES "/.git;/build;.*~;.*exe;.*AppImage;.*doc.zip;${CPACK_SOURCE_IGNORE_FILES}" )
set ( PACKAGE_NAME           ${CPACK_PACKAGE_NAME} )
set ( PACKAGE_VERSION        ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR} )
if ( CPACK_PACKAGE_VERSION_PATCH )
  set ( PACKAGE_VERSION       ${PACKAGE_VERSION}.${CPACK_PACKAGE_VERSION_PATCH} )
endif ()
set ( CPACK_SOURCE_PACKAGE_FILE_NAME ${PACKAGE_NAME}-${PACKAGE_VERSION} )

# uninstall target
configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  IMMEDIATE @ONLY
)

add_custom_target ( uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

set ( PERSALYS_LIBRARY                 persalys )
set ( PERSALYS_LIBRARIES ${OPENTURNS_LIBRARIES} )
list ( APPEND PERSALYS_LIBRARIES ${PERSALYS_LIBRARY} )
set ( PERSALYS_LIBRARY_DIRS ${OPENTURNS_LIBRARY_DIRS})
if (OTMORRIS_FOUND)
  list ( APPEND PERSALYS_LIBRARY_DIRS ${OTMORRIS_LIBRARY_DIRS} )
endif ()
list ( APPEND PERSALYS_LIBRARY_DIRS ${LIBRARY_PATH} )
set ( PERSALYS_INCLUDE_DIR ${PERSALYS_INCLUDE_PATH} )
set ( PERSALYS_INCLUDE_DIRS ${PERSALYS_INCLUDE_DIR} )
set ( PERSALYS_ROOT_DIR       ${CMAKE_INSTALL_PREFIX} )
list ( APPEND PERSALYS_INCLUDE_DIRS ${OPENTURNS_INCLUDE_DIRS} )
set ( PERSALYS_VERSION_STRING ${PACKAGE_VERSION} )
set ( PERSALYS_VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR} )
set ( PERSALYS_VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR} )
set ( PERSALYS_VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH} )
set ( PERSALYS_USE_FILE      ${PERSALYS_CONFIG_CMAKE_PATH}/UsePersalys.cmake )

list (APPEND PERSALYS_DEFINITIONS -DPERSALYS_VERSION="${PERSALYS_VERSION_STRING}")
list (APPEND PERSALYS_DEFINITIONS -DINSTALL_PATH="${CMAKE_INSTALL_PREFIX}")

add_definitions (${PERSALYS_DEFINITIONS})

# The tests can't be run if this function is absent
enable_testing ()
add_custom_target ( tests COMMENT "Build tests" )
add_custom_target ( check COMMENT "Run pre-installation tests" )
add_custom_target ( installcheck COMMENT "Run post-installation tests" )

add_subdirectory ( lib )

add_subdirectory ( translations )

add_dependencies ( check cppcheck )

if (BUILD_GUI)
  add_subdirectory (main)
endif ()

if (PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND SWIG_FOUND)
  add_subdirectory ( python )
  add_dependencies ( installcheck pyinstallcheck )
endif ()


include(CMakePackageConfigHelpers)
configure_package_config_file (
    cmake/PersalysConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/PersalysConfig.cmake
    INSTALL_DESTINATION ${PERSALYS_CONFIG_CMAKE_PATH}
    PATH_VARS
      PERSALYS_USE_FILE
      PERSALYS_ROOT_DIR
      PERSALYS_INCLUDE_DIR
      PERSALYS_INCLUDE_DIRS
      PERSALYS_LIBRARY_DIRS
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
                                )
write_basic_package_version_file( PersalysConfigVersion.cmake VERSION ${PERSALYS_VERSION_STRING} COMPATIBILITY AnyNewerVersion )

install ( FILES cmake/UsePersalys.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/PersalysConfig.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/PersalysConfigVersion.cmake
            DESTINATION ${PERSALYS_CONFIG_CMAKE_PATH}
          )

include (CPack)

include (FeatureSummary)
feature_summary (WHAT ALL)

