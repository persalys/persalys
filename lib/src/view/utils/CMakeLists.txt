set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file (ListWidgetWithCheckBox.cxx)
ot_add_source_file (ValueLineEdit.cxx)
ot_add_source_file (CollapsibleGroupBox.cxx)
ot_add_source_file (SpinBoxDelegate.cxx)
ot_add_source_file (CodeDelegate.cxx)
ot_add_source_file (ComboBoxDelegate.cxx)
ot_add_source_file (CustomDoubleValidator.cxx)
ot_add_source_file (LineEditWithQValidatorDelegate.cxx)
ot_add_source_file (DataTableWidget.cxx)
ot_add_source_file (NotEditableTableWidget.cxx)
ot_add_source_file (HorizontalHeaderViewWithCombobox.cxx)
ot_add_source_file (QtTools.cxx)

qt5_wrap_cpp (OTGUI_UTILS_MOC_SRCS
              otgui/ListWidgetWithCheckBox.hxx
              otgui/ValueLineEdit.hxx
              otgui/CollapsibleGroupBox.hxx
              otgui/SpinBoxDelegate.hxx
              otgui/CodeDelegate.hxx
              otgui/ComboBoxDelegate.hxx
              otgui/LineEditWithQValidatorDelegate.hxx
              otgui/DataTableWidget.hxx
              otgui/NotEditableTableWidget.hxx
              otgui/HorizontalHeaderViewWithCombobox.hxx
              otgui/CustomDoubleValidator.hxx
              otgui/QtTools.hxx              
             )

include_directories (${INTERNAL_INCLUDE_DIRS})
add_library (otguiutils ${SOURCEFILES} ${OTGUI_UTILS_MOC_SRCS})

if ( BUILD_SHARED_LIBS )
  set_target_properties ( otguiutils PROPERTIES COMPILE_DEFINITIONS "OTGUI_DLL_EXPORTS" )
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION )
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( otguiutils PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( otguiutils PROPERTIES SOVERSION ${LIB_SOVERSION} )

target_link_libraries (otguiutils ${QT_LIBRARIES})

install ( TARGETS otguiutils
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)

