
add_subdirectory (pythonConsole)
add_subdirectory (plot)

set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( MainWindow.cxx )
ot_add_source_file ( CollapsibleGroupBox.cxx )
ot_add_source_file ( OTguiToolBar.cxx )
ot_add_source_file ( OTguiMenuBar.cxx )
ot_add_source_file ( OTguiStatusBar.cxx )
ot_add_source_file ( OTguiSubWindow.cxx )
ot_add_source_file ( OTguiWizard.cxx )
ot_add_source_file ( OTguiMdiArea.cxx )
ot_add_source_file ( NotEditableTableWidget.cxx )
ot_add_source_file ( AnalysisExecutionFailedWindow.cxx )
ot_add_source_file ( ComboBoxDelegate.cxx )
ot_add_source_file ( SpinBoxDelegate.cxx )
ot_add_source_file ( LineEditWithQValidatorDelegate.cxx )
ot_add_source_file ( CustomDoubleValidator.cxx )
ot_add_source_file ( ImportTablePage.cxx )
ot_add_source_file ( HorizontalHeaderViewWithCombobox.cxx )
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
ot_add_source_file ( DataTableView.cxx )
ot_add_source_file ( OTguiTableView.cxx )
ot_add_source_file ( ModelEvaluationWizard.cxx )
ot_add_source_file ( ModelEvaluationResultWindow.cxx )
ot_add_source_file ( CentralTendencyWizard.cxx )
ot_add_source_file ( MonteCarloResultWindow.cxx )
ot_add_source_file ( TaylorExpansionsMomentsResultWindow.cxx )
ot_add_source_file ( SobolResultWindow.cxx )
ot_add_source_file ( SRCResultWindow.cxx )
ot_add_source_file ( SensitivityAnalysisWizard.cxx )
ot_add_source_file ( LimitStateWindow.cxx )
ot_add_source_file ( ReliabilityAnalysisWizard.cxx )
ot_add_source_file ( MonteCarloReliabilityResultWindow.cxx )
ot_add_source_file ( CodeDelegate.cxx )
ot_add_source_file ( DataTableWidget.cxx )

ot_install_header_file ( MainWindow.hxx )
ot_install_header_file ( CollapsibleGroupBox.hxx )
ot_install_header_file ( OTguiToolBar.hxx )
ot_install_header_file ( OTguiMenuBar.hxx )
ot_install_header_file ( OTguiStatusBar.hxx )
ot_install_header_file ( OTguiWizard.hxx )
ot_install_header_file ( OTguiMdiArea.hxx )
ot_install_header_file ( NotEditableTableWidget.hxx )
ot_install_header_file ( AnalysisExecutionFailedWindow.hxx )
ot_install_header_file ( ComboBoxDelegate.hxx )
ot_install_header_file ( SpinBoxDelegate.hxx )
ot_install_header_file ( LineEditWithQValidatorDelegate.hxx )
ot_install_header_file ( CustomDoubleValidator.hxx )
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
ot_install_header_file ( DataTableView.hxx )
ot_install_header_file ( OTguiTableView.hxx )
ot_install_header_file ( ModelEvaluationResultWindow.hxx )
ot_install_header_file ( MonteCarloResultWindow.hxx )
ot_install_header_file ( TaylorExpansionsMomentsResultWindow.hxx )
ot_install_header_file ( SobolResultWindow.hxx )
ot_install_header_file ( SRCResultWindow.hxx )
ot_install_header_file ( MonteCarloReliabilityResultWindow.hxx )
ot_install_header_file ( StudyTreeView.hxx )
ot_install_header_file ( ImportTablePage.hxx )
ot_install_header_file ( HorizontalHeaderViewWithCombobox.hxx )
ot_install_header_file ( DeterministicDesignPage.hxx )
ot_install_header_file ( ProbabilisticDesignPage.hxx )
ot_install_header_file ( CodeDelegate.hxx )
ot_install_header_file ( DataTableWidget.hxx )

qt5_wrap_cpp (OTGUI_VIEW_MOC_SRCS otgui/MainWindow.hxx
                                  otgui/CollapsibleGroupBox.hxx
                                  otgui/OTguiToolBar.hxx
                                  otgui/OTguiMenuBar.hxx
                                  otgui/OTguiStatusBar.hxx
                                  otgui/OTguiSubWindow.hxx
                                  otgui/OTguiWizard.hxx
                                  otgui/OTguiMdiArea.hxx
                                  otgui/NotEditableTableWidget.hxx
                                  otgui/ComboBoxDelegate.hxx
                                  otgui/SpinBoxDelegate.hxx
                                  otgui/LineEditWithQValidatorDelegate.hxx
                                  otgui/CustomDoubleValidator.hxx
                                  otgui/ImportTablePage.hxx
                                  otgui/StudyTreeView.hxx
                                  otgui/DataTableView.hxx
                                  otgui/OTguiTableView.hxx
                                  otgui/HorizontalHeaderViewWithCombobox.hxx
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
                                  otgui/TaylorExpansionsMomentsResultWindow.hxx
                                  otgui/SobolResultWindow.hxx
                                  otgui/SRCResultWindow.hxx
                                  otgui/SensitivityAnalysisWizard.hxx
                                  otgui/LimitStateWindow.hxx
                                  otgui/ReliabilityAnalysisWizard.hxx
                                  otgui/MonteCarloReliabilityResultWindow.hxx
                                  otgui/CodeDelegate.hxx
                                  otgui/DataTableWidget.hxx
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

