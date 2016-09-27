set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file (ListWidgetWithCheckBox.cxx)
ot_add_source_file (TimeLineEdit.cxx)
ot_add_source_file (ValueLineEdit.cxx)
ot_add_source_file (CollapsibleGroupBox.cxx)
ot_add_source_file (SpinBoxDelegate.cxx)
ot_add_source_file (CodeDelegate.cxx)
ot_add_source_file (ComboBoxDelegate.cxx)
ot_add_source_file (LineEditWithQValidatorDelegate.cxx)
ot_add_source_file (HorizontalHeaderViewWithCombobox.cxx)
ot_add_source_file (ExportableTableView.cxx)
ot_add_source_file (CopyableTableView.cxx)
ot_add_source_file (QtTools.cxx)
ot_add_source_file (ResizableTableViewWithoutScrollBar.cxx)
ot_add_source_file (DoubleSpinBox.cxx)
ot_add_source_file (UIntSpinBox.cxx)
ot_add_source_file (CheckableHeaderView.cxx)
ot_add_source_file (ResizableStackedWidget.cxx)
ot_add_source_file (TitledComboBox.cxx)

ot_install_header_file ( ListWidgetWithCheckBox.hxx )
ot_install_header_file ( TimeLineEdit.hxx )
ot_install_header_file ( ValueLineEdit.hxx )
ot_install_header_file ( CollapsibleGroupBox.hxx )
ot_install_header_file ( SpinBoxDelegate.hxx )
ot_install_header_file ( CodeDelegate.hxx )
ot_install_header_file ( ComboBoxDelegate.hxx )
ot_install_header_file ( LineEditWithQValidatorDelegate.hxx )
ot_install_header_file ( HorizontalHeaderViewWithCombobox.hxx )
ot_install_header_file ( ExportableTableView.hxx )
ot_install_header_file ( CopyableTableView.hxx )
ot_install_header_file ( QtTools.hxx )
ot_install_header_file ( ResizableTableViewWithoutScrollBar.hxx )
ot_install_header_file ( DoubleSpinBox.hxx )
ot_install_header_file ( UIntSpinBox.hxx )
ot_install_header_file ( CheckableHeaderView.hxx )
ot_install_header_file ( ResizableStackedWidget.hxx )

qt5_wrap_cpp (OTGUI_UTILS_MOC_SRCS
              otgui/ListWidgetWithCheckBox.hxx
              otgui/ValueLineEdit.hxx
              otgui/TimeLineEdit.hxx
              otgui/CollapsibleGroupBox.hxx
              otgui/SpinBoxDelegate.hxx
              otgui/CodeDelegate.hxx
              otgui/ComboBoxDelegate.hxx
              otgui/LineEditWithQValidatorDelegate.hxx
              otgui/HorizontalHeaderViewWithCombobox.hxx
              otgui/ExportableTableView.hxx
              otgui/CopyableTableView.hxx
              otgui/ResizableTableViewWithoutScrollBar.hxx
              otgui/DoubleSpinBox.hxx
              otgui/UIntSpinBox.hxx
              otgui/CheckableHeaderView.hxx
              otgui/ResizableStackedWidget.hxx
              otgui/TitledComboBox.hxx
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

target_link_libraries (otguiutils otguibase)
target_link_libraries (otguiutils ${QT_LIBRARIES})

install ( TARGETS otguiutils
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)

