set(qrc_files)

if (PARAVIEW_BUILD_QT_GUI)
  list(APPEND qrc_files
    BagPlotViewsAndFilters.qrc)
endif ()

paraview_add_plugin(BagPlotViewsAndFilters
  REQUIRED_ON_SERVER
  VERSION "1.0"
  FORCE_STATIC ON
  XML_DOCUMENTATION OFF
  SERVER_MANAGER_XML BagPlotViewsAndFilters.xml
  UI_RESOURCES ${qrc_files}
  MODULES BagPlotViewsAndFiltersBagPlot
  MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/BagPlot/vtk.module")

