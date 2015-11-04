
add_subdirectory (pythonConsole)
add_subdirectory (plot)

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( MainWindow.cxx )
ot_add_source_file ( OTguiSubWindow.cxx )
ot_add_source_file ( PhysicalModelWindow.cxx )
ot_add_source_file ( StudyTreeView.cxx )
ot_add_source_file ( OTguiTableView.cxx )
ot_add_source_file ( ParametricCalculusWizard.cxx )
ot_add_source_file ( ParametricCalculusResultWindow.cxx )
ot_add_source_file ( DistributionAnalysisWizard.cxx )
ot_add_source_file ( MonteCarloCalculusResultWindow.cxx )
ot_add_source_file ( QuadraticCumulCalculusResultWindow.cxx )
ot_add_source_file ( SobolCalculusResultWindow.cxx )
ot_add_source_file ( SensitivityAnalysisWizard.cxx )

add_subdirectory (otgui)

qt4_wrap_cpp ( OTGUI_VIEW_MOC_SRCS otgui/MainWindow.hxx
                                   otgui/OTguiSubWindow.hxx
                                   otgui/StudyTreeView.hxx
                                   otgui/OTguiTableView.hxx
                                   otgui/PhysicalModelWindow.hxx
                                   otgui/ParametricCalculusWizard.hxx
                                   otgui/ParametricCalculusResultWindow.hxx
                                   otgui/DistributionAnalysisWizard.hxx
                                   otgui/MonteCarloCalculusResultWindow.hxx
                                   otgui/QuadraticCumulCalculusResultWindow.hxx
                                   otgui/SobolCalculusResultWindow.hxx
                                   otgui/SensitivityAnalysisWizard.hxx )

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

