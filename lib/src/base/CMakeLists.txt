
include_directories (${INTERNAL_INCLUDE_DIRS})
include_directories (${PYTHON_INCLUDE_DIRS})

ot_add_current_dir_to_include_dirs ()

set(BASE_SOURCEFILES DistributionDictionary.cxx
                     OTguiException.cxx
                     Observer.cxx Observable.cxx OTStudy.cxx Variable.cxx Input.cxx Output.cxx
                     PhysicalModel.cxx PhysicalModelImplementation.cxx AnalyticalPhysicalModel.cxx 
                     PythonPhysicalModel.cxx
                     PythonEvaluation.cxx
                     DesignOfExperimentImplementation.cxx DesignOfExperiment.cxx
                     AnalysisImplementation.cxx Analysis.cxx SimulationAnalysis.cxx
                     ModelEvaluationResult.cxx ModelEvaluation.cxx
                     SimulationAnalysisResult.cxx
                     MonteCarloResult.cxx MonteCarloAnalysis.cxx TaylorExpansionMomentsResult.cxx
                     TaylorExpansionMomentsAnalysis.cxx SobolResult.cxx SobolAnalysis.cxx
                     SRCResult.cxx SRCAnalysis.cxx
                     LimitStateImplementation.cxx LimitState.cxx
                     ReliabilityAnalysis.cxx
                     MonteCarloReliabilityAnalysis.cxx
                     MonteCarloReliabilityResult.cxx
                     PythonEnvironment.cxx)
if (SalomeYACS_FOUND)
  list (APPEND BASE_SOURCEFILES YACSPhysicalModel.cxx YACSEvaluation.cxx YACSEvalSessionSingleton.cxx)
endif ()

ot_install_header_file ( DistributionDictionary.hxx )
ot_install_header_file ( OTguiException.hxx )
ot_install_header_file ( Variable.hxx )
ot_install_header_file ( Input.hxx )
ot_install_header_file ( Output.hxx )
ot_install_header_file ( PhysicalModel.hxx )
ot_install_header_file ( PhysicalModelImplementation.hxx )
ot_install_header_file ( AnalyticalPhysicalModel.hxx )
ot_install_header_file ( PythonPhysicalModel.hxx )
ot_install_header_file ( DesignOfExperimentImplementation.hxx )
ot_install_header_file ( DesignOfExperiment.hxx )
ot_install_header_file ( AnalysisImplementation.hxx )
ot_install_header_file ( Analysis.hxx )
ot_install_header_file ( SimulationAnalysis.hxx )
ot_install_header_file ( SimulationAnalysisResult.hxx )
ot_install_header_file ( ModelEvaluation.hxx )
ot_install_header_file ( ModelEvaluationResult.hxx )
ot_install_header_file ( MonteCarloAnalysis.hxx )
ot_install_header_file ( MonteCarloResult.hxx )
ot_install_header_file ( TaylorExpansionMomentsAnalysis.hxx )
ot_install_header_file ( TaylorExpansionMomentsResult.hxx )
ot_install_header_file ( SobolAnalysis.hxx )
ot_install_header_file ( SobolResult.hxx )
ot_install_header_file ( SRCAnalysis.hxx )
ot_install_header_file ( SRCResult.hxx )
ot_install_header_file ( OTStudy.hxx )
ot_install_header_file ( Observable.hxx )
ot_install_header_file ( Observer.hxx )
ot_install_header_file ( LimitStateImplementation.hxx )
ot_install_header_file ( LimitState.hxx )
ot_install_header_file ( ReliabilityAnalysis.hxx )
ot_install_header_file ( MonteCarloReliabilityAnalysis.hxx )
ot_install_header_file ( MonteCarloReliabilityResult.hxx )
ot_install_header_file ( PythonEnvironment.hxx )

if (SalomeYACS_FOUND)
  ot_install_header_file (YACSPhysicalModel.hxx)
  ot_install_header_file (YACSEvaluation.hxx)
  ot_install_header_file (YACSEvalSessionSingleton.hxx)
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

