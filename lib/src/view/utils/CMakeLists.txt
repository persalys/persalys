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
ot_add_source_file (EditableExportableTableView.cxx)
ot_add_source_file (CopyableTableView.cxx)
ot_add_source_file (QtTools.cxx)
ot_add_source_file (DoubleSpinBox.cxx)
ot_add_source_file (UIntSpinBox.cxx)
ot_add_source_file (LogSpinBox.cxx)
ot_add_source_file (LogDoubleSpinBox.cxx)
ot_add_source_file (CheckableHeaderView.cxx)
ot_add_source_file (RadioButtonDelegate.cxx)
ot_add_source_file (ResizableStackedWidget.cxx)
ot_add_source_file (TitledComboBox.cxx)
ot_add_source_file (Arrow.cxx)
ot_add_source_file (DiagramPushButton.cxx)
ot_add_source_file (ResizableHeaderlessTableView.cxx)
ot_add_source_file (TemporaryLabel.cxx)
ot_add_source_file (DocumentationToolButton.cxx)
ot_add_source_file (EditButtonDelegate.cxx)
ot_add_source_file (EditValuesWizard.cxx)
ot_add_source_file (ParametersTableView.cxx)

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
ot_install_header_file ( EditableExportableTableView.hxx )
ot_install_header_file ( CopyableTableView.hxx )
ot_install_header_file ( QtTools.hxx )
ot_install_header_file ( DoubleSpinBox.hxx )
ot_install_header_file ( UIntSpinBox.hxx )
ot_install_header_file ( LogSpinBox.hxx )
ot_install_header_file ( LogDoubleSpinBox.hxx )
ot_install_header_file ( CheckableHeaderView.hxx )
ot_install_header_file ( ResizableStackedWidget.hxx )
ot_install_header_file ( RadioButtonDelegate.hxx )
ot_install_header_file ( ParametersTableView.hxx )
ot_install_header_file ( Arrow.hxx )
ot_install_header_file ( DiagramPushButton.hxx )
ot_install_header_file ( ResizableHeaderlessTableView.hxx )
ot_install_header_file ( TemporaryLabel.hxx )
ot_install_header_file ( DocumentationToolButton.hxx )
ot_install_header_file ( EditButtonDelegate.hxx )
ot_install_header_file ( EditValuesWizard.hxx )

qt5_wrap_cpp (PERSALYS_UTILS_MOC_SRCS
              persalys/ListWidgetWithCheckBox.hxx
              persalys/ValueLineEdit.hxx
              persalys/TimeLineEdit.hxx
              persalys/CollapsibleGroupBox.hxx
              persalys/SpinBoxDelegate.hxx
              persalys/CodeDelegate.hxx
              persalys/ComboBoxDelegate.hxx
              persalys/LineEditWithQValidatorDelegate.hxx
              persalys/HorizontalHeaderViewWithCombobox.hxx
              persalys/ExportableTableView.hxx
              persalys/EditableExportableTableView.hxx
              persalys/CopyableTableView.hxx
              persalys/DoubleSpinBox.hxx
              persalys/UIntSpinBox.hxx
              persalys/LogSpinBox.hxx
              persalys/LogDoubleSpinBox.hxx
              persalys/CheckableHeaderView.hxx
              persalys/ResizableStackedWidget.hxx
              persalys/TitledComboBox.hxx
              persalys/Arrow.hxx
              persalys/DiagramPushButton.hxx
              persalys/TemporaryLabel.hxx
              persalys/DocumentationToolButton.hxx
              persalys/EditButtonDelegate.hxx
              persalys/EditValuesWizard.hxx
             )

include_directories (${INTERNAL_INCLUDE_DIRS})
persalys_add_library (persalysutils ${SOURCEFILES} ${PERSALYS_UTILS_MOC_SRCS})

if (BUILD_SHARED_LIBS)
  target_compile_definitions (persalysutils PRIVATE PERSALYS_UTILS_DLL_EXPORTS)
endif ()

if ( NOT DEFINED LIB_VERSION )
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION )
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( persalysutils PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( persalysutils PROPERTIES SOVERSION ${LIB_SOVERSION} )

target_link_libraries (persalysutils persalysbase persalysmodel)
target_link_libraries (persalysutils ${QT_LIBRARIES})

install ( TARGETS persalysutils
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${PERSALYS_LIBRARY_PATH}
  ARCHIVE DESTINATION ${PERSALYS_LIBRARY_PATH}
)
