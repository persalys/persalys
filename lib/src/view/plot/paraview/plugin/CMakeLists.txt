
paraview_add_plugin(XYChartRepresentationColumns
  VERSION "1.0"
  MODULES XYChartRepresentationColumnsModule
  MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/XYChartRepresentationColumnsModule/vtk.module"
  SERVER_MANAGER_XML views.xml
  XML_DOCUMENTATION OFF
  FORCE_STATIC ON
)

set_target_properties (XYChartRepresentationColumns PROPERTIES POSITION_INDEPENDENT_CODE ON)
