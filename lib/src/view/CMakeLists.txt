add_subdirectory (PyInterp)
add_subdirectory (PyConsole)
add_subdirectory (utils)
add_subdirectory (plot)

set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( MainWindow.cxx )
ot_add_source_file ( MainWidget.cxx )
ot_add_source_file ( OTguiToolBar.cxx )
ot_add_source_file ( OTguiActions.cxx )
ot_add_source_file ( OTguiMenuBar.cxx )
ot_add_source_file ( OTguiStatusBar.cxx )
ot_add_source_file ( OTguiSubWindow.cxx )
ot_add_source_file ( OTguiWizard.cxx )
ot_add_source_file ( OTguiWizardPage.cxx )
ot_add_source_file ( StudyManager.cxx )
ot_add_source_file ( WindowFactory.cxx )
ot_add_source_file ( PhysicalModelDiagramWindow.cxx )
ot_add_source_file ( DataModelDiagramWindow.cxx )
ot_add_source_file ( DataModelWindow.cxx )
ot_add_source_file ( DataAnalysisWindow.cxx )
ot_add_source_file ( DataAnalysisResultWindow.cxx )
ot_add_source_file ( AnalysisWizard.cxx )
ot_add_source_file ( OTguiMdiArea.cxx )
ot_add_source_file ( WelcomeWindow.cxx )
ot_add_source_file ( OTStudyWindow.cxx )
ot_add_source_file ( BlockSizeGroupBox.cxx )
ot_add_source_file ( MinMaxTableGroupBox.cxx )
ot_add_source_file ( MomentsEstimatesTableGroupBox.cxx )
ot_add_source_file ( OutputsSelectionGroupBox.cxx )
ot_add_source_file ( ParametersWidget.cxx )
ot_add_source_file ( ResultWindow.cxx )
ot_add_source_file ( AnalysisWindow.cxx )
ot_add_source_file ( IntroDesignOfExperimentPage.cxx )
ot_add_source_file ( DeterministicDesignPage.cxx )
ot_add_source_file ( ProbabilisticDesignPage.cxx )
ot_add_source_file ( DesignOfExperimentInputWindow.cxx )
ot_add_source_file ( DesignOfExperimentEvaluationWizard.cxx )
ot_add_source_file ( ImportDesignOfExperimentPage.cxx )
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
ot_add_source_file ( IntroCentralTendencyPage.cxx )
ot_add_source_file ( MonteCarloPage.cxx )
ot_add_source_file ( CentralTendencyWizard.cxx )
ot_add_source_file ( MonteCarloResultWindow.cxx )
ot_add_source_file ( TaylorExpansionMomentsResultWindow.cxx )
ot_add_source_file ( SensitivityResultWidget.cxx )
ot_add_source_file ( SobolResultWindow.cxx )
ot_add_source_file ( SRCResultWindow.cxx )
ot_add_source_file ( IntroSensitivityPage.cxx )
ot_add_source_file ( SobolPage.cxx )
ot_add_source_file ( SRCPage.cxx )
ot_add_source_file ( SensitivityAnalysisWizard.cxx )
ot_add_source_file ( LimitStateWindow.cxx )
ot_add_source_file ( IntroReliabilityPage.cxx )
ot_add_source_file ( SimulationReliabilityPage.cxx )
ot_add_source_file ( ApproximationReliabilityPage.cxx )
ot_add_source_file ( ReliabilityAnalysisWizard.cxx )
ot_add_source_file ( SimulationReliabilityResultWindow.cxx )
ot_add_source_file ( ApproximationResultWindow.cxx )
ot_add_source_file ( ApproximationResultTabWidget.cxx )
ot_add_source_file ( StopCriteriaGroupBox.cxx )
ot_add_source_file ( ParametersDefinitionWizard.cxx )
ot_add_source_file ( IntroMetaModelPage.cxx )
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
ot_add_source_file ( WidgetBoundToDockWidget.cxx )
if (OTMORRIS_FOUND)
  ot_add_source_file ( IntroScreeningPage.cxx )
  ot_add_source_file ( MorrisPage.cxx )
  ot_add_source_file ( ScreeningAnalysisWizard.cxx )
  ot_add_source_file ( MorrisResultWidget.cxx )
  ot_add_source_file ( MorrisResultWindow.cxx )
  ot_add_source_file ( ScreeningResultWizard.cxx )
endif ()

