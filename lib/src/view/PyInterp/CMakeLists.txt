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

qt_wrap_cpp (PERSALYS_CONSOLE_MOC_SRCS
              PyInterp_Dispatcher.h)

include_directories (${INTERNAL_INCLUDE_DIRS})
add_library (persalyspyinterp ${SOURCEFILES} ${PERSALYS_CONSOLE_MOC_SRCS})

if ( BUILD_SHARED_LIBS )
  target_compile_definitions (persalyspyinterp PRIVATE PERSALYS_DLL_EXPORTS PyInterp_EXPORTS)
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties (persalyspyinterp PROPERTIES VERSION ${LIB_VERSION})
set_target_properties (persalyspyinterp PROPERTIES SOVERSION ${LIB_SOVERSION})

target_include_directories (persalyspyinterp PUBLIC ${Python_INCLUDE_DIRS})
target_link_libraries (persalyspyinterp ${QT_LIBRARIES})
target_link_libraries (persalyspyinterp ${Python_LIBRARIES})
if (MINGW AND CMAKE_SIZEOF_VOID_P EQUAL 8)
  target_compile_definitions (persalyspyinterp PRIVATE MS_WIN64)
endif ()

install ( TARGETS persalyspyinterp
  RUNTIME DESTINATION ${PERSALYS_BIN_PATH}
  LIBRARY DESTINATION ${PERSALYS_LIBRARY_PATH}
  ARCHIVE DESTINATION ${PERSALYS_LIBRARY_PATH}
)
