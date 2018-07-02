add_subdirectory (PyInterp)
add_subdirectory (PyConsole)
add_subdirectory (utils)
add_subdirectory (plot)

set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( MainWindow.cxx )
ot_add_source_file ( MainWidget.cxx )
ot_add_source_file ( ToolBar.cxx )
ot_add_source_file ( Actions.cxx )
ot_add_source_file ( MenuBar.cxx )
ot_add_source_file ( StatusBar.cxx )
ot_add_source_file ( SubWindow.cxx )
ot_add_source_file ( Wizard.cxx )
ot_add_source_file ( SubWindowsStackedWidget.cxx )
ot_add_source_file ( StudyManager.cxx )
ot_add_source_file ( WindowFactory.cxx )
ot_add_source_file ( Controller.cxx )
ot_add_source_file ( PhysicalModelDiagramWindow.cxx )
ot_add_source_file ( FieldModelDiagramWindow.cxx )
ot_add_source_file ( DataModelDiagramWindow.cxx )
ot_add_source_file ( DataModelWindow.cxx )
ot_add_source_file ( DataAnalysisWindow.cxx )
ot_add_source_file ( DataAnalysisResultWindow.cxx )
ot_add_source_file ( AnalysisWizard.cxx )
ot_add_source_file ( WelcomeWindow.cxx )
ot_add_source_file ( StudyWindow.cxx )
ot_add_source_file ( MeshWindow.cxx )
ot_add_source_file ( BlockSizeGroupBox.cxx )
ot_add_source_file ( MinMaxTableGroupBox.cxx )
ot_add_source_file ( MomentsEstimatesTableGroupBox.cxx )
ot_add_source_file ( OutputsSelectionGroupBox.cxx )
ot_add_source_file ( ParametersWidget.cxx )
ot_add_source_file ( ResultWindow.cxx )
ot_add_source_file ( AnalysisWindow.cxx )
ot_add_source_file ( DesignOfExperimentIntroPage.cxx )
ot_add_source_file ( GridDesignPage.cxx )
ot_add_source_file ( ProbabilisticDesignPage.cxx )
ot_add_source_file ( DesignOfExperimentInputWindow.cxx )
ot_add_source_file ( DesignOfExperimentEvaluationWizard.cxx )
ot_add_source_file ( ImportedDesignPage.cxx )
ot_add_source_file ( PhysicalModelWindowWidget.cxx )
ot_add_source_file ( SymbolicPhysicalModelWindow.cxx )
ot_add_source_file ( PythonPhysicalModelWindow.cxx )
ot_add_source_file ( MetaModelWindow.cxx )
if (SalomeYACS_FOUND)
  ot_add_source_file ( YACSPhysicalModelWindow.cxx)
endif ()
if (USE_OTFMI)
  ot_add_source_file ( FMIPhysicalModelWindow.cxx)
