
set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file (PyConsole_Console.cxx)
ot_add_source_file (PyConsole_Event.cxx)
ot_add_source_file (PyConsole_Interp.cxx)
ot_add_source_file (PyConsole_Request.cxx)
ot_add_source_file (PyConsole_Editor.cxx)

# ot_install_header_file (PyConsole.h)
# ot_install_header_file (PyConsole_Event.h)
# ot_install_header_file (PyConsole_Interp.h)
# ot_install_header_file (PyConsole_Request.h)

qt_wrap_cpp (PERSALYS_CONSOLE_MOC_SRCS
              PyConsole_Console.h
              PyConsole_Editor.h)

include_directories (${INTERNAL_INCLUDE_DIRS})
persalys_add_library (persalyspyconsole ${SOURCEFILES} ${PERSALYS_CONSOLE_MOC_SRCS})

if ( BUILD_SHARED_LIBS )
  target_compile_definitions (persalyspyconsole PRIVATE PERSALYS_DLL_EXPORTS PyConsole_EXPORTS)
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties (persalyspyconsole PROPERTIES VERSION ${LIB_VERSION})
set_target_properties (persalyspyconsole PROPERTIES SOVERSION ${LIB_SOVERSION})

target_link_libraries (persalyspyconsole ${QT_LIBRARIES})
target_link_libraries (persalyspyconsole ${Python_LIBRARIES})
if (MINGW AND CMAKE_SIZEOF_VOID_P EQUAL 8)
  target_compile_definitions (persalyspyconsole PRIVATE MS_WIN64)
endif ()

target_link_libraries (persalyspyconsole persalyspyinterp)

install ( TARGETS persalyspyconsole
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${PERSALYS_LIBRARY_PATH}
  ARCHIVE DESTINATION ${PERSALYS_LIBRARY_PATH}
)
