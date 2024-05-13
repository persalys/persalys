cmake_minimum_required (VERSION 3.13)

option ( BUILD_PYTHON                 "Build the python module for the library"                               ON )
option ( BUILD_GUI                    "Build the GUI"                                                         ON )
option ( BUILD_SHARED_LIBS            "Build shared libraries"                                                ON )
option (USE_SALOME "use salome" ON)
option (USE_OTFMI "use otfmi" ON)
option (USE_OTMORRIS "use otmorris" ON)
option (USE_SPHINX "use sphinx" ON)
option (USE_PARAVIEW "use ParaView" ON)
set (PERSALYS_QT_MAJOR_VERSION 5 CACHE STRING "Qt version")

# By default, build in Release mode. Must appear before project() command
set( CMAKE_BUILD_TYPE Release CACHE STRING "Build type")

project (persalys)

if (POLICY CMP0020)
  # Automatically link Qt executables to qtmain target on Windows
  cmake_policy (SET CMP0020 NEW)
endif ()

if (POLICY CMP0058)
  # Ninja requires custom command byproducts to be explicit
  cmake_policy (SET CMP0058 NEW)
endif()

if (POLICY CMP0042)
  # MACOSX_RPATH is enabled by default
  cmake_policy(SET CMP0042 NEW)
endif()

string (TOLOWER ${CMAKE_PROJECT_NAME} PACKAGE_NAME)

