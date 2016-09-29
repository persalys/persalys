
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

qt5_wrap_cpp (OTGUI_CONSOLE_MOC_SRCS
              PyConsole_Console.h
              PyConsole_Editor.h)

include_directories (${INTERNAL_INCLUDE_DIRS})
add_library (otguipyconsole ${SOURCEFILES} ${OTGUI_CONSOLE_MOC_SRCS})

if ( BUILD_SHARED_LIBS )
  set_target_properties (otguipyconsole PROPERTIES COMPILE_DEFINITIONS "OTGUI_DLL_EXPORTS;PyConsole_EXPORTS")
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties (otguipyconsole PROPERTIES VERSION ${LIB_VERSION})
set_target_properties (otguipyconsole PROPERTIES SOVERSION ${LIB_SOVERSION})

include_directories (${PYTHON_INCLUDE_DIRS})
target_link_libraries (otguipyconsole ${QT_LIBRARIES})
target_link_libraries (otguipyconsole ${PYTHON_LIBRARIES})
target_link_libraries (otguipyconsole otguipyinterp)

install ( TARGETS otguipyconsole
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)