endif ()
ot_add_source_file ( ProbabilisticModelWindow.cxx )
ot_add_source_file ( DesignOfExperimentWizard.cxx )
ot_add_source_file ( DesignOfExperimentWindow.cxx )
ot_add_source_file ( StudyTreeView.cxx )
ot_add_source_file ( ModelEvaluationWizard.cxx )
ot_add_source_file ( ModelEvaluationResultWindow.cxx )
ot_add_source_file ( CentralTendencyIntroPage.cxx )
ot_add_source_file ( MonteCarloPage.cxx )
ot_add_source_file ( CentralTendencyWizard.cxx )
ot_add_source_file ( MonteCarloResultWindow.cxx )
ot_add_source_file ( TaylorExpansionMomentsResultWindow.cxx )
ot_add_source_file ( SensitivityResultWidget.cxx )
ot_add_source_file ( SobolResultWindow.cxx )
ot_add_source_file ( SRCResultWindow.cxx )
ot_add_source_file ( SensitivityIntroPage.cxx )
ot_add_source_file ( SobolPage.cxx )
ot_add_source_file ( SRCPage.cxx )
ot_add_source_file ( SensitivityAnalysisWizard.cxx )
ot_add_source_file ( LimitStateWindow.cxx )
ot_add_source_file ( ReliabilityIntroPage.cxx )
ot_add_source_file ( SimulationReliabilityPage.cxx )
ot_add_source_file ( ApproximationReliabilityPage.cxx )
ot_add_source_file ( ReliabilityAnalysisWizard.cxx )
ot_add_source_file ( SimulationReliabilityResultWindow.cxx )
ot_add_source_file ( ApproximationResultWindow.cxx )
ot_add_source_file ( ApproximationResultTabWidget.cxx )
ot_add_source_file ( StopCriteriaGroupBox.cxx )
ot_add_source_file ( ParametersDefinitionWizard.cxx )
ot_add_source_file ( MetaModelIntroPage.cxx )
ot_add_source_file ( KrigingPage.cxx )
ot_add_source_file ( FunctionalChaosPage.cxx )
ot_add_source_file ( MetaModelAnalysisWizard.cxx )
ot_add_source_file ( MetaModelValidationPage.cxx )
ot_add_source_file ( MetaModelValidationWidget.cxx )
ot_add_source_file ( FunctionalChaosResultWindow.cxx )
ot_add_source_file ( KrigingResultWindow.cxx )
ot_add_source_file ( InferenceWizard.cxx )
ot_add_source_file ( InferenceResultWindow.cxx )
ot_add_source_file ( DistributionsForInferenceWidget.cxx )
ot_add_source_file ( InferenceResultWizard.cxx )
ot_add_source_file ( InferenceResultWidget.cxx )
ot_add_source_file ( CopulaParametersTabWidget.cxx )
ot_add_source_file ( CopulaInferenceResultWidget.cxx )
ot_add_source_file ( CopulaInferenceResultWindow.cxx )
ot_add_source_file ( OptimizationIntroPage.cxx )
ot_add_source_file ( OptimizationWizard.cxx )
ot_add_source_file ( OptimizationResultWindow.cxx )
ot_add_source_file ( WidgetBoundToDockWidget.cxx )
ot_add_source_file ( MarginalsWidget.cxx )
ot_add_source_file ( DependenciesWidget.cxx )
ot_add_source_file ( CopulaWidget.cxx )
ot_add_source_file ( CopulaInferenceWizard.cxx )
ot_add_source_file ( CopulaInferenceResultWizard.cxx )
ot_add_source_file ( AboutDialog.cxx )
ot_add_source_file ( MeshDefinitionWizard.cxx )
ot_add_source_file ( FieldModelEvaluationResultWindow.cxx )
ot_add_source_file ( FieldMonteCarloWizard.cxx )
if (OTMORRIS_FOUND)
  ot_add_source_file ( ScreeningIntroPage.cxx )
  ot_add_source_file ( MorrisPage.cxx )
  ot_add_source_file ( ScreeningAnalysisWizard.cxx )
  ot_add_source_file ( MorrisResultWidget.cxx )
  ot_add_source_file ( MorrisResultWindow.cxx )
  ot_add_source_file ( ScreeningResultWizard.cxx )
endif ()

