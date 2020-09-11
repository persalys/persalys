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
ot_add_source_file ( DiagramWindow.cxx )
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
if (OTFMI_FOUND)
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
ot_add_source_file ( ExtractDataFieldWizard.cxx )
ot_add_source_file ( ImportSampleWidget.cxx )
ot_add_source_file ( ObservationsWizard.cxx )
ot_add_source_file ( CalibrationAnalysisWizard.cxx )
ot_add_source_file ( CalibrationResultWindow.cxx )
ot_add_source_file ( CouplingModelWindow.cxx )
ot_add_source_file ( DataCleaningWizard.cxx )
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
ot_install_header_file ( DiagramWindow.hxx )
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
if (OTFMI_FOUND)
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
ot_install_header_file ( ExtractDataFieldWizard.hxx )
ot_install_header_file ( ImportSampleWidget.hxx )
ot_install_header_file ( ObservationsWizard.hxx )
ot_install_header_file ( ObservationsWindow.hxx )
ot_install_header_file ( CalibrationAnalysisWizard.hxx )
ot_install_header_file ( CalibrationResultWindow.hxx )
ot_install_header_file ( CouplingModelWindow.hxx )
ot_install_header_file ( DataCleaningWizard.hxx )
if (OTMORRIS_FOUND)
  ot_install_header_file ( ScreeningIntroPage.hxx )
  ot_install_header_file ( MorrisPage.hxx )
  ot_install_header_file ( ScreeningAnalysisWizard.hxx )
  ot_install_header_file ( MorrisResultWidget.hxx )
  ot_install_header_file ( MorrisResultWindow.hxx )
  ot_install_header_file ( ScreeningResultWizard.hxx )
endif ()

qt5_wrap_cpp (PERSALYS_VIEW_MOC_SRCS persalys/MainWindow.hxx
                                  persalys/MainWidget.hxx
                                  persalys/Actions.hxx
                                  persalys/ToolBar.hxx
                                  persalys/MenuBar.hxx
                                  persalys/StatusBar.hxx
                                  persalys/SubWindow.hxx
                                  persalys/DiagramWindow.hxx
                                  persalys/Wizard.hxx
                                  persalys/SubWindowsStackedWidget.hxx
                                  persalys/StudyManager.hxx
                                  persalys/WelcomeWindow.hxx
                                  persalys/StudyWindow.hxx
                                  persalys/MeshWindow.hxx
                                  persalys/PhysicalModelDiagramWindow.hxx
                                  persalys/FieldModelDiagramWindow.hxx
                                  persalys/DataModelDiagramWindow.hxx
                                  persalys/ResultWindow.hxx
                                  persalys/AnalysisWindow.hxx
                                  persalys/BlockSizeGroupBox.hxx
                                  persalys/MinMaxTableGroupBox.hxx
                                  persalys/MomentsEstimatesTableGroupBox.hxx
                                  persalys/OutputsSelectionGroupBox.hxx
                                  persalys/DataModelWindow.hxx
                                  persalys/DataAnalysisWindow.hxx
                                  persalys/DataAnalysisResultWindow.hxx
                                  persalys/StudyTreeView.hxx
                                  persalys/DesignOfExperimentIntroPage.hxx
                                  persalys/GridDesignPage.hxx
                                  persalys/ProbabilisticDesignPage.hxx
                                  persalys/ImportedDesignPage.hxx
                                  persalys/DesignOfExperimentInputWindow.hxx
                                  persalys/DesignOfExperimentEvaluationWizard.hxx
                                  persalys/PhysicalModelWindowWidget.hxx
                                  persalys/SymbolicPhysicalModelWindow.hxx
                                  persalys/PythonPhysicalModelWindow.hxx
                                  persalys/MetaModelWindow.hxx
                                  persalys/ProbabilisticModelWindow.hxx
                                  persalys/DesignOfExperimentWizard.hxx
                                  persalys/DesignOfExperimentWindow.hxx
                                  persalys/ModelEvaluationWizard.hxx
                                  persalys/ModelEvaluationResultWindow.hxx
                                  persalys/CentralTendencyIntroPage.hxx
                                  persalys/MonteCarloPage.hxx
                                  persalys/CentralTendencyWizard.hxx
                                  persalys/MonteCarloResultWindow.hxx
                                  persalys/TaylorExpansionMomentsResultWindow.hxx
                                  persalys/SensitivityResultWidget.hxx
                                  persalys/SobolResultWindow.hxx
                                  persalys/SRCResultWindow.hxx
                                  persalys/SensitivityIntroPage.hxx
                                  persalys/SobolPage.hxx
                                  persalys/SRCPage.hxx
                                  persalys/SensitivityAnalysisWizard.hxx
                                  persalys/LimitStateWindow.hxx
                                  persalys/ReliabilityIntroPage.hxx
                                  persalys/SimulationReliabilityPage.hxx
                                  persalys/ApproximationReliabilityPage.hxx
                                  persalys/ReliabilityAnalysisWizard.hxx
                                  persalys/SimulationReliabilityResultWindow.hxx
                                  persalys/ApproximationResultTabWidget.hxx
                                  persalys/ApproximationResultWindow.hxx
                                  persalys/StopCriteriaGroupBox.hxx
                                  persalys/ParametersDefinitionWizard.hxx
                                  persalys/MetaModelIntroPage.hxx
                                  persalys/MetaModelValidationPage.hxx
                                  persalys/KrigingPage.hxx
                                  persalys/FunctionalChaosPage.hxx
                                  persalys/MetaModelAnalysisWizard.hxx
                                  persalys/MetaModelValidationWidget.hxx
                                  persalys/FunctionalChaosResultWindow.hxx
                                  persalys/KrigingResultWindow.hxx
                                  persalys/InferenceResultWindow.hxx
                                  persalys/InferenceWizard.hxx
                                  persalys/DistributionsForInferenceWidget.hxx
                                  persalys/InferenceResultWizard.hxx
                                  persalys/InferenceResultWidget.hxx
                                  persalys/Controller.hxx
                                  persalys/ControllerWidget.hxx
                                  persalys/CopulaInferenceResultWidget.hxx
                                  persalys/CopulaInferenceResultWindow.hxx
                                  persalys/CopulaParametersTabWidget.hxx
                                  persalys/OptimizationIntroPage.hxx
                                  persalys/OptimizationWizard.hxx
                                  persalys/OptimizationResultWindow.hxx
                                  persalys/WidgetBoundToDockWidget.hxx
                                  persalys/MarginalsWidget.hxx
                                  persalys/DependenciesWidget.hxx
                                  persalys/CopulaWidget.hxx
                                  persalys/CopulaInferenceWizard.hxx
                                  persalys/CopulaInferenceResultWizard.hxx
                                  persalys/AboutDialog.hxx
                                  persalys/MeshDefinitionWizard.hxx
                                  persalys/FieldModelEvaluationResultWindow.hxx
                                  persalys/FieldMonteCarloWizard.hxx
                                  persalys/ExtractDataFieldWizard.hxx
                                  persalys/ImportSampleWidget.hxx
                                  persalys/ObservationsWizard.hxx
                                  persalys/ObservationsWindow.hxx
                                  persalys/CalibrationAnalysisWizard.hxx
                                  persalys/CalibrationResultWindow.hxx
                                  persalys/CouplingModelWindow.hxx
                                  persalys/DataCleaningWizard.hxx
                                  )


