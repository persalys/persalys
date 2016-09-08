
add_subdirectory (pythonConsole)
add_subdirectory (utils)
add_subdirectory (plot)

set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( MainWindow.cxx )
ot_add_source_file ( OTguiToolBar.cxx )
ot_add_source_file ( OTguiMenuBar.cxx )
ot_add_source_file ( OTguiStatusBar.cxx )
ot_add_source_file ( OTguiSubWindow.cxx )
ot_add_source_file ( OTguiWizard.cxx )
ot_add_source_file ( ImportDataPage.cxx )
ot_add_source_file ( ImportDataModelPage.cxx )
ot_add_source_file ( ImportDesignOfExperimentPage.cxx )
ot_add_source_file ( DataModelWizard.cxx )
ot_add_source_file ( DataModelWindow.cxx )
ot_add_source_file ( DataAnalysisWindow.cxx )
ot_add_source_file ( DataAnalysisResultWindow.cxx )
ot_add_source_file ( AnalysisWizard.cxx )
ot_add_source_file ( OTguiMdiArea.cxx )
ot_add_source_file ( BlockSizeGroupBox.cxx )
ot_add_source_file ( MinMaxTableGroupBox.cxx )
ot_add_source_file ( MomentsEstimatesTableGroupBox.cxx )
ot_add_source_file ( ResultWindow.cxx )
ot_add_source_file ( AnalysisExecutionFailedWindow.cxx )
ot_add_source_file ( ImportTablePage.cxx )
ot_add_source_file ( DeterministicDesignPage.cxx )
ot_add_source_file ( ProbabilisticDesignPage.cxx )
ot_add_source_file ( PhysicalModelWindowWidget.cxx )
ot_add_source_file ( AnalyticalPhysicalModelWindow.cxx )
ot_add_source_file ( PythonPhysicalModelWindow.cxx )
if (SalomeYACS_FOUND)
  ot_add_source_file ( YACSPhysicalModelWindow.cxx)
endif ()
ot_add_source_file ( ProbabilisticModelWindow.cxx )
ot_add_source_file ( DesignOfExperimentWizard.cxx )
ot_add_source_file ( DesignOfExperimentWindow.cxx )
ot_add_source_file ( StudyTreeView.cxx )
ot_add_source_file ( ModelEvaluationWizard.cxx )
ot_add_source_file ( ModelEvaluationResultWindow.cxx )
ot_add_source_file ( CentralTendencyWizard.cxx )
ot_add_source_file ( MonteCarloResultWindow.cxx )
ot_add_source_file ( TaylorExpansionMomentsResultWindow.cxx )
ot_add_source_file ( SobolResultWindow.cxx )
ot_add_source_file ( SRCResultWindow.cxx )
ot_add_source_file ( SensitivityAnalysisWizard.cxx )
ot_add_source_file ( LimitStateWindow.cxx )
ot_add_source_file ( ReliabilityAnalysisWizard.cxx )
ot_add_source_file ( MonteCarloReliabilityResultWindow.cxx )
ot_add_source_file ( StopCriteriaGroupBox.cxx )

ot_install_header_file ( MainWindow.hxx )
ot_install_header_file ( OTguiToolBar.hxx )
ot_install_header_file ( OTguiMenuBar.hxx )
ot_install_header_file ( OTguiStatusBar.hxx )
ot_install_header_file ( OTguiWizard.hxx )
ot_install_header_file ( AnalysisWizard.hxx )
ot_install_header_file ( OTguiMdiArea.hxx )
ot_install_header_file ( AnalysisExecutionFailedWindow.hxx )
ot_install_header_file ( ModelEvaluationWizard.hxx )
ot_install_header_file ( CentralTendencyWizard.hxx )
ot_install_header_file ( SensitivityAnalysisWizard.hxx )
ot_install_header_file ( LimitStateWindow.hxx )
ot_install_header_file ( ReliabilityAnalysisWizard.hxx )
ot_install_header_file ( OTguiSubWindow.hxx )
ot_install_header_file ( PhysicalModelWindowWidget.hxx )
ot_install_header_file ( AnalyticalPhysicalModelWindow.hxx )
ot_install_header_file ( PythonPhysicalModelWindow.hxx )
if (SalomeYACS_FOUND)
  ot_install_header_file (YACSPhysicalModelWindow.hxx)
