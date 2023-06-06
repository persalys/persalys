set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file (PVViewWidget.cxx)
ot_add_source_file (PVParCooViewWidget.cxx)
ot_add_source_file (PVSpreadSheetViewWidget.cxx)
ot_add_source_file (PVMatrixPlotViewWidget.cxx)
ot_add_source_file (PVXYChartSettingWidget.cxx)
ot_add_source_file (PVXYChartViewWidget.cxx)
ot_add_source_file (PVPlotSettingWidget.cxx)
ot_add_source_file (PVServerManagerSingleton.cxx)
ot_add_source_file (PVBagChartViewWidget.cxx)

ot_install_header_file (PVMatrixPlotViewWidget.hxx)
ot_install_header_file (PVParCooViewWidget.hxx)
ot_install_header_file (PVSpreadSheetViewWidget.hxx)
ot_install_header_file (PVViewWidget.hxx)
ot_install_header_file (PVServerManagerInterface.hxx)
ot_install_header_file (PVServerManagerSingleton.hxx)
ot_install_header_file (PVXYChartSettingWidget.hxx)
ot_install_header_file (PVXYChartViewWidget.hxx)
ot_install_header_file (PVPlotSettingWidget.hxx)
ot_install_header_file (PVBagChartViewWidget.hxx)

qt_wrap_cpp (PERSALYS_PLOTPARAVIEW_MOC_SRCS
              persalys/PVMatrixPlotViewWidget.hxx
              persalys/PVParCooViewWidget.hxx
              persalys/PVSpreadSheetViewWidget.hxx
              persalys/PVViewWidget.hxx
              persalys/PVXYChartSettingWidget.hxx
              persalys/PVXYChartViewWidget.hxx
              persalys/PVPlotSettingWidget.hxx
              persalys/PVBagChartViewWidget.hxx
              )

include_directories (${INTERNAL_INCLUDE_DIRS})
add_library (persalysplotpv ${SOURCEFILES} ${PERSALYS_PLOTPARAVIEW_MOC_SRCS})

if (BUILD_SHARED_LIBS)
  target_compile_definitions (persalysplotpv PRIVATE PERSALYS_PLOTPV_DLL_EXPORTS)
else ()
  target_compile_definitions (persalysplotpv PUBLIC PERSALYS_PLOTPV_STATIC)
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( persalysplotpv PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( persalysplotpv PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries ( persalysplotpv persalysutils )

target_link_libraries (persalysplotpv ParaView::pqApplicationComponents ParaView::RemotingViews)

install ( TARGETS persalysplotpv
  RUNTIME DESTINATION ${PERSALYS_BIN_PATH}
  LIBRARY DESTINATION ${PERSALYS_LIBRARY_PATH}
  ARCHIVE DESTINATION ${PERSALYS_LIBRARY_PATH}
)

# https://gitlab.kitware.com/paraview/paraview/issues/19358
if (CMAKE_LIBRARY_OUTPUT_DIRECTORY STREQUAL "")
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ".")
endif ()
# https://kitware.github.io/paraview-docs/latest/cxx/PluginHowto.html
paraview_plugin_scan (
  PLUGIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/plugin/paraview.plugin"
  PROVIDES_PLUGINS plugins
  REQUIRES_MODULES required_modules
  ENABLE_BY_DEFAULT ON)

foreach(module IN LISTS required_modules)
  if(NOT TARGET "${module}")
    message("Missing required module: ${module}")
    return()
  endif()
endforeach()

paraview_plugin_build (
  RUNTIME_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY_DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY_SUBDIRECTORY "${PARAVIEW_PLUGIN_SUBDIR}"
  PLUGINS ${plugins}
  AUTOLOAD ${plugins})
target_link_libraries (persalysplotpv XYChartRepresentationColumns)


