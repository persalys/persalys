set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( SampleTableModel.cxx )
ot_add_source_file ( InputTableModel.cxx )
ot_add_source_file ( EnvironmentTableModel.cxx )
ot_add_source_file ( InputTableProbabilisticModel.cxx )
ot_add_source_file ( OutputTableModel.cxx )
ot_add_source_file ( ExperimentTableModel.cxx )
ot_add_source_file ( CorrelationTableModel.cxx )
ot_add_source_file ( DifferentiationTableModel.cxx )
ot_add_source_file ( GradientTableModel.cxx )
ot_add_source_file ( DataModelTableModel.cxx )
ot_add_source_file ( DataCleaningTableModel.cxx )
ot_add_source_file ( DistributionsTableModel.cxx )
ot_add_source_file ( CalibrationTableModel.cxx )
ot_add_source_file ( ItemFactory.cxx )
ot_add_source_file ( Item.cxx )
ot_add_source_file ( MeshItem.cxx )
ot_add_source_file ( DataMeshItem.cxx )
ot_add_source_file ( PhysicalModelDiagramItem.cxx )
ot_add_source_file ( PhysicalModelDefinitionItem.cxx )
ot_add_source_file ( LimitStateItem.cxx )
ot_add_source_file ( DesignOfExperimentItem.cxx )
ot_add_source_file ( DesignOfExperimentDefinitionItem.cxx )
ot_add_source_file ( DataModelDiagramItem.cxx )
ot_add_source_file ( DataFieldModelDiagramItem.cxx )
ot_add_source_file ( DataModelDefinitionItem.cxx )
ot_add_source_file ( DataFieldModelDefinitionItem.cxx )
ot_add_source_file ( ProbabilisticModelItem.cxx )
ot_add_source_file ( PhysicalModelItem.cxx )
ot_add_source_file ( DataFieldModelItem.cxx )
ot_add_source_file ( StudyItem.cxx )
ot_add_source_file ( AnalysisItem.cxx )
ot_add_source_file ( ObservationsItem.cxx )
ot_add_source_file ( StudyTreeViewModel.cxx )
ot_add_source_file ( CodeModel.cxx)
ot_add_source_file ( VariablesSelectionTableModel.cxx)
ot_add_source_file ( CustomStandardItemModel.cxx )
ot_add_source_file ( DependenciesTableModel.cxx )
ot_add_source_file ( OptimizationTableModel.cxx )
ot_add_source_file ( MultiObjectiveOptimizationTableModel.cxx )
ot_add_source_file ( ConstraintsTableModel.cxx )
ot_add_source_file ( ObjectivesTableModel.cxx )
ot_add_source_file ( TranslationManager.cxx )
ot_add_source_file ( FileTools.cxx )
ot_add_source_file ( AnsysSystemTableModel.cxx )
ot_add_source_file ( AnsysVariableTableModel.cxx )
ot_add_source_file ( QtTools.cxx )

if (OTMORRIS_FOUND)
  ot_add_source_file ( MorrisTableModel.cxx )
  ot_add_source_file ( MorrisResultTableModel.cxx )
endif ()

ot_install_header_file ( SampleTableModel.hxx )
ot_install_header_file ( InputTableModel.hxx )
ot_install_header_file ( EnvironmentTableModel.hxx )
ot_install_header_file ( InputTableProbabilisticModel.hxx )
ot_install_header_file ( OutputTableModel.hxx )
ot_install_header_file ( ExperimentTableModel.hxx )
ot_install_header_file ( CorrelationTableModel.hxx )
ot_install_header_file ( DifferentiationTableModel.hxx )
ot_install_header_file ( GradientTableModel.hxx )
ot_install_header_file ( CalibrationTableModel.hxx )
ot_install_header_file ( ItemFactory.hxx )
ot_install_header_file ( Item.hxx )
ot_install_header_file ( MeshItem.hxx )
ot_install_header_file ( DataMeshItem.hxx )
ot_install_header_file ( PhysicalModelDiagramItem.hxx )
ot_install_header_file ( PhysicalModelDefinitionItem.hxx )
ot_install_header_file ( AnalysisItem.hxx )
ot_install_header_file ( LimitStateItem.hxx )
ot_install_header_file ( DesignOfExperimentItem.hxx )
ot_install_header_file ( DesignOfExperimentDefinitionItem.hxx )
ot_install_header_file ( DataModelDiagramItem.hxx )
ot_install_header_file ( DataFieldModelDiagramItem.hxx )
ot_install_header_file ( DataModelDefinitionItem.hxx )
ot_install_header_file ( DataFieldModelDefinitionItem.hxx )
ot_install_header_file ( ProbabilisticModelItem.hxx )
ot_install_header_file ( PhysicalModelItem.hxx )
ot_install_header_file ( DataFieldModelItem.hxx )
ot_install_header_file ( StudyItem.hxx )
ot_install_header_file ( ObservationsItem.hxx )
ot_install_header_file ( StudyTreeViewModel.hxx )
ot_install_header_file ( VariablesSelectionTableModel.hxx )
ot_install_header_file ( CustomStandardItemModel.hxx )
ot_install_header_file ( DependenciesTableModel.hxx )
ot_install_header_file ( OptimizationTableModel.hxx )
ot_install_header_file ( MultiObjectiveOptimizationTableModel.hxx )
ot_install_header_file ( ConstraintsTableModel.hxx )
ot_install_header_file ( ObjectivesTableModel.hxx )
ot_install_header_file ( TranslationManager.hxx )
ot_install_header_file ( FileTools.hxx )
ot_install_header_file ( AnsysSystemTableModel.hxx )
ot_install_header_file ( AnsysVariableTableModel.hxx )
ot_install_header_file ( QtTools.hxx )
if (OTMORRIS_FOUND)
ot_install_header_file ( MorrisTableModel.hxx )
ot_install_header_file ( MorrisResultTableModel.hxx )
endif ()