ot_install_header_file ( MainWindow.hxx )
ot_install_header_file ( MainWidget.hxx )
ot_install_header_file ( Actions.hxx )
ot_install_header_file ( ToolBar.hxx )
ot_install_header_file ( MenuBar.hxx )
ot_install_header_file ( StatusBar.hxx )
ot_install_header_file ( Wizard.hxx )
ot_install_header_file ( SubWindowsStackedWidget.hxx )
ot_install_header_file ( StudyManager.hxx )
ot_install_header_file ( WindowFactory.hxx )
ot_install_header_file ( AnalysisWizard.hxx )
ot_install_header_file ( WelcomeWindow.hxx )
ot_install_header_file ( StudyWindow.hxx )
ot_install_header_file ( MeshWindow.hxx )
ot_install_header_file ( PhysicalModelDiagramWindow.hxx )
ot_install_header_file ( FieldModelDiagramWindow.hxx )
ot_install_header_file ( DataModelDiagramWindow.hxx )
ot_install_header_file ( ResultWindow.hxx )
ot_install_header_file ( BlockSizeGroupBox.hxx )
ot_install_header_file ( MinMaxTableGroupBox.hxx )
ot_install_header_file ( MomentsEstimatesTableGroupBox.hxx )
ot_install_header_file ( OutputsSelectionGroupBox.hxx )
ot_install_header_file ( ParametersWidget.hxx )
ot_install_header_file ( DataModelWindow.hxx )
ot_install_header_file ( DataAnalysisWindow.hxx )
ot_install_header_file ( DataAnalysisResultWindow.hxx )
ot_install_header_file ( AnalysisWindow.hxx )
ot_install_header_file ( ModelEvaluationWizard.hxx )
ot_install_header_file ( CentralTendencyIntroPage.hxx )
ot_install_header_file ( MonteCarloPage.hxx )
ot_install_header_file ( CentralTendencyWizard.hxx )
ot_install_header_file ( SensitivityIntroPage.hxx )
ot_install_header_file ( SobolPage.hxx )
ot_install_header_file ( SRCPage.hxx )
ot_install_header_file ( SensitivityAnalysisWizard.hxx )
ot_install_header_file ( LimitStateWindow.hxx )
ot_install_header_file ( ReliabilityIntroPage.hxx )
ot_install_header_file ( SimulationReliabilityPage.hxx )
ot_install_header_file ( ApproximationReliabilityPage.hxx )
ot_install_header_file ( ReliabilityAnalysisWizard.hxx )
ot_install_header_file ( SubWindow.hxx )
ot_install_header_file ( PhysicalModelWindowWidget.hxx )
ot_install_header_file ( SymbolicPhysicalModelWindow.hxx )
ot_install_header_file ( PythonPhysicalModelWindow.hxx )
ot_install_header_file ( MetaModelWindow.hxx )
if (SalomeYACS_FOUND)
  ot_install_header_file (YACSPhysicalModelWindow.hxx)
endif ()
if (USE_OTFMI)
  ot_install_header_file (FMIPhysicalModelWindow.hxx)
endif ()
ot_install_header_file ( ProbabilisticModelWindow.hxx )
ot_install_header_file ( DesignOfExperimentWizard.hxx )
ot_install_header_file ( DesignOfExperimentWindow.hxx )
ot_install_header_file ( ModelEvaluationResultWindow.hxx )
ot_install_header_file ( MonteCarloResultWindow.hxx )
ot_install_header_file ( TaylorExpansionMomentsResultWindow.hxx )
ot_install_header_file ( SensitivityResultWidget.hxx )
ot_install_header_file ( SobolResultWindow.hxx )
ot_install_header_file ( SRCResultWindow.hxx )
ot_install_header_file ( SimulationReliabilityResultWindow.hxx )
ot_install_header_file ( ApproximationResultWindow.hxx )
ot_install_header_file ( ApproximationResultTabWidget.hxx )
ot_install_header_file ( StudyTreeView.hxx )
ot_install_header_file ( DesignOfExperimentIntroPage.hxx )
ot_install_header_file ( GridDesignPage.hxx )
ot_install_header_file ( ProbabilisticDesignPage.hxx )
ot_install_header_file ( ImportedDesignPage.hxx )
ot_install_header_file ( DesignOfExperimentInputWindow.hxx )
ot_install_header_file ( DesignOfExperimentEvaluationWizard.hxx )
ot_install_header_file ( StopCriteriaGroupBox.hxx )
ot_install_header_file ( ParametersDefinitionWizard.hxx )
ot_install_header_file ( MetaModelIntroPage.hxx )
ot_install_header_file ( KrigingPage.hxx )
ot_install_header_file ( FunctionalChaosPage.hxx )
ot_install_header_file ( MetaModelAnalysisWizard.hxx )
ot_install_header_file ( MetaModelValidationPage.hxx )
ot_install_header_file ( MetaModelValidationWidget.hxx )
ot_install_header_file ( FunctionalChaosResultWindow.hxx )
ot_install_header_file ( KrigingResultWindow.hxx )
ot_install_header_file ( InferenceResultWindow.hxx )
ot_install_header_file ( InferenceResultWizard.hxx )
ot_install_header_file ( InferenceResultWidget.hxx )
ot_install_header_file ( Controller.hxx )
ot_install_header_file ( ControllerWidget.hxx )
ot_install_header_file ( CopulaInferenceResultWidget.hxx )
ot_install_header_file ( CopulaInferenceResultWindow.hxx )
ot_install_header_file ( CopulaParametersTabWidget.hxx )
ot_install_header_file ( OptimizationIntroPage.hxx )
ot_install_header_file ( OptimizationWizard.hxx )
ot_install_header_file ( OptimizationResultWindow.hxx )
ot_install_header_file ( WidgetBoundToDockWidget.hxx )
ot_install_header_file ( MarginalsWidget.hxx )
ot_install_header_file ( DependenciesWidget.hxx )
ot_install_header_file ( CopulaWidget.hxx )
ot_install_header_file ( CopulaInferenceWizard.hxx )
ot_install_header_file ( CopulaInferenceResultWizard.hxx )
ot_install_header_file ( AboutDialog.hxx )
ot_install_header_file ( MeshDefinitionWizard.hxx )
ot_install_header_file ( FieldModelEvaluationResultWindow.hxx )
ot_install_header_file ( FieldMonteCarloWizard.hxx )
if (OTMORRIS_FOUND)
  ot_install_header_file ( ScreeningIntroPage.hxx )
  ot_install_header_file ( MorrisPage.hxx )
  ot_install_header_file ( ScreeningAnalysisWizard.hxx )
  ot_install_header_file ( MorrisResultWidget.hxx )
  ot_install_header_file ( MorrisResultWindow.hxx )
  ot_install_header_file ( ScreeningResultWizard.hxx )