if (SalomeYACS_FOUND)
  set_source_files_properties (AnalysisWindow.cxx YACSPhysicalModelWindow.cxx PROPERTIES COTIRE_EXCLUDED ON SKIP_UNITY_BUILD_INCLUSION ON) # FIXME: Python/Qt conflict due to YACS
  qt5_wrap_cpp (PERSALYS_VIEW_MOC_SRCS persalys/YACSPhysicalModelWindow.hxx)
endif ()

if (OTFMI_FOUND)
  qt5_wrap_cpp (PERSALYS_VIEW_MOC_SRCS persalys/FMIPhysicalModelWindow.hxx)
endif ()

if (OTMORRIS_FOUND)
  qt5_wrap_cpp (PERSALYS_VIEW_MOC_SRCS persalys/ScreeningIntroPage.hxx persalys/MorrisPage.hxx persalys/ScreeningAnalysisWizard.hxx persalys/MorrisResultWidget.hxx persalys/MorrisResultWindow.hxx persalys/ScreeningResultWizard.hxx)
endif ()

set(RESOURCES ${CMAKE_SOURCE_DIR}/persalys.qrc)

qt5_add_resources (PERSALYS_RESOURCES_RCC ${RESOURCES})

include_directories (${INTERNAL_INCLUDE_DIRS})
include_directories (${PYTHON_INCLUDE_DIRS})

persalys_add_library (persalysview ${SOURCEFILES} ${PERSALYS_VIEW_MOC_SRCS} ${PERSALYS_RESOURCES_RCC})
target_compile_features (persalysview PRIVATE cxx_lambdas)

if (BUILD_SHARED_LIBS)
  target_compile_definitions (persalysview PRIVATE PERSALYS_VIEW_DLL_EXPORTS)
endif ()

if ( NOT DEFINED LIB_VERSION )
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION )
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties (persalysview PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties (persalysview PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries (persalysview persalysmodel)
target_link_libraries (persalysview persalyspyconsole)
target_link_libraries (persalysview persalysplot)
if (SalomeYACS_FOUND)
  target_link_libraries (persalysview py2yacsguilib )
  target_link_libraries (persalysview ydefxgui)
endif ()

include_directories (${QWT_INCLUDE_DIRS})
target_link_libraries (persalysview ${QWT_LIBRARIES})


install ( TARGETS persalysview
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${PERSALYS_LIBRARY_PATH}
  ARCHIVE DESTINATION ${PERSALYS_LIBRARY_PATH}
)
