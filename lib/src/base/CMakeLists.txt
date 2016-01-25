
include_directories (${INTERNAL_INCLUDE_DIRS})
include_directories (${PYTHON_INCLUDE_DIRS})

ot_add_current_dir_to_include_dirs ()

add_subdirectory ( otgui )

set(BASE_SOURCEFILES Observer.cxx Observable.cxx OTStudy.cxx Variable.cxx Input.cxx Output.cxx
                     PhysicalModel.cxx PhysicalModelImplementation.cxx AnalyticalPhysicalModel.cxx 
                     PythonPhysicalModel.cxx
                     PythonEvaluation.cxx
                     DesignOfExperimentImplementation.cxx DesignOfExperiment.cxx
                     AnalysisImplementation.cxx Analysis.cxx SimulationAnalysis.cxx
                     ModelEvaluationResult.cxx ModelEvaluation.cxx
                     SimulationAnalysisResult.cxx
                     MonteCarloResult.cxx MonteCarloAnalysis.cxx QuadraticCumulResult.cxx
                     QuadraticCumulAnalysis.cxx SobolResult.cxx SobolAnalysis.cxx
                     SRCResult.cxx SRCAnalysis.cxx
                     LimitStateImplementation.cxx LimitState.cxx
                     ReliabilityAnalysis.cxx
                     MonteCarloReliabilityAnalysis.cxx)
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

target_link_libraries (otguibase ${PYTHON_LIBRARIES})
  
install ( TARGETS otguibase
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${OTGUI_LIBRARY_PATH}
  ARCHIVE DESTINATION ${OTGUI_LIBRARY_PATH}
)