endif ()

qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/MainWindow.hxx
                                  otgui/MainWidget.hxx
                                  otgui/Actions.hxx
                                  otgui/ToolBar.hxx
                                  otgui/MenuBar.hxx
                                  otgui/StatusBar.hxx
                                  otgui/SubWindow.hxx
                                  otgui/Wizard.hxx
                                  otgui/SubWindowsStackedWidget.hxx
                                  otgui/StudyManager.hxx
                                  otgui/WelcomeWindow.hxx
                                  otgui/StudyWindow.hxx
                                  otgui/MeshWindow.hxx
                                  otgui/PhysicalModelDiagramWindow.hxx
                                  otgui/FieldModelDiagramWindow.hxx
                                  otgui/DataModelDiagramWindow.hxx
                                  otgui/ResultWindow.hxx
                                  otgui/AnalysisWindow.hxx
                                  otgui/BlockSizeGroupBox.hxx
                                  otgui/MinMaxTableGroupBox.hxx
                                  otgui/MomentsEstimatesTableGroupBox.hxx
                                  otgui/OutputsSelectionGroupBox.hxx
                                  otgui/DataModelWindow.hxx
                                  otgui/DataAnalysisWindow.hxx
                                  otgui/DataAnalysisResultWindow.hxx
                                  otgui/StudyTreeView.hxx
                                  otgui/DesignOfExperimentIntroPage.hxx
                                  otgui/GridDesignPage.hxx
                                  otgui/ProbabilisticDesignPage.hxx
                                  otgui/ImportedDesignPage.hxx
                                  otgui/DesignOfExperimentInputWindow.hxx
                                  otgui/DesignOfExperimentEvaluationWizard.hxx
                                  otgui/PhysicalModelWindowWidget.hxx
                                  otgui/SymbolicPhysicalModelWindow.hxx
                                  otgui/PythonPhysicalModelWindow.hxx
                                  otgui/MetaModelWindow.hxx
                                  otgui/ProbabilisticModelWindow.hxx
                                  otgui/DesignOfExperimentWizard.hxx
                                  otgui/DesignOfExperimentWindow.hxx
                                  otgui/ModelEvaluationWizard.hxx
                                  otgui/ModelEvaluationResultWindow.hxx
                                  otgui/CentralTendencyIntroPage.hxx
                                  otgui/MonteCarloPage.hxx
                                  otgui/CentralTendencyWizard.hxx
                                  otgui/MonteCarloResultWindow.hxx
                                  otgui/TaylorExpansionMomentsResultWindow.hxx
                                  otgui/SensitivityResultWidget.hxx
                                  otgui/SobolResultWindow.hxx
                                  otgui/SRCResultWindow.hxx
                                  otgui/SensitivityIntroPage.hxx
                                  otgui/SobolPage.hxx
                                  otgui/SRCPage.hxx
                                  otgui/SensitivityAnalysisWizard.hxx
                                  otgui/LimitStateWindow.hxx
                                  otgui/ReliabilityIntroPage.hxx
                                  otgui/SimulationReliabilityPage.hxx
                                  otgui/ApproximationReliabilityPage.hxx
                                  otgui/ReliabilityAnalysisWizard.hxx
                                  otgui/SimulationReliabilityResultWindow.hxx
                                  otgui/ApproximationResultTabWidget.hxx
                                  otgui/ApproximationResultWindow.hxx
                                  otgui/StopCriteriaGroupBox.hxx
                                  otgui/ParametersDefinitionWizard.hxx
                                  otgui/MetaModelIntroPage.hxx
                                  otgui/MetaModelValidationPage.hxx
                                  otgui/KrigingPage.hxx
                                  otgui/FunctionalChaosPage.hxx
                                  otgui/MetaModelAnalysisWizard.hxx
                                  otgui/MetaModelValidationWidget.hxx
                                  otgui/FunctionalChaosResultWindow.hxx
                                  otgui/KrigingResultWindow.hxx
                                  otgui/InferenceResultWindow.hxx
                                  otgui/InferenceWizard.hxx
                                  otgui/DistributionsForInferenceWidget.hxx
                                  otgui/InferenceResultWizard.hxx
                                  otgui/InferenceResultWidget.hxx
                                  otgui/Controller.hxx
                                  otgui/ControllerWidget.hxx
                                  otgui/CopulaInferenceResultWidget.hxx
                                  otgui/CopulaInferenceResultWindow.hxx
                                  otgui/CopulaParametersTabWidget.hxx
                                  otgui/OptimizationIntroPage.hxx
                                  otgui/OptimizationWizard.hxx
                                  otgui/OptimizationResultWindow.hxx
                                  otgui/WidgetBoundToDockWidget.hxx
                                  otgui/MarginalsWidget.hxx
                                  otgui/DependenciesWidget.hxx
                                  otgui/CopulaWidget.hxx
                                  otgui/CopulaInferenceWizard.hxx
                                  otgui/CopulaInferenceResultWizard.hxx
                                  otgui/AboutDialog.hxx
                                  otgui/MeshDefinitionWizard.hxx
                                  otgui/FieldModelEvaluationResultWindow.hxx
                                  otgui/FieldMonteCarloWizard.hxx
                                  )


