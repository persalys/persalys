set (SOURCEFILES "" CACHE STRING "" FORCE)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file (PVViewWidget.cxx)
ot_add_source_file (PVParCooViewWidget.cxx)
ot_add_source_file (PVSpreadSheetViewWidget.cxx)
ot_add_source_file (PVMatrixPlotViewWidget.cxx)
ot_add_source_file (PVXYChartSettingWidget.cxx)
ot_add_source_file (PVXYChartViewWidget.cxx)
ot_add_source_file (PVPlotSettingWidget.cxx)
ot_add_source_file (PVServerManagerSingleton.cxx)
ot_add_source_file (PVBagChartViewWidget.cxx)

ot_install_header_file (PVMatrixPlotViewWidget.hxx)
ot_install_header_file (PVParCooViewWidget.hxx)
ot_install_header_file (PVSpreadSheetViewWidget.hxx)
ot_install_header_file (PVViewWidget.hxx)
ot_install_header_file (PVServerManagerInterface.hxx)
ot_install_header_file (PVServerManagerSingleton.hxx)
ot_install_header_file (PVXYChartSettingWidget.hxx)
ot_install_header_file (PVXYChartViewWidget.hxx)
ot_install_header_file (PVPlotSettingWidget.hxx)
ot_install_header_file (PVBagChartViewWidget.hxx)

qt5_wrap_cpp (PERSALYS_PLOTPARAVIEW_MOC_SRCS
              persalys/PVMatrixPlotViewWidget.hxx
              persalys/PVParCooViewWidget.hxx
              persalys/PVSpreadSheetViewWidget.hxx
              persalys/PVViewWidget.hxx
              persalys/PVXYChartSettingWidget.hxx
              persalys/PVXYChartViewWidget.hxx
              persalys/PVPlotSettingWidget.hxx
              persalys/PVBagChartViewWidget.hxx
              )

include_directories (${INTERNAL_INCLUDE_DIRS})
add_library (persalysplotparaview ${SOURCEFILES} ${PERSALYS_PLOTPARAVIEW_MOC_SRCS})

ADD_PARAVIEW_PLUGIN(XYChartRepresentationColumns "1.0"
  SERVER_MANAGER_XML XYChartRepresentationColumns.xml
  SERVER_MANAGER_SOURCES
  vtkXYChartRepresentationColumns.cxx)

if ( BUILD_SHARED_LIBS )
  set_target_properties ( persalysplotparaview PROPERTIES COMPILE_DEFINITIONS "PERSALYS_DLL_EXPORTS" )
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( persalysplotparaview PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( persalysplotparaview PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries ( persalysplotparaview persalysutils )

if (TARGET ParaView::pqApplicationComponents)
  target_link_libraries (persalysplotparaview ParaView::pqApplicationComponents ParaView::vtkPVServerManagerRenderingCS ParaView::vtkRenderingFreeTypeCS)
  target_link_libraries (persalysplotparaview ParaView::vtkRenderingContext2DCS) # for vtkContextScene::SetDirty(bool)
else ()
  target_link_libraries (persalysplotparaview pqApplicationComponents vtkPVServerManagerRendering vtkRenderingFreeType)
  target_link_libraries (persalysplotparaview vtkRenderingContext2D) # for vtkContextScene::SetDirty(bool)
endif ()

target_link_libraries (persalysplotparaview XYChartRepresentationColumns)

install ( TARGETS persalysplotparaview XYChartRepresentationColumns
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${PERSALYS_LIBRARY_PATH}
  ARCHIVE DESTINATION ${PERSALYS_LIBRARY_PATH}
)

