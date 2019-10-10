paraview_add_plugin(XYChartRepresentationColumns
  VERSION "1.0"
  FORCE_STATIC ON
  XML_DOCUMENTATION OFF
  SERVER_MANAGER_XML XYChartRepresentationColumns.xml
  SOURCES vtkXYChartRepresentationColumns.cxx)

set_target_properties (XYChartRepresentationColumns PROPERTIES POSITION_INDEPENDENT_CODE ON)