endif ()
ot_install_header_file ( ProbabilisticModelWindow.hxx )
ot_install_header_file ( DesignOfExperimentWizard.hxx )
ot_install_header_file ( DesignOfExperimentWindow.hxx )
ot_install_header_file ( ModelEvaluationResultWindow.hxx )
ot_install_header_file ( MonteCarloResultWindow.hxx )
ot_install_header_file ( TaylorExpansionMomentsResultWindow.hxx )
ot_install_header_file ( SobolResultWindow.hxx )
ot_install_header_file ( SRCResultWindow.hxx )
ot_install_header_file ( MonteCarloReliabilityResultWindow.hxx )
ot_install_header_file ( StudyTreeView.hxx )
ot_install_header_file ( ImportTablePage.hxx )
ot_install_header_file ( DeterministicDesignPage.hxx )
ot_install_header_file ( ProbabilisticDesignPage.hxx )
ot_install_header_file ( StopCriteriaGroupBox.hxx )

qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/MainWindow.hxx
                                  otgui/OTguiToolBar.hxx
                                  otgui/OTguiMenuBar.hxx
                                  otgui/OTguiStatusBar.hxx
                                  otgui/OTguiSubWindow.hxx
                                  otgui/OTguiWizard.hxx
                                  otgui/OTguiMdiArea.hxx
                                  otgui/ResultWindow.hxx
                                  otgui/BlockSizeGroupBox.hxx
                                  otgui/MinMaxTableGroupBox.hxx
                                  otgui/MomentsEstimatesTableGroupBox.hxx
                                  otgui/ImportDataPage.hxx
                                  otgui/ImportDataModelPage.hxx
                                  otgui/ImportDesignOfExperimentPage.hxx
                                  otgui/DataModelWizard.hxx
                                  otgui/DataModelWindow.hxx
                                  otgui/DataAnalysisWindow.hxx
                                  otgui/DataAnalysisResultWindow.hxx
                                  otgui/ImportTablePage.hxx
                                  otgui/StudyTreeView.hxx
                                  otgui/DeterministicDesignPage.hxx
                                  otgui/ProbabilisticDesignPage.hxx
                                  otgui/PhysicalModelWindowWidget.hxx
                                  otgui/AnalyticalPhysicalModelWindow.hxx
                                  otgui/PythonPhysicalModelWindow.hxx
                                  otgui/ProbabilisticModelWindow.hxx
                                  otgui/DesignOfExperimentWizard.hxx
                                  otgui/DesignOfExperimentWindow.hxx
                                  otgui/ModelEvaluationWizard.hxx
                                  otgui/ModelEvaluationResultWindow.hxx
                                  otgui/CentralTendencyWizard.hxx
                                  otgui/MonteCarloResultWindow.hxx
                                  otgui/TaylorExpansionMomentsResultWindow.hxx
                                  otgui/SobolResultWindow.hxx
                                  otgui/SRCResultWindow.hxx
                                  otgui/SensitivityAnalysisWizard.hxx
                                  otgui/LimitStateWindow.hxx
                                  otgui/ReliabilityAnalysisWizard.hxx
                                  otgui/MonteCarloReliabilityResultWindow.hxx
                                  otgui/StopCriteriaGroupBox.hxx
                                  )


if (SalomeYACS_FOUND)
  qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/YACSPhysicalModelWindow.hxx)
endif ()

set(RESOURCES ${CMAKE_SOURCE_DIR}/gui.qrc)

qt5_add_resources (OTGUI_RESOURCES_RCC ${RESOURCES})

include_directories (${INTERNAL_INCLUDE_DIRS})


add_library (otguiview ${SOURCEFILES} ${OTGUI_VIEW_MOC_SRCS} ${OTGUI_RESOURCES_RCC})

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
target_link_libraries (otguiview otguipythonconsole)
target_link_libraries (otguiview otguiplot)

include_directories (${QWT_INCLUDE_DIRS})
target_link_libraries (otguiview ${QWT_LIBRARIES})


install ( TARGETS otguiview
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)

