

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( ComboBoxDelegate.cxx )
ot_add_source_file ( OTguiTableModel.cxx )
ot_add_source_file ( InputTableModel.cxx )
ot_add_source_file ( OutputTableModel.cxx )
ot_add_source_file ( Item.cxx )
ot_add_source_file ( PhysicalModelItem.cxx )
ot_add_source_file ( StudyItem.cxx )
ot_add_source_file ( CalculusItem.cxx )
ot_add_source_file ( ParametricCalculusItem.cxx )
ot_add_source_file ( ParametricCalculusTableModel.cxx )
ot_add_source_file ( StudyTreeViewModel.cxx )


qt4_wrap_cpp (OTGUI_MODEL_MOC_SRCS ComboBoxDelegate.hxx InputTableModel.hxx OutputTableModel.hxx CalculusItem.hxx ParametricCalculusItem.hxx ParametricCalculusTableModel.hxx StudyTreeViewModel.hxx PhysicalModelItem.hxx Item.hxx StudyItem.hxx)

ot_install_header_file ( CalculusItem.hxx )
ot_install_header_file ( ComboBoxDelegate.hxx )
ot_install_header_file ( OTguiTableModel.hxx )
ot_install_header_file ( InputTableModel.hxx )
ot_install_header_file ( OutputTableModel.hxx )
ot_install_header_file ( Item.hxx )
ot_install_header_file ( CalculusItem.hxx )
ot_install_header_file ( ParametricCalculusItem.hxx )
ot_install_header_file ( PhysicalModelItem.hxx )
ot_install_header_file ( StudyItem.hxx )
ot_install_header_file ( ParametricCalculusTableModel.hxx )
ot_install_header_file ( StudyTreeViewModel.hxx )

include_directories ( ${INTERNAL_INCLUDE_DIRS} )

add_library ( otguimodel ${SOURCEFILES} ${OTGUI_MODEL_MOC_SRCS})
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

