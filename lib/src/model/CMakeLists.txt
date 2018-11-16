set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( SampleTableModel.cxx )
ot_add_source_file ( InputTableModel.cxx )
ot_add_source_file ( InputTableProbabilisticModel.cxx )
ot_add_source_file ( OutputTableModel.cxx )
ot_add_source_file ( ExperimentTableModel.cxx )
ot_add_source_file ( CorrelationTableModel.cxx )
ot_add_source_file ( DifferentiationTableModel.cxx )
ot_add_source_file ( DataModelTableModel.cxx )
ot_add_source_file ( DistributionsTableModel.cxx )
ot_add_source_file ( Item.cxx )
ot_add_source_file ( PhysicalModelDiagramItem.cxx )
ot_add_source_file ( PhysicalModelDefinitionItem.cxx )
ot_add_source_file ( LimitStateItem.cxx )
ot_add_source_file ( DesignOfExperimentItem.cxx )
ot_add_source_file ( DesignOfExperimentDefinitionItem.cxx )
ot_add_source_file ( DataModelDiagramItem.cxx )
ot_add_source_file ( DataModelDefinitionItem.cxx )
ot_add_source_file ( ProbabilisticModelItem.cxx )
ot_add_source_file ( PhysicalModelItem.cxx )
ot_add_source_file ( StudyItem.cxx )
ot_add_source_file ( AnalysisItem.cxx )
ot_add_source_file ( StudyTreeViewModel.cxx )
ot_add_source_file ( CodeModel.cxx)
ot_add_source_file ( VariablesSelectionTableModel.cxx)
ot_add_source_file ( CustomStandardItemModel.cxx )
ot_add_source_file ( DependenciesTableModel.cxx )
ot_add_source_file ( OptimizationTableModel.cxx )
ot_add_source_file ( TranslationManager.cxx )
ot_add_source_file ( FileTools.cxx )
if (OTMORRIS_FOUND)
  ot_add_source_file ( MorrisTableModel.cxx )
  ot_add_source_file ( MorrisResultTableModel.cxx )
endif ()

ot_install_header_file ( SampleTableModel.hxx )
ot_install_header_file ( InputTableModel.hxx )
ot_install_header_file ( InputTableProbabilisticModel.hxx )
ot_install_header_file ( OutputTableModel.hxx )
ot_install_header_file ( ExperimentTableModel.hxx )
ot_install_header_file ( CorrelationTableModel.hxx )
ot_install_header_file ( DifferentiationTableModel.hxx )
ot_install_header_file ( Item.hxx )
ot_install_header_file ( PhysicalModelDiagramItem.hxx )
ot_install_header_file ( PhysicalModelDefinitionItem.hxx )
ot_install_header_file ( AnalysisItem.hxx )
ot_install_header_file ( LimitStateItem.hxx )
ot_install_header_file ( DesignOfExperimentItem.hxx )
ot_install_header_file ( DesignOfExperimentDefinitionItem.hxx )
ot_install_header_file ( DataModelDiagramItem.hxx )
ot_install_header_file ( DataModelDefinitionItem.hxx )
ot_install_header_file ( ProbabilisticModelItem.hxx )
ot_install_header_file ( PhysicalModelItem.hxx )
ot_install_header_file ( StudyItem.hxx )
ot_install_header_file ( StudyTreeViewModel.hxx )
ot_install_header_file ( VariablesSelectionTableModel.hxx )
ot_install_header_file ( CustomStandardItemModel.hxx )
ot_install_header_file ( DependenciesTableModel.hxx )
ot_install_header_file ( OptimizationTableModel.hxx )
ot_install_header_file ( TranslationManager.hxx )
ot_install_header_file ( FileTools.hxx )
if (OTMORRIS_FOUND)
ot_install_header_file ( MorrisTableModel.hxx )
ot_install_header_file ( MorrisResultTableModel.hxx )
endif ()

qt5_wrap_cpp ( OTGUI_MODEL_MOC_SRCS otgui/InputTableModel.hxx
                                    otgui/InputTableProbabilisticModel.hxx
                                    otgui/OutputTableModel.hxx
                                    otgui/SampleTableModel.hxx
                                    otgui/ExperimentTableModel.hxx
                                    otgui/CorrelationTableModel.hxx
                                    otgui/DifferentiationTableModel.hxx
                                    otgui/DataModelTableModel.hxx
                                    otgui/DistributionsTableModel.hxx
                                    otgui/Item.hxx
                                    otgui/PhysicalModelDiagramItem.hxx
                                    otgui/PhysicalModelDefinitionItem.hxx
                                    otgui/AnalysisItem.hxx
                                    otgui/StudyTreeViewModel.hxx
                                    otgui/LimitStateItem.hxx
                                    otgui/DesignOfExperimentItem.hxx
                                    otgui/DesignOfExperimentDefinitionItem.hxx
                                    otgui/DataModelDiagramItem.hxx
                                    otgui/DataModelDefinitionItem.hxx
                                    otgui/ProbabilisticModelItem.hxx
                                    otgui/PhysicalModelItem.hxx
                                    otgui/StudyItem.hxx
                                    otgui/CodeModel.hxx
                                    otgui/VariablesSelectionTableModel.hxx
                                    otgui/CustomStandardItemModel.hxx
                                    otgui/DependenciesTableModel.hxx
                                    otgui/OptimizationTableModel.hxx
                                    otgui/TranslationManager.hxx)

if (OTMORRIS_FOUND)
  qt5_wrap_cpp (OTGUI_MODEL_MOC_SRCS otgui/MorrisTableModel.hxx)
  qt5_wrap_cpp (OTGUI_MODEL_MOC_SRCS otgui/MorrisResultTableModel.hxx)
endif ()

include_directories ( ${INTERNAL_INCLUDE_DIRS} )

otgui_add_library ( otguimodel ${SOURCEFILES} ${OTGUI_MODEL_MOC_SRCS} )
if ( BUILD_SHARED_LIBS )
  set_target_properties ( otguimodel PROPERTIES COMPILE_DEFINITIONS "OTGUI_DLL_EXPORTS" )
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( otguimodel PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( otguimodel PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries ( otguimodel otguibase ${QT_LIBRARIES})

install ( TARGETS otguimodel
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)

