
include_directories (${INTERNAL_INCLUDE_DIRS})

ot_add_current_dir_to_include_dirs ()

add_subdirectory ( otgui )

set(BASE_SOURCEFILES Observer.cxx Observable.cxx OTStudy.cxx Variable.cxx Input.cxx Output.cxx PhysicalModelImplementation.cxx PhysicalModel.cxx CalculusImplementation.cxx Calculus.cxx ParametricCalculusResult.cxx ParametricCalculus.cxx MonteCarloResult.cxx MonteCarloCalculus.cxx QuadraticCumulResult.cxx QuadraticCumulCalculus.cxx SobolCalculusResult.cxx SobolCalculus.cxx)
if (SalomeYACS_FOUND)
  list (APPEND BASE_SOURCEFILES YACSPhysicalModel.cxx YACSEvaluation.cxx)
endif ()
add_library (otguibase ${BASE_SOURCEFILES})

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

