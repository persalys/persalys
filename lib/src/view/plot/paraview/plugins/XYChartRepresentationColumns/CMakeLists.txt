paraview_add_plugin(XYChartRepresentationColumns
  VERSION "1.0"
  FORCE_STATIC ON
  XML_DOCUMENTATION OFF
  MODULES XYChartRepresentationColumn
  MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/XYChartRepresentationColumn/vtk.module")

set_target_properties (XYChartRepresentationColumns PROPERTIES POSITION_INDEPENDENT_CODE ON)
