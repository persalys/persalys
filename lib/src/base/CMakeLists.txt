

ot_add_current_dir_to_include_dirs ()

ot_install_header_file ( Variable.hxx )
ot_install_header_file ( Input.hxx )
ot_install_header_file ( Output.hxx )
ot_install_header_file ( PhysicalModelImplementation.hxx )
ot_install_header_file ( PhysicalModel.hxx )
ot_install_header_file ( YACSPhysicalModel.hxx )
ot_install_header_file ( CalculusImplementation.hxx )
ot_install_header_file ( Calculus.hxx )
ot_install_header_file ( ParametricCalculus.hxx )
ot_install_header_file ( OTStudy.hxx )
ot_install_header_file ( Observable.hxx )
ot_install_header_file ( Observer.hxx )

include_directories (${INTERNAL_INCLUDE_DIRS})

#add_library ( otguibase ${SOURCEFILES} )
add_library (otguibase Observer.cxx Observable.cxx OTStudy.cxx Variable.cxx Input.cxx Output.cxx PhysicalModelImplementation.cxx PhysicalModel.cxx YACSPhysicalModel.cxx CalculusImplementation.cxx Calculus.cxx ParametricCalculus.cxx)

if ( BUILD_SHARED_LIBS )
  set_target_properties ( otguibase PROPERTIES COMPILE_DEFINITIONS "OTGUI_DLL_EXPORTS" )
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( otguibase PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( otguibase PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries ( otguibase ${OPENTURNS_LIBRARIES} )

if (SalomeYACS_FOUND)
  target_link_libraries ( otguibase YACSevalYFX )
endif ()


install ( TARGETS otguibase
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)

