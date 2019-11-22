
vtk_module_add_module(XYChartRepresentationColumn
  CLASSES vtkXYChartRepresentationColumns)

paraview_add_server_manager_xmls(
  XMLS XYChartRepresentationColumns.xml)

