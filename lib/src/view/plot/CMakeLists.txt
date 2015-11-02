set (SOURCEFILES "" CACHE STRING "otguiplot sources" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( DistributionScaleEngine.cxx )
ot_add_source_file ( OtguiPlotWidget.cxx )
ot_add_source_file ( PlotWidget.cxx )
ot_add_source_file ( ImageEditionDialog.cxx )
ot_add_source_file ( BarChart.cxx )

qt4_wrap_cpp (OTGUI_PLOT_MOC_SRCS OtguiPlotWidget.hxx PlotWidget.hxx ImageEditionDialog.hxx BarChart.hxx)

ot_install_header_file ( OtguiPlotWidget.hxx )
ot_install_header_file ( DistributionScaleEngine.hxx )
ot_install_header_file ( PlotWidget.hxx )
ot_install_header_file ( ImageEditionDialog.hxx )
ot_install_header_file ( BarChart.hxx )


include_directories (${INTERNAL_INCLUDE_DIRS})
add_library ( otguiplot ${SOURCEFILES} ${OTGUI_PLOT_MOC_SRCS})

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
target_link_libraries ( otguiplot otguibase )

find_package ( Qwt REQUIRED )
include_directories ( ${QWT_INCLUDE_DIRS} )
target_link_libraries( otguiplot ${QWT_LIBRARIES} )

install ( TARGETS otguiplot
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)

