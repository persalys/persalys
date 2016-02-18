
add_subdirectory (pythonConsole)
add_subdirectory (plot)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( MainWindow.cxx )
ot_add_source_file ( OTguiToolBar.cxx )
ot_add_source_file ( OTguiMenuBar.cxx )
ot_add_source_file ( OTguiStatusBar.cxx )
ot_add_source_file ( OTguiSubWindow.cxx )
ot_add_source_file ( OTguiWizard.cxx )
ot_add_source_file ( OTguiMdiArea.cxx )
ot_add_source_file ( ComboBoxDelegate.cxx )
ot_add_source_file ( SpinBoxDelegate.cxx )
ot_add_source_file ( LineEditWithQValidatorDelegate.cxx )
ot_add_source_file ( ImportTablePage.cxx )
ot_add_source_file ( HorizontalHeaderViewWithCombobox.cxx )
ot_add_source_file ( DeterministicDesignPage.cxx )
ot_add_source_file ( ProbabilisticDesignPage.cxx )
ot_add_source_file ( PhysicalModelWindow.cxx )
ot_add_source_file ( ProbabilisticModelWindow.cxx )
ot_add_source_file ( DesignOfExperimentWizard.cxx )
ot_add_source_file ( DesignOfExperimentWindow.cxx )
ot_add_source_file ( StudyTreeView.cxx )
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

add_subdirectory (otgui)

qt4_wrap_cpp ( OTGUI_VIEW_MOC_SRCS otgui/MainWindow.hxx
                                   otgui/OTguiToolBar.hxx
                                   otgui/OTguiMenuBar.hxx
                                   otgui/OTguiStatusBar.hxx
                                   otgui/OTguiSubWindow.hxx
                                   otgui/OTguiWizard.hxx
                                   otgui/OTguiMdiArea.hxx
                                   otgui/ComboBoxDelegate.hxx
                                   otgui/SpinBoxDelegate.hxx
                                   otgui/LineEditWithQValidatorDelegate.hxx
                                   otgui/ImportTablePage.hxx
                                   otgui/StudyTreeView.hxx
                                   otgui/OTguiTableView.hxx
                                   otgui/HorizontalHeaderViewWithCombobox.hxx
                                   otgui/DeterministicDesignPage.hxx
                                   otgui/ProbabilisticDesignPage.hxx
                                   otgui/PhysicalModelWindow.hxx
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
                                   )

set(RESOURCES ${CMAKE_SOURCE_DIR}/gui.qrc)
qt4_add_resources(OTGUI_RESOURCES_RCC ${RESOURCES})

include_directories ( ${INTERNAL_INCLUDE_DIRS} )

add_library ( otguiview ${SOURCEFILES} ${OTGUI_VIEW_MOC_SRCS} ${OTGUI_RESOURCES_RCC} )

if ( BUILD_SHARED_LIBS )
  set_target_properties ( otguiview PROPERTIES COMPILE_DEFINITIONS "OTGUI_DLL_EXPORTS" )
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( otguiview PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( otguiview PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries ( otguiview otguimodel )
target_link_libraries ( otguiview otguipythonconsole )
target_link_libraries ( otguiview otguiplot )


find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(otguiview ${PYTHON_LIBRARIES})

find_package ( Qwt REQUIRED )
include_directories ( ${QWT_INCLUDE_DIRS} )
target_link_libraries( otguiview ${QWT_LIBRARIES} )


install ( TARGETS otguiview
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)