qt_wrap_cpp ( PERSALYS_MODEL_MOC_SRCS persalys/InputTableModel.hxx
                                    persalys/EnvironmentTableModel.hxx
                                    persalys/InputTableProbabilisticModel.hxx
                                    persalys/OutputTableModel.hxx
                                    persalys/SampleTableModel.hxx
                                    persalys/ExperimentTableModel.hxx
                                    persalys/CorrelationTableModel.hxx
                                    persalys/DifferentiationTableModel.hxx
                                    persalys/GradientTableModel.hxx
                                    persalys/DataModelTableModel.hxx
                                    persalys/DataCleaningTableModel.hxx
                                    persalys/DistributionsTableModel.hxx
                                    persalys/CalibrationTableModel.hxx
                                    persalys/ItemFactory.hxx
                                    persalys/Item.hxx
                                    persalys/MeshItem.hxx
                                    persalys/DataMeshItem.hxx
                                    persalys/PhysicalModelDiagramItem.hxx
                                    persalys/PhysicalModelDefinitionItem.hxx
                                    persalys/AnalysisItem.hxx
                                    persalys/StudyTreeViewModel.hxx
                                    persalys/LimitStateItem.hxx
                                    persalys/DesignOfExperimentItem.hxx
                                    persalys/DesignOfExperimentDefinitionItem.hxx
                                    persalys/DataModelDiagramItem.hxx
                                    persalys/DataFieldModelDiagramItem.hxx
                                    persalys/DataModelDefinitionItem.hxx
                                    persalys/DataFieldModelDefinitionItem.hxx
                                    persalys/ProbabilisticModelItem.hxx
                                    persalys/PhysicalModelItem.hxx
                                    persalys/DataFieldModelItem.hxx
                                    persalys/StudyItem.hxx
                                    persalys/ObservationsItem.hxx
                                    persalys/CodeModel.hxx
                                    persalys/VariablesSelectionTableModel.hxx
                                    persalys/CustomStandardItemModel.hxx
                                    persalys/DependenciesTableModel.hxx
                                    persalys/OptimizationTableModel.hxx
                                    persalys/MultiObjectiveOptimizationTableModel.hxx
                                    persalys/ConstraintsTableModel.hxx
                                    persalys/ObjectivesTableModel.hxx
                                    persalys/TranslationManager.hxx
                                    persalys/AnsysSystemTableModel.hxx
                                    persalys/AnsysVariableTableModel.hxx
                                    persalys/FileTools.hxx)

if (OTMORRIS_FOUND)
  qt_wrap_cpp (PERSALYS_MODEL_MOC_SRCS persalys/MorrisTableModel.hxx)
  qt_wrap_cpp (PERSALYS_MODEL_MOC_SRCS persalys/MorrisResultTableModel.hxx)
endif ()

if (SalomeYACS_FOUND)
  set_source_files_properties (StudyItem.cxx PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON) # FIXME: Python/Qt conflict due to YACS
endif ()

include_directories ( ${INTERNAL_INCLUDE_DIRS} )

add_library (persalysmodel ${SOURCEFILES} ${PERSALYS_MODEL_MOC_SRCS})

if (BUILD_SHARED_LIBS)
  target_compile_definitions (persalysmodel PRIVATE PERSALYS_MODEL_DLL_EXPORTS)
else ()
  target_compile_definitions (persalysmodel PUBLIC PERSALYS_MODEL_STATIC)
endif ()

if ( NOT DEFINED LIB_VERSION )
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION )
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( persalysmodel PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( persalysmodel PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries ( persalysmodel persalysbase ${QT_LIBRARIES})

install ( TARGETS persalysmodel
  RUNTIME DESTINATION ${PERSALYS_BIN_PATH}
  LIBRARY DESTINATION ${PERSALYS_LIBRARY_PATH}
  ARCHIVE DESTINATION ${PERSALYS_LIBRARY_PATH}
)
