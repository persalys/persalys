set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file (PVViewWidget.cxx)
ot_add_source_file (PVParCooViewWidget.cxx)
ot_add_source_file (PVMatrixPlotViewWidget.cxx)

ot_install_header_file (PVMatrixPlotViewWidget.hxx)
ot_install_header_file (PVMatrixPlotViewWidget.hxx)
ot_install_header_file (PVParCooViewWidget.hxx)
ot_install_header_file (PVViewWidget.hxx)
ot_install_header_file (PVServerManagerInterface.hxx)

qt5_wrap_cpp (OTGUI_PLOTPARAVIEW_MOC_SRCS
              otgui/PVMatrixPlotViewWidget.hxx
              otgui/PVParCooViewWidget.hxx
              otgui/PVViewWidget.hxx)

include_directories (${INTERNAL_INCLUDE_DIRS})
add_library (otguiplotparaview ${SOURCEFILES} ${OTGUI_PLOTPARAVIEW_MOC_SRCS})

if ( BUILD_SHARED_LIBS )
  set_target_properties ( otguiplotparaview PROPERTIES COMPILE_DEFINITIONS "OTGUI_DLL_EXPORTS" )
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( otguiplotparaview PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( otguiplotparaview PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries ( otguiplotparaview otguiutils )
target_link_libraries (otguiplotparaview pqApplicationComponents vtkRenderingFreeType)

install ( TARGETS otguiplotparaview
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)

