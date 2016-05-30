set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( DataTableModel.cxx )
ot_add_source_file ( InputTableModel.cxx )
ot_add_source_file ( InputTableProbabilisticModel.cxx )
ot_add_source_file ( OutputTableModel.cxx )
ot_add_source_file ( ExperimentTableModel.cxx )
ot_add_source_file ( CorrelationTableModel.cxx )
ot_add_source_file ( LimitStateItem.cxx )
ot_add_source_file ( DesignOfExperimentItem.cxx )
ot_add_source_file ( ProbabilisticModelItem.cxx )
ot_add_source_file ( PhysicalModelItem.cxx )
ot_add_source_file ( OTStudyItem.cxx )
ot_add_source_file ( AnalysisItem.cxx )
ot_add_source_file ( StudyTreeViewModel.cxx )
ot_add_source_file ( CodeModel.cxx)
ot_add_source_file ( CustomStandardItemModel.cxx )

ot_install_header_file ( DataTableModel.hxx )
ot_install_header_file ( InputTableModel.hxx )
ot_install_header_file ( InputTableProbabilisticModel.hxx )
ot_install_header_file ( OutputTableModel.hxx )
ot_install_header_file ( ExperimentTableModel.hxx )
ot_install_header_file ( CorrelationTableModel.hxx )
ot_install_header_file ( AnalysisItem.hxx )
ot_install_header_file ( LimitStateItem.hxx )
ot_install_header_file ( DesignOfExperimentItem.hxx )
ot_install_header_file ( ProbabilisticModelItem.hxx )
ot_install_header_file ( PhysicalModelItem.hxx )
ot_install_header_file ( OTStudyItem.hxx )
ot_install_header_file ( StudyTreeViewModel.hxx )
ot_install_header_file ( CustomStandardItemModel.hxx )

qt5_wrap_cpp ( OTGUI_MODEL_MOC_SRCS otgui/InputTableModel.hxx
                                    otgui/InputTableProbabilisticModel.hxx
                                    otgui/OutputTableModel.hxx
                                    otgui/ExperimentTableModel.hxx
                                    otgui/CorrelationTableModel.hxx
                                    otgui/AnalysisItem.hxx
                                    otgui/StudyTreeViewModel.hxx
                                    otgui/LimitStateItem.hxx
                                    otgui/DesignOfExperimentItem.hxx
                                    otgui/ProbabilisticModelItem.hxx
                                    otgui/PhysicalModelItem.hxx
                                    otgui/OTStudyItem.hxx
                                    otgui/CodeModel.hxx)

include_directories ( ${INTERNAL_INCLUDE_DIRS} )

add_library ( otguimodel ${SOURCEFILES} ${OTGUI_MODEL_MOC_SRCS} )
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

