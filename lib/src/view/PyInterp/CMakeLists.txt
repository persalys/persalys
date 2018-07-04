set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file (PyInterp_Dispatcher.cxx)
ot_add_source_file (PyInterp_Event.cxx)
ot_add_source_file (PyInterp_Interp.cxx)
ot_add_source_file (PyInterp_Request.cxx)

# ot_install_header_file (PyInterp.h)
# ot_install_header_file (PyInterp_Event.h)
# ot_install_header_file (PyInterp_Interp.h)
# ot_install_header_file (PyInterp_Request.h)
# ot_install_header_file (PyInterp_Utils.h)

qt5_wrap_cpp (OTGUI_CONSOLE_MOC_SRCS
              PyInterp_Dispatcher.h)

include_directories (${INTERNAL_INCLUDE_DIRS})
otgui_add_library (otguipyinterp ${SOURCEFILES} ${OTGUI_CONSOLE_MOC_SRCS})

if ( BUILD_SHARED_LIBS )
  set_target_properties (otguipyinterp PROPERTIES COMPILE_DEFINITIONS "OTGUI_DLL_EXPORTS;PyInterp_EXPORTS")
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties (otguipyinterp PROPERTIES VERSION ${LIB_VERSION})
set_target_properties (otguipyinterp PROPERTIES SOVERSION ${LIB_SOVERSION})

include_directories (${PYTHON_INCLUDE_DIRS})
target_link_libraries (otguipyinterp ${QT_LIBRARIES})
target_link_libraries (otguipyinterp ${PYTHON_LIBRARIES})
if (MINGW AND CMAKE_SIZEOF_VOID_P EQUAL 8)
  target_compile_definitions (otguipyinterp PRIVATE -DMS_WIN64)
endif ()

install ( TARGETS otguipyinterp
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)