if (SalomeYACS_FOUND)
  qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/YACSPhysicalModelWindow.hxx)
endif ()
if (USE_OTFMI)
  qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/FMIPhysicalModelWindow.hxx)
endif ()
if (OTMORRIS_FOUND)
  qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/ScreeningIntroPage.hxx)
  qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/MorrisPage.hxx)
  qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/ScreeningAnalysisWizard.hxx)
  qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/MorrisResultWidget.hxx)
  qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/MorrisResultWindow.hxx)
  qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/ScreeningResultWizard.hxx)
endif ()

set(RESOURCES ${CMAKE_SOURCE_DIR}/gui.qrc)

qt5_add_resources (OTGUI_RESOURCES_RCC ${RESOURCES})

include_directories (${INTERNAL_INCLUDE_DIRS})
include_directories (${PYTHON_INCLUDE_DIRS})

otgui_add_library (otguiview ${SOURCEFILES} ${OTGUI_VIEW_MOC_SRCS} ${OTGUI_RESOURCES_RCC})
target_compile_features (otguiview PRIVATE cxx_lambdas)

if ( BUILD_SHARED_LIBS )
  set_target_properties ( otguiview PROPERTIES COMPILE_DEFINITIONS "OTGUI_DLL_EXPORTS" )
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties (otguiview PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties (otguiview PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries (otguiview otguimodel)
target_link_libraries (otguiview otguipyconsole)
target_link_libraries (otguiview otguiplot)

include_directories (${QWT_INCLUDE_DIRS})
target_link_libraries (otguiview ${QWT_LIBRARIES})


install ( TARGETS otguiview
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)

