set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file (qconsole.cpp)
ot_add_source_file (qpyconsole.cpp)

qt4_wrap_cpp (OTGUI_CONSOLE_MOC_SRCS
              otgui/qconsole.h
              otgui/qpyconsole.h)

include_directories (${INTERNAL_INCLUDE_DIRS})
add_library (otguipythonconsole ${SOURCEFILES} ${OTGUI_CONSOLE_MOC_SRCS})

if ( BUILD_SHARED_LIBS )
  set_target_properties (otguipythonconsole PROPERTIES COMPILE_DEFINITIONS "OTGUI_DLL_EXPORTS")
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties (otguipythonconsole PROPERTIES VERSION ${LIB_VERSION})
set_target_properties (otguipythonconsole PROPERTIES SOVERSION ${LIB_SOVERSION})

include_directories (${PYTHON_INCLUDE_DIRS})
target_link_libraries (otguipythonconsole ${QT_LIBRARIES})
target_link_libraries (otguipythonconsole ${PYTHON_LIBRARIES})

install ( TARGETS otguipythonconsole
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)

