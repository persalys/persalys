cmake_minimum_required ( VERSION 2.8 )

option ( BUILD_PYTHON                 "Build the python module for the library"                               ON )
option ( BUILD_GUI                    "Build the GUI"                                                         ON )
option ( LINK_PYTHON_LIBRARY          "Link python modules against python library"                            ON )
option ( BUILD_SHARED_LIBS            "Build shared libraries"                                                ON )
option (USE_SALOME "use salome" ON)
option (USE_OTFMI "use otfmi" ON)
option (USE_SPHINX "use sphinx" ON)
option (USE_QT5 "use Qt5" ON)
option (USE_COTIRE "Use cotire for unity builds" OFF)

# By default, build in Release mode. Must appear before project() command
if ( NOT DEFINED CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

project ( OTGui CXX )

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


# add library with unity build support, inspired by https://github.com/onqtam/ucm
macro (otgui_add_library name)
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

find_package (OpenTURNS 1.9 NO_MODULE REQUIRED)
include ( ${OPENTURNS_USE_FILE} )
set ( OTGUI_DEFINITIONS ${OPENTURNS_DEFINITIONS} )
if (NOT BUILD_SHARED_LIBS)
  list ( APPEND OTGUI_DEFINITIONS "-DOTGUI_STATIC" )
endif ()

find_package (Boost COMPONENTS regex filesystem REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

find_package (PythonInterp)
find_package (PythonLibs REQUIRED)

if (BUILD_GUI)

  if (USE_QT5)
    find_package (Qt5Widgets NO_MODULE)
    find_package (Qt5Test NO_MODULE)
    set (QT_LIBRARIES ${Qt5Widgets_LIBRARIES} ${Qt5Test_LIBRARIES})
    set (CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${CMAKE_CXX_FLAGS}")
    include_directories (${Qt5Widgets_INCLUDE_DIRS} ${Qt5Test_INCLUDE_DIRS})
  endif ()

  if (NOT Qt5Gui_FOUND)
    find_package (Qt4 COMPONENTS QtCore QtGui QtTest REQUIRED)
    include (${QT_USE_FILE})

    macro (qt5_wrap_cpp)
      qt4_wrap_cpp (${ARGN})
    endmacro ()

    macro (qt5_add_resources)
      qt4_add_resources (${ARGN})
    endmacro ()

    macro (qt5_generate_moc)
      qt4_generate_moc (${ARGN})
    endmacro ()
  endif ()

  find_package (Qwt 6 REQUIRED)
endif ()

if (USE_SALOME)
  find_package (SalomeKERNEL NO_MODULE)
  find_package (SalomeGUI NO_MODULE)
  find_package (SalomeYACS NO_MODULE)
  if (SalomeYACS_FOUND)
    list (APPEND OTGUI_DEFINITIONS ${YACS_DEFINITIONS})
    list (APPEND OTGUI_DEFINITIONS -DOTGUI_HAVE_YACS)
    include_directories (${YACS_INCLUDE_DIRS}) 
  endif ()
endif ()

if (USE_OTFMI)
  include (FindPythonModule)
  find_python_module (otfmi)
  if (OTFMI_FOUND)
    list (APPEND OTGUI_DEFINITIONS -DOTGUI_HAVE_OTFMI)
  endif ()
endif ()

add_definitions (${OTGUI_DEFINITIONS})

set ( OTGUI_LIBRARY_PATH lib${LIB_SUFFIX} )
set ( OTGUI_INCLUDE_PATH include )
set ( OTGUI_SWIG_INCLUDE_PATH ${OTGUI_INCLUDE_PATH}/${PACKAGE_NAME}/swig )
set ( OTGUI_DATA_PATH share )
set ( OTGUI_EXAMPLE_PATH ${OTGUI_DATA_PATH}/${PACKAGE_NAME}/examples )
set ( OTGUI_CONFIG_CMAKE_PATH lib${LIB_SUFFIX}/cmake/otgui )
set ( DOC_INSTALL_PATH share/${PACKAGE_NAME}/doc )

if ( BUILD_PYTHON )
  # workaround for cmake bug #0013449
  if ( NOT DEFINED CMAKE_FIND_ROOT_PATH )
    find_package ( SWIG )
  else ()
    find_program ( SWIG_EXECUTABLE NAMES swig2.0 swig )
    if ( SWIG_EXECUTABLE )
      set ( SWIG_USE_FILE ${CMAKE_ROOT}/Modules/UseSWIG.cmake )
      set ( SWIG_FOUND TRUE )
    endif ()
  endif ()
  include (FindPythonModule)

  if (USE_SPHINX)
    find_program (SPHINX_EXECUTABLE NAMES sphinx-build DOC "Sphinx Documentation Builder (sphinx-doc.org)")
    if (SPHINX_EXECUTABLE)
      find_python_module (sphinx 1.1)
      find_python_module (numpydoc)
    endif ()
  endif ()

  if ( PYTHONINTERP_FOUND )
    execute_process ( COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(plat_specific=True, prefix='${CMAKE_INSTALL_PREFIX}'))"
                      OUTPUT_VARIABLE _ABS_PYTHON_MODULE_PATH
                      RESULT_VARIABLE _PYTHON_pythonlib_result 
                      OUTPUT_STRIP_TRAILING_WHITESPACE )

    if (NOT _PYTHON_pythonlib_result) 
      get_filename_component ( _ABS_PYTHON_MODULE_PATH ${_ABS_PYTHON_MODULE_PATH} ABSOLUTE )
      file ( RELATIVE_PATH _REL_PYTHON_MODULE_PATH ${CMAKE_INSTALL_PREFIX} ${_ABS_PYTHON_MODULE_PATH} )                   

      set (OTGUI_PYTHON_MODULE_PATH ${_REL_PYTHON_MODULE_PATH})

    else ()
      message ( SEND_ERROR "Could not run ${PYTHON_EXECUTABLE}" ) 
    endif ()

    if (CMAKE_VERSION VERSION_LESS "2.8.6")
      execute_process (COMMAND ${PYTHON_EXECUTABLE} -c "import sys; print(sys.version_info[0])"
                       OUTPUT_VARIABLE PYTHON_VERSION_MAJOR
                       OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif ()
    set (OTGUI_PYTHON${PYTHON_VERSION_MAJOR}_MODULE_PATH ${OTGUI_PYTHON_MODULE_PATH})
  endif ( PYTHONINTERP_FOUND )
endif ()


# WARNING: This is where variables without OPENTURNS_ prefix are defined !
foreach ( _var PYTHON_MODULE_PATH LIBRARY_PATH INCLUDE_PATH CONFIG_CMAKE_PATH)
  if ( NOT IS_ABSOLUTE OTGUI_${_var} )
    set ( OTGUI_${_var} ${CMAKE_INSTALL_PREFIX}/${OTGUI_${_var}} )
  endif ()
  set ( ${_var} ${OTGUI_${_var}} )
endforeach ( _var )

if (CMAKE_INSTALL_PREFIX MATCHES "^/usr")
  set (SYSTEM_INSTALL ON)
else ()
  set (SYSTEM_INSTALL OFF)
endif ()

# Selectively add or remove RPATH from executable
if (NOT SYSTEM_INSTALL)
  set (CMAKE_INSTALL_RPATH ${LIBRARY_PATH})
  set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif ()

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

if ( PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND SWIG_FOUND AND BUILD_SHARED_LIBS )
  add_subdirectory ( python )
  add_dependencies ( installcheck pyinstallcheck )
endif ()

set ( CPACK_PACKAGE_NAME          ${PACKAGE_NAME} )
set ( CPACK_PACKAGE_VERSION_MAJOR 3              )
set ( CPACK_PACKAGE_VERSION_MINOR 0dev              )
set ( CPACK_PACKAGE_VERSION_PATCH                )
set ( CPACK_SOURCE_GENERATOR      "TGZ;TBZ2"     )
set ( CPACK_BINARY_STGZ           "OFF"          )
set ( CPACK_BINARY_TBZ2           "ON"           )
set ( CPACK_BINARY_TGZ            "ON"           )
set ( CPACK_BINARY_TZ             "OFF"          )
set ( CPACK_SOURCE_IGNORE_FILES "/.git;/build;.*~;${CPACK_SOURCE_IGNORE_FILES}" )
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

set ( OTGUI_LIBRARY                 otgui )
set ( OTGUI_LIBRARIES ${OPENTURNS_LIBRARIES} )
list ( APPEND OTGUI_LIBRARIES ${OTGUI_LIBRARY} )
set ( OTGUI_LIBRARY_DIRS ${OPENTURNS_LIBRARY_DIRS} )
list ( APPEND OTGUI_LIBRARY_DIRS ${LIBRARY_PATH} )
set ( OTGUI_INCLUDE_DIR ${OTGUI_INCLUDE_PATH} )
set ( OTGUI_INCLUDE_DIRS ${OTGUI_INCLUDE_DIR} )
set ( OTGUI_ROOT_DIR       ${CMAKE_INSTALL_PREFIX} )
list ( APPEND OTGUI_INCLUDE_DIRS ${OPENTURNS_INCLUDE_DIRS} )
set ( OTGUI_VERSION_STRING ${PACKAGE_VERSION} )
set ( OTGUI_VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR} )
set ( OTGUI_VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR} )
set ( OTGUI_VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH} )
set ( OTGUI_USE_FILE      ${OTGUI_CONFIG_CMAKE_PATH}/UseOTGui.cmake )

