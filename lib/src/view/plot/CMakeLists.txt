if (ParaView_FOUND)
  add_subdirectory (pv)
endif ()

set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file (CustomScaleEngine.cxx)
ot_add_source_file (CustomScaleDraw.cxx)
ot_add_source_file (PlotWidget.cxx)
ot_add_source_file (ContourPlot.cxx)
ot_add_source_file (MorrisPlot.cxx)
ot_add_source_file (BoxPlot.cxx)
ot_add_source_file (SensitivityIndicesPlot.cxx)
ot_add_source_file (PieChartView.cxx)
ot_add_source_file (PlotMatrixWidget.cxx)
ot_add_source_file (GraphConfigurationWidget.cxx)
ot_add_source_file (PlotMatrixConfigurationWidget.cxx)

ot_install_header_file ( CustomScaleEngine.hxx )
ot_install_header_file ( CustomScaleDraw.hxx )
ot_install_header_file ( PlotWidget.hxx )
ot_install_header_file ( ContourPlot.hxx )
ot_install_header_file ( MorrisPlot.hxx )
ot_install_header_file ( BoxPlot.hxx )
ot_install_header_file ( SensitivityIndicesPlot.hxx )
ot_install_header_file ( PieChartView.hxx )
ot_install_header_file ( PlotMatrixWidget.hxx )
ot_install_header_file ( GraphConfigurationWidget.hxx )
ot_install_header_file ( PlotMatrixConfigurationWidget.hxx )

qt5_wrap_cpp (PERSALYS_PLOT_MOC_SRCS
              persalys/PlotWidget.hxx
              persalys/ContourPlot.hxx
              persalys/MorrisPlot.hxx
              persalys/BoxPlot.hxx
              persalys/SensitivityIndicesPlot.hxx
              persalys/PieChartView.hxx
              persalys/PlotMatrixWidget.hxx
              persalys/GraphConfigurationWidget.hxx
              persalys/PlotMatrixConfigurationWidget.hxx)

include_directories (${INTERNAL_INCLUDE_DIRS})
persalys_add_library (persalysplot ${SOURCEFILES} ${PERSALYS_PLOT_MOC_SRCS})

if (BUILD_SHARED_LIBS)
  target_compile_definitions (persalysplot PRIVATE PERSALYS_PLOT_DLL_EXPORTS)
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( persalysplot PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( persalysplot PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries ( persalysplot persalysutils )

include_directories (${QWT_INCLUDE_DIRS})
target_link_libraries (persalysplot ${QT_LIBRARIES})
target_link_libraries (persalysplot ${QWT_LIBRARIES})
if (ParaView_FOUND)
  target_link_libraries (persalysplot persalysplotpv)
endif ()

install ( TARGETS persalysplot
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${PERSALYS_LIBRARY_PATH}
  ARCHIVE DESTINATION ${PERSALYS_LIBRARY_PATH}
)