# set module dir to find custom scripts
list (APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include (GNUInstallDirs)

set (PERSALYS_LIBRARY_PATH ${CMAKE_INSTALL_LIBDIR})
set (PERSALYS_BIN_PATH ${CMAKE_INSTALL_BINDIR})
set (PERSALYS_INCLUDE_PATH ${CMAKE_INSTALL_INCLUDEDIR})
set (PERSALYS_SWIG_INCLUDE_PATH ${PERSALYS_INCLUDE_PATH}/${PACKAGE_NAME}/swig)
set (PERSALYS_DATA_PATH ${CMAKE_INSTALL_DATAROOTDIR})
set (PERSALYS_CONFIG_CMAKE_PATH ${CMAKE_INSTALL_LIBDIR}/cmake/persalys)
set (PERSALYS_DOC_PATH ${CMAKE_INSTALL_DOCDIR})

find_package (OpenTURNS 1.23 CONFIG REQUIRED)
get_target_property (ot_features OT OPENTURNS_ENABLED_FEATURES)
if (NOT ot_features MATCHES "hdf5")
  message (FATAL_ERROR "OpenTURNS lacks hdf5 support")
endif()
message (STATUS "Found OpenTURNS: ${OPENTURNS_ROOT_DIR} (found version \"${OPENTURNS_VERSION_STRING}\")")
list (APPEND PERSALYS_PUBLIC_LIBRARIES ${OPENTURNS_LIBRARIES})

if (NOT BUILD_SHARED_LIBS)
  list (APPEND PERSALYS_DEFINITIONS "-DPERSALYS_STATIC -DPYINTERP_STATIC -DPYCONSOLE_STATIC")
endif ()

if (MSVC)
  # Disable some warnings
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4197 /wd4244 /wd4251 /wd4267 /wd4996")
  # for swig
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif ()

find_package (Boost COMPONENTS locale program_options REQUIRED)
list (APPEND PERSALYS_PRIVATE_LIBRARIES ${Boost_LIBRARIES})

find_package (Python 3.5 COMPONENTS Interpreter Development REQUIRED)
list (APPEND PERSALYS_PUBLIC_LIBRARIES ${Python_LIBRARIES})

if (BUILD_GUI)

  find_package (Qt${PERSALYS_QT_MAJOR_VERSION} COMPONENTS Widgets Test OpenGL CONFIG)
  set (QT_LIBRARIES ${Qt${PERSALYS_QT_MAJOR_VERSION}Widgets_LIBRARIES} ${Qt${PERSALYS_QT_MAJOR_VERSION}OpenGL_LIBRARIES} ${Qt${PERSALYS_QT_MAJOR_VERSION}Test_LIBRARIES})

  if (Qt${PERSALYS_QT_MAJOR_VERSION}Widgets_VERSION VERSION_LESS 5.15)
    macro (qt_wrap_cpp)
      qt5_wrap_cpp (${ARGN})
    endmacro ()

    macro (qt_add_resources)
      qt5_add_resources (${ARGN})
    endmacro ()

    macro (qt_generate_moc)
      qt5_generate_moc (${ARGN})
    endmacro ()
  endif ()

  find_package (Qwt 6 REQUIRED)

  if (USE_PARAVIEW)
    find_package (ParaView 5.8 CONFIG)
    if (ParaView_FOUND)
      message(STATUS "Found ParaView: ${ParaView_DIR} (found version \"${ParaView_VERSION}\")")
      list (APPEND PERSALYS_DEFINITIONS -DPERSALYS_HAVE_PARAVIEW)
      if (ParaView_VERSION VERSION_GREATER_EQUAL 5.11)
        list (APPEND PERSALYS_DEFINITIONS -DPERSALYS_HAVE_PARAVIEW511)
      endif ()
    endif ()
  endif ()

endif ()

if (USE_SALOME)
  find_package (SalomeKERNEL CONFIG)
  find_package (Qt5WebEngine CONFIG)
  find_package (Qt5WebEngineWidgets CONFIG)
  find_package (SalomeGUI CONFIG)
  find_package (SalomeYACS CONFIG)
  find_package (Py2cpp CONFIG)
  find_package (ydefx CONFIG)
  if (SalomeYACS_FOUND AND ydefx_FOUND)
    list (APPEND PERSALYS_DEFINITIONS ${YACS_DEFINITIONS})
    list (APPEND PERSALYS_DEFINITIONS -DPERSALYS_HAVE_YACS)
    include_directories (${YACS_INCLUDE_DIRS})
    list (APPEND PERSALYS_PUBLIC_LIBRARIES ydefx)
  endif ()

  find_package (AdaoCppLayer)
  if (AdaoCppLayer_FOUND)
    list (APPEND PERSALYS_DEFINITIONS -DPERSALYS_HAVE_ADAO_CPP_LAYER)
    include_directories (${AdaoCppLayer_INCLUDE_DIRS})
    list (APPEND PERSALYS_PRIVATE_LIBRARIES ${AdaoCppLayer_LIBRARIES})
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
  find_package (OTMORRIS 0.14 CONFIG)
  if (OTMORRIS_FOUND)
    message (STATUS "Found OTMORRIS: ${OTMORRIS_DIR} (found version \"${OTMORRIS_VERSION}\")")
    list (APPEND PERSALYS_DEFINITIONS -DPERSALYS_HAVE_OTMORRIS)
    list (APPEND PERSALYS_DEFINITIONS -DOTMORRIS_VERSION_STRING="${OTMORRIS_VERSION}")
    list (APPEND PERSALYS_PRIVATE_LIBRARIES ${OTMORRIS_LIBRARIES})
  endif ()
endif ()

list (APPEND PERSALYS_DEFINITIONS -DDOCUMENTATION_INSTALL_PATH="${PERSALYS_DOC_PATH}")

if (BUILD_PYTHON)
  find_package (SWIG 3)
  if (SWIG_FOUND)
    include (${SWIG_USE_FILE})
  endif ()

  include (FindPythonModule)
  if (NOT TARGET Python::Module)
    include (TargetLinkLibrariesWithDynamicLookup)
  endif ()

  if (USE_SPHINX)
    find_program (SPHINX_EXECUTABLE NAMES sphinx-build DOC "Sphinx Documentation Builder (sphinx-doc.org)")
    if (NOT SPHINX_EXECUTABLE)
      message (STATUS "Could NOT find sphinx-build executable")
    endif ()
    find_program (DVISVGM_EXECUTABLE NAMES dvisvgm DOC "dvi to svg converter")
    find_program (DVIPNG_EXECUTABLE NAMES dvipng DOC "dvi to png converter")
    if (NOT (DVISVGM_EXECUTABLE OR DVIPNG_EXECUTABLE))
      message (STATUS "Could NOT find dvisvgm|dvipng executable")
    endif ()
    if (SPHINX_EXECUTABLE AND (DVISVGM_EXECUTABLE OR DVIPNG_EXECUTABLE))
      find_python_module (sphinx 1.8)
      find_python_module (numpydoc 0.7)
    endif ()
    if (NOT SPHINX_EXECUTABLE OR NOT SPHINX_FOUND OR NOT NUMPYDOC_FOUND)
      set (SPHINX_FOUND FALSE)
      message (STATUS "Disabling documentation (requires dvisvgm/sphinx/numpydoc)")
    endif ()
  endif ()

  if (NOT DEFINED PYTHON_SITE_PACKAGES AND NOT CMAKE_CROSSCOMPILING)
    execute_process (COMMAND ${Python_EXECUTABLE} -c "import sysconfig, os; print(sysconfig.get_path('platlib').replace(sysconfig.get_path('data'), '').lstrip(os.path.sep))"
                    OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
    file (TO_CMAKE_PATH "${PYTHON_SITE_PACKAGES}" PYTHON_SITE_PACKAGES)
  endif ()

  if (Python_FOUND)
    if (DEFINED PYTHON_SITE_PACKAGES)
      set (PERSALYS_PYTHON_MODULE_PATH "${PYTHON_SITE_PACKAGES}")
    else ()
     if (WIN32)
       set (PERSALYS_PYTHON_MODULE_PATH Lib/site-packages)
     else ()
       set (PERSALYS_PYTHON_MODULE_PATH ${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages)
      endif ()
    endif ()
  endif ()
endif ()

# WARNING: This is where variables without PERSALYS_ prefix are defined !
foreach (_var PYTHON_MODULE_PATH LIBRARY_PATH INCLUDE_PATH CONFIG_CMAKE_PATH)
  if (IS_ABSOLUTE PERSALYS_${_var})
    set (${_var} ${PERSALYS_${_var}})
  else ()
    set (${_var} ${CMAKE_INSTALL_PREFIX}/${PERSALYS_${_var}})
  endif ()
endforeach ()

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
set (SOURCEFILES "" CACHE INTERNAL "List of source files to compile")
macro (ot_add_source_file FILENAME)
  set (sf ${SOURCEFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME})
  set (SOURCEFILES ${sf} CACHE INTERNAL "List of source files to compile")
endmacro ()

macro (ot_install_header_file FILENAME)
  install (FILES ${PACKAGE_NAME}/${FILENAME} DESTINATION ${PERSALYS_INCLUDE_PATH}/${PACKAGE_NAME})
endmacro ()

macro (ot_install_swig_file FILENAME)
	install (FILES ${FILENAME} DESTINATION ${PERSALYS_SWIG_INCLUDE_PATH})
endmacro ()

set ( INTERNAL_INCLUDE_DIRS "" CACHE INTERNAL "List of directories with header files needed for build" )
macro ( ot_add_current_dir_to_include_dirs )
  set ( inc_dirs ${INTERNAL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )
  set ( INTERNAL_INCLUDE_DIRS ${inc_dirs} CACHE INTERNAL "List of directories with header files needed for build" )
endmacro ( ot_add_current_dir_to_include_dirs )


set ( CPACK_PACKAGE_NAME          ${PACKAGE_NAME} )
set ( CPACK_PACKAGE_VERSION_MAJOR master              )
set ( CPACK_PACKAGE_VERSION_MINOR master              )
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

if (Python_FOUND AND SWIG_FOUND)
  add_subdirectory ( python )
  add_dependencies ( installcheck pyinstallcheck )
endif ()


include(CMakePackageConfigHelpers)
configure_package_config_file (
    cmake/PersalysConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/PersalysConfig.cmake
    INSTALL_DESTINATION ${PERSALYS_CONFIG_CMAKE_PATH}
    PATH_VARS
      PERSALYS_ROOT_DIR
      PERSALYS_INCLUDE_DIR
      PERSALYS_INCLUDE_DIRS
      PERSALYS_LIBRARY_DIRS
      PYTHON_MODULE_PATH
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
                                )
write_basic_package_version_file( PersalysConfigVersion.cmake VERSION ${PERSALYS_VERSION_STRING} COMPATIBILITY AnyNewerVersion )

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/PersalysConfig.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/PersalysConfigVersion.cmake
         DESTINATION ${PERSALYS_CONFIG_CMAKE_PATH})

include (CPack)

include (FeatureSummary)
feature_summary (WHAT ALL)
