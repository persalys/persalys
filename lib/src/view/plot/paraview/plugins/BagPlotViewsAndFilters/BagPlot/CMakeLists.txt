set(classes
  vtkPVExtractBagPlots
  vtkSMFunctionalBagChartSeriesSelectionDomain
  vtkSMBagChartSeriesListDomain
  vtkPVBagChartRepresentation
  vtkPVBagPlotMatrixView
  vtkPVBagPlotMatrixRepresentation)

vtk_module_add_module(BagPlotViewsAndFiltersBagPlot
  CLASSES ${classes})