ot_install_header_file ( MainWindow.hxx )
ot_install_header_file ( MainWidget.hxx )
ot_install_header_file ( OTguiActions.hxx )
ot_install_header_file ( OTguiToolBar.hxx )
ot_install_header_file ( OTguiMenuBar.hxx )
ot_install_header_file ( OTguiStatusBar.hxx )
ot_install_header_file ( OTguiWizard.hxx )
ot_install_header_file ( OTguiWizardPage.hxx )
ot_install_header_file ( StudyManager.hxx )
ot_install_header_file ( WindowFactory.hxx )
ot_install_header_file ( AnalysisWizard.hxx )
ot_install_header_file ( OTguiMdiArea.hxx )
ot_install_header_file ( WelcomeWindow.hxx )
ot_install_header_file ( OTStudyWindow.hxx )
ot_install_header_file ( PhysicalModelDiagramWindow.hxx )
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
ot_install_header_file ( IntroCentralTendencyPage.hxx )
ot_install_header_file ( MonteCarloPage.hxx )
ot_install_header_file ( CentralTendencyWizard.hxx )
ot_install_header_file ( IntroSensitivityPage.hxx )
ot_install_header_file ( SobolPage.hxx )
ot_install_header_file ( SRCPage.hxx )
ot_install_header_file ( SensitivityAnalysisWizard.hxx )
ot_install_header_file ( LimitStateWindow.hxx )
ot_install_header_file ( IntroReliabilityPage.hxx )
ot_install_header_file ( SimulationReliabilityPage.hxx )
ot_install_header_file ( ApproximationReliabilityPage.hxx )
ot_install_header_file ( ReliabilityAnalysisWizard.hxx )
ot_install_header_file ( OTguiSubWindow.hxx )
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
ot_install_header_file ( IntroDesignOfExperimentPage.hxx )
ot_install_header_file ( DeterministicDesignPage.hxx )
ot_install_header_file ( ProbabilisticDesignPage.hxx )
ot_install_header_file ( ImportDesignOfExperimentPage.hxx )
ot_install_header_file ( DesignOfExperimentInputWindow.hxx )
ot_install_header_file ( DesignOfExperimentEvaluationWizard.hxx )
ot_install_header_file ( StopCriteriaGroupBox.hxx )
ot_install_header_file ( ParametersDefinitionWizard.hxx )
ot_install_header_file ( IntroMetaModelPage.hxx )
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
ot_install_header_file ( WidgetBoundToDockWidget.hxx )

if (OTMORRIS_FOUND)
  ot_install_header_file ( IntroScreeningPage.hxx )
  ot_install_header_file ( MorrisPage.hxx )
  ot_install_header_file ( ScreeningAnalysisWizard.hxx )
  ot_install_header_file ( MorrisResultWidget.hxx )
  ot_install_header_file ( MorrisResultWindow.hxx )
  ot_install_header_file ( ScreeningResultWizard.hxx )
endif ()

qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/MainWindow.hxx
                                  otgui/MainWidget.hxx
                                  otgui/OTguiActions.hxx
                                  otgui/OTguiToolBar.hxx
                                  otgui/OTguiMenuBar.hxx
                                  otgui/OTguiStatusBar.hxx
                                  otgui/OTguiSubWindow.hxx
                                  otgui/OTguiWizard.hxx
                                  otgui/OTguiWizardPage.hxx
                                  otgui/OTguiMdiArea.hxx
                                  otgui/StudyManager.hxx
                                  otgui/WelcomeWindow.hxx
                                  otgui/OTStudyWindow.hxx
                                  otgui/PhysicalModelDiagramWindow.hxx
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
                                  otgui/IntroDesignOfExperimentPage.hxx
                                  otgui/DeterministicDesignPage.hxx
                                  otgui/ProbabilisticDesignPage.hxx
                                  otgui/ImportDesignOfExperimentPage.hxx
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
                                  otgui/IntroCentralTendencyPage.hxx
                                  otgui/MonteCarloPage.hxx
                                  otgui/CentralTendencyWizard.hxx
                                  otgui/MonteCarloResultWindow.hxx
                                  otgui/TaylorExpansionMomentsResultWindow.hxx
                                  otgui/SensitivityResultWidget.hxx
                                  otgui/SobolResultWindow.hxx
                                  otgui/SRCResultWindow.hxx
                                  otgui/IntroSensitivityPage.hxx
                                  otgui/SobolPage.hxx
                                  otgui/SRCPage.hxx
                                  otgui/SensitivityAnalysisWizard.hxx
                                  otgui/LimitStateWindow.hxx
                                  otgui/IntroReliabilityPage.hxx
                                  otgui/SimulationReliabilityPage.hxx
                                  otgui/ApproximationReliabilityPage.hxx
                                  otgui/ReliabilityAnalysisWizard.hxx
                                  otgui/SimulationReliabilityResultWindow.hxx
                                  otgui/ApproximationResultTabWidget.hxx
                                  otgui/ApproximationResultWindow.hxx
                                  otgui/StopCriteriaGroupBox.hxx
                                  otgui/ParametersDefinitionWizard.hxx
                                  otgui/IntroMetaModelPage.hxx
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
                                  otgui/WidgetBoundToDockWidget.hxx
                                  )


if (SalomeYACS_FOUND)
  qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/YACSPhysicalModelWindow.hxx)
endif ()
if (USE_OTFMI)
  qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/FMIPhysicalModelWindow.hxx)
endif ()
if (OTMORRIS_FOUND)
  qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/IntroScreeningPage.hxx)
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