if (CMAKE_VERSION VERSION_LESS 2.8.8)
  # CMakePackageConfigHelpers is not available
  set (PACKAGE_INIT "
# Generated by a CMake version without CMakePackageConfigHelpers.
macro(set_and_check _var _value)
  set(\${_var} \"\${_value}\")
endmacro(set_and_check _var _value)
"     )
  foreach (_var
      OTGUI_USE_FILE
      OTGUI_ROOT_DIR
      OTGUI_INCLUDE_DIR
      OTGUI_INCLUDE_DIRS
      OTGUI_LIBRARY_DIRS )
    set(PACKAGE_${_var} ${${_var}})
  endforeach ()
  
configure_file ( ${CMAKE_SOURCE_DIR}/cmake/OTGuiConfig.cmake.in
                 ${CMAKE_BINARY_DIR}/OTGuiConfig.cmake
                 @ONLY ESCAPE_QUOTES
               )

install ( FILES cmake/UseOTGui.cmake 
                ${CMAKE_BINARY_DIR}/OTGuiConfig.cmake
          DESTINATION ${OTGUI_CONFIG_CMAKE_PATH}
        )

else ()

  include(CMakePackageConfigHelpers)
  configure_package_config_file (
    cmake/OTGuiConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/OTGuiConfig.cmake
    INSTALL_DESTINATION ${OTGUI_CONFIG_CMAKE_PATH}
    PATH_VARS
      OTGUI_USE_FILE
      OTGUI_ROOT_DIR
      OTGUI_INCLUDE_DIR
      OTGUI_INCLUDE_DIRS
      OTGUI_LIBRARY_DIRS
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
                                )                   
  write_basic_package_version_file( OTGuiConfigVersion.cmake VERSION ${OTGUI_VERSION_STRING} COMPATIBILITY AnyNewerVersion )               

  install ( FILES cmake/UseOTGui.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/OTGuiConfig.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/OTGuiConfigVersion.cmake
            DESTINATION ${OTGUI_CONFIG_CMAKE_PATH}
          )
endif ()

include (CPack)

if (CMAKE_VERSION VERSION_GREATER 2.8.5)
  include (FeatureSummary)
  feature_summary (WHAT ALL)
endif ()

