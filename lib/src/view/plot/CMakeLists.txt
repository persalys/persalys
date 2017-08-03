if (ParaView_FOUND)
  add_subdirectory (paraview)
endif ()

set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file (CustomScaleEngine.cxx)
ot_add_source_file (CustomScaleDraw.cxx)
ot_add_source_file (PlotWidget.cxx)
ot_add_source_file (PieChartView.cxx)
ot_add_source_file (PlotMatrixWidget.cxx)
ot_add_source_file (GraphConfigurationWidget.cxx)
ot_add_source_file (PlotMatrixConfigurationWidget.cxx)

ot_install_header_file ( CustomScaleEngine.hxx )
ot_install_header_file ( CustomScaleDraw.hxx )
ot_install_header_file ( ContourData.hxx )
ot_install_header_file ( PlotWidget.hxx )
ot_install_header_file ( PieChartView.hxx )
ot_install_header_file ( PlotMatrixWidget.hxx )
ot_install_header_file ( GraphConfigurationWidget.hxx )
ot_install_header_file ( PlotMatrixConfigurationWidget.hxx )

qt5_wrap_cpp (OTGUI_PLOT_MOC_SRCS
              otgui/PlotWidget.hxx
              otgui/PieChartView.hxx
              otgui/PlotMatrixWidget.hxx
              otgui/GraphConfigurationWidget.hxx
              otgui/PlotMatrixConfigurationWidget.hxx)

include_directories (${INTERNAL_INCLUDE_DIRS})
otgui_add_library (otguiplot ${SOURCEFILES} ${OTGUI_PLOT_MOC_SRCS})

if ( BUILD_SHARED_LIBS )
  set_target_properties ( otguiplot PROPERTIES COMPILE_DEFINITIONS "OTGUI_DLL_EXPORTS" )
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( otguiplot PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( otguiplot PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries ( otguiplot otguiutils )

include_directories (${QWT_INCLUDE_DIRS})
target_link_libraries (otguiplot ${QT_LIBRARIES})
target_link_libraries (otguiplot ${QWT_LIBRARIES})
if (ParaView_FOUND)
  target_link_libraries (otguiplot otguiplotparaview)
endif ()

install ( TARGETS otguiplot
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)

