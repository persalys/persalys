
ot_add_current_dir_to_include_dirs ()
include_directories (${INTERNAL_INCLUDE_DIRS})

set(BASE_SOURCEFILES DistributionDictionary.cxx
                     BaseTools.cxx
                     AppliException.cxx
                     Observer.cxx Observable.cxx Study.cxx StudyImplementation.cxx
                     Variable.cxx Input.cxx Output.cxx
                     DataSample.cxx
                     DataImport.cxx
                     DataModel.cxx
                     MeshModel.cxx MeshModelImplementation.cxx
                     GridMeshModel.cxx
                     ImportedMeshModel.cxx
                     PhysicalModel.cxx PhysicalModelImplementation.cxx SymbolicPhysicalModel.cxx
                     PythonPhysicalModel.cxx
                     MetaModel.cxx
                     SymbolicFieldModel.cxx
                     PythonScriptEvaluation.cxx
                     PythonScriptFieldFunction.cxx
                     PythonFieldModel.cxx
                     DesignOfExperimentImplementation.cxx DesignOfExperiment.cxx
                     FixedDesignOfExperiment.cxx
                     GridDesignOfExperiment.cxx
                     ImportedDesignOfExperiment.cxx
                     ProbabilisticDesignOfExperiment.cxx
                     AnalysisResult.cxx
                     EvaluationResult.cxx
                     AnalysisImplementation.cxx Analysis.cxx SimulationAnalysis.cxx
                     PhysicalModelAnalysis.cxx DesignOfExperimentAnalysis.cxx
                     WithStopCriteriaAnalysis.cxx
                     DataAnalysisResult.cxx DataAnalysis.cxx
                     ModelEvaluation.cxx
                     FieldModelEvaluation.cxx
                     DesignOfExperimentEvaluation.cxx
                     MonteCarloAnalysis.cxx TaylorExpansionMomentsResult.cxx
                     FieldMonteCarloAnalysis.cxx FieldMonteCarloResult.cxx
                     TaylorExpansionMomentsAnalysis.cxx SobolResult.cxx SobolAnalysis.cxx
                     SRCResult.cxx SRCAnalysis.cxx
                     LimitStateImplementation.cxx LimitState.cxx
                     ReliabilityAnalysis.cxx
                     SimulationInterface.cxx
                     SimulationReliabilityAnalysis.cxx
                     SimulationReliabilityResult.cxx
                     MonteCarloReliabilityAnalysis.cxx
                     ImportanceSamplingAnalysis.cxx
                     ApproximationAnalysis.cxx
                     FORMAnalysisResult.cxx FORMAnalysis.cxx
                     SORMAnalysisResult.cxx SORMAnalysis.cxx
                     FORMImportanceSamplingAnalysis.cxx
                     MetaModelAnalysis.cxx
                     MetaModelValidationResult.cxx
                     MetaModelAnalysisResult.cxx
                     FunctionalChaosAnalysisResult.cxx
                     FunctionalChaosAnalysis.cxx
                     KrigingAnalysisResult.cxx
                     KrigingAnalysis.cxx
                     PolynomialRegressionAnalysisResult.cxx
                     PolynomialRegressionAnalysis.cxx
                     FittingTestResult.cxx
                     InferenceResult.cxx
                     InferenceAnalysis.cxx
                     CopulaInferenceSetResult.cxx
                     CopulaInferenceResult.cxx
                     CopulaInferenceAnalysis.cxx
                     OptimizationAnalysis.cxx
                     MultiObjectiveOptimizationAnalysis.cxx
                     PythonEnvironment.cxx
                     Observations.cxx
                     CalibrationAnalysis.cxx
                     CalibrationAnalysisResult.cxx
                     CouplingInputFile.cxx
                     CouplingResourceFile.cxx
                     CouplingOutputFile.cxx
                     CouplingStep.cxx
                     CouplingPhysicalModel.cxx
                     FileMemoizeEvaluation.cxx
                     FileMemoizeFunction.cxx
                     DataCleaning.cxx
                     MorrisAnalysis.cxx
                     MorrisResult.cxx
                     FMIPhysicalModel.cxx
                     FMUInfo.cxx
                     AnsysParser.cxx
                     )
if (SalomeYACS_FOUND)
  list (APPEND BASE_SOURCEFILES YACSPhysicalModel.cxx YACSEvaluation.cxx YACSCouplingPhysicalModel.cxx)
endif ()
if (AdaoCppLayer_FOUND)
  list (APPEND BASE_SOURCEFILES AdaoCalibration.cxx)
endif ()

ot_install_header_file ( DistributionDictionary.hxx )
ot_install_header_file ( BaseTools.hxx )
ot_install_header_file ( AppliException.hxx )
ot_install_header_file ( Variable.hxx )
ot_install_header_file ( Input.hxx )
ot_install_header_file ( Output.hxx )
ot_install_header_file ( DataSample.hxx )
ot_install_header_file ( DataImport.hxx )
ot_install_header_file ( MeshModel.hxx )
ot_install_header_file ( MeshModelImplementation.hxx )
ot_install_header_file ( GridMeshModel.hxx )
ot_install_header_file ( ImportedMeshModel.hxx )
ot_install_header_file ( PhysicalModel.hxx )
ot_install_header_file ( PhysicalModelImplementation.hxx )
ot_install_header_file ( SymbolicPhysicalModel.hxx )
ot_install_header_file ( PythonPhysicalModel.hxx )
ot_install_header_file ( SymbolicFieldModel.hxx )
ot_install_header_file ( PythonScriptEvaluation.hxx )
ot_install_header_file ( PythonScriptFieldFunction.hxx )
ot_install_header_file ( PythonFieldModel.hxx )
ot_install_header_file ( MetaModel.hxx )
ot_install_header_file ( DesignOfExperimentImplementation.hxx )
ot_install_header_file ( DesignOfExperiment.hxx )
ot_install_header_file ( FixedDesignOfExperiment.hxx )
ot_install_header_file ( GridDesignOfExperiment.hxx )
ot_install_header_file ( ImportedDesignOfExperiment.hxx )
ot_install_header_file ( ProbabilisticDesignOfExperiment.hxx )
ot_install_header_file ( DataModel.hxx )
ot_install_header_file ( AnalysisResult.hxx )
ot_install_header_file ( EvaluationResult.hxx )
ot_install_header_file ( AnalysisImplementation.hxx )
ot_install_header_file ( PhysicalModelAnalysis.hxx )
ot_install_header_file ( DesignOfExperimentAnalysis.hxx )
ot_install_header_file ( Analysis.hxx )
ot_install_header_file ( SimulationAnalysis.hxx )
ot_install_header_file ( WithStopCriteriaAnalysis.hxx )
ot_install_header_file ( DataAnalysisResult.hxx )
ot_install_header_file ( DataAnalysis.hxx )
ot_install_header_file ( DesignOfExperimentEvaluation.hxx )
ot_install_header_file ( ModelEvaluation.hxx )
ot_install_header_file ( FieldModelEvaluation.hxx )
ot_install_header_file ( MonteCarloAnalysis.hxx )
ot_install_header_file ( FieldMonteCarloResult.hxx )
ot_install_header_file ( FieldMonteCarloAnalysis.hxx )
ot_install_header_file ( TaylorExpansionMomentsAnalysis.hxx )
ot_install_header_file ( TaylorExpansionMomentsResult.hxx )
ot_install_header_file ( SobolAnalysis.hxx )
ot_install_header_file ( SobolResult.hxx )
ot_install_header_file ( SRCAnalysis.hxx )
ot_install_header_file ( SRCResult.hxx )
ot_install_header_file ( Study.hxx )
ot_install_header_file ( StudyImplementation.hxx )
ot_install_header_file ( Observable.hxx )
ot_install_header_file ( Observer.hxx )
ot_install_header_file ( LimitStateImplementation.hxx )
ot_install_header_file ( LimitState.hxx )
ot_install_header_file ( ReliabilityAnalysis.hxx )
ot_install_header_file ( SimulationInterface.hxx )
ot_install_header_file ( SimulationReliabilityAnalysis.hxx )
ot_install_header_file ( SimulationReliabilityResult.hxx )
ot_install_header_file ( MonteCarloReliabilityAnalysis.hxx )
ot_install_header_file ( ImportanceSamplingAnalysis.hxx )
ot_install_header_file ( ApproximationAnalysis.hxx )
ot_install_header_file ( FORMAnalysis.hxx )
ot_install_header_file ( FORMAnalysisResult.hxx )
ot_install_header_file ( SORMAnalysis.hxx )
ot_install_header_file ( SORMAnalysisResult.hxx )
ot_install_header_file ( FORMImportanceSamplingAnalysis.hxx )
ot_install_header_file ( MetaModelValidationResult.hxx )
ot_install_header_file ( MetaModelAnalysisResult.hxx )
ot_install_header_file ( MetaModelAnalysis.hxx )
ot_install_header_file ( FunctionalChaosAnalysisResult.hxx )
ot_install_header_file ( FunctionalChaosAnalysis.hxx )
ot_install_header_file ( KrigingAnalysisResult.hxx )
ot_install_header_file ( KrigingAnalysis.hxx )
ot_install_header_file ( PolynomialRegressionAnalysisResult.hxx )
ot_install_header_file ( PolynomialRegressionAnalysis.hxx )
ot_install_header_file ( FittingTestResult.hxx )
ot_install_header_file ( InferenceResult.hxx )
ot_install_header_file ( InferenceAnalysis.hxx )
ot_install_header_file ( CopulaInferenceSetResult.hxx )
ot_install_header_file ( CopulaInferenceResult.hxx )
ot_install_header_file ( CopulaInferenceAnalysis.hxx )
ot_install_header_file ( OptimizationAnalysis.hxx )
ot_install_header_file ( MultiObjectiveOptimizationAnalysis.hxx )
ot_install_header_file ( PythonEnvironment.hxx )
ot_install_header_file ( LaunchParametersVisitor.hxx )
ot_install_header_file ( Observations.hxx )
ot_install_header_file ( CalibrationAnalysis.hxx )
ot_install_header_file ( CalibrationAnalysisResult.hxx )
ot_install_header_file ( CouplingInputFile.hxx )
ot_install_header_file ( CouplingResourceFile.hxx )
ot_install_header_file ( CouplingOutputFile.hxx )
ot_install_header_file ( CouplingStep.hxx )
ot_install_header_file ( DataCleaning.hxx )
ot_install_header_file (FMIPhysicalModel.hxx)
ot_install_header_file (FMUInfo.hxx)
ot_install_header_file (AnsysParser.hxx)
ot_install_header_file (MorrisAnalysis.hxx)
ot_install_header_file (MorrisResult.hxx)
ot_install_header_file (YACSPhysicalModel.hxx)
ot_install_header_file (YACSEvaluation.hxx)


add_library (persalysbase ${BASE_SOURCEFILES})
target_compile_features (persalysbase PRIVATE cxx_lambdas)

if (BUILD_SHARED_LIBS)
  target_compile_definitions (persalysbase PRIVATE PERSALYS_BASE_DLL_EXPORTS)
else ()
  target_compile_definitions (persalysbase PUBLIC PERSALYS_BASE_STATIC)
endif ()

target_include_directories (persalysbase PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories (persalysbase PRIVATE ${Python_INCLUDE_DIRS})

if (OTMORRIS_FOUND)
  target_include_directories (persalysbase PRIVATE ${OTMORRIS_INCLUDE_DIRS})
endif ()

if (NOT DEFINED LIB_VERSION)
  set (LIB_VERSION 0.0.0)
endif ()
if (NOT DEFINED LIB_SOVERSION)
  set (LIB_SOVERSION 0)
endif ()
set_target_properties (persalysbase PROPERTIES VERSION ${LIB_VERSION})
set_target_properties (persalysbase PROPERTIES SOVERSION ${LIB_SOVERSION})
set_target_properties (persalysbase PROPERTIES CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1)
  target_link_libraries (persalysbase PRIVATE stdc++fs)
endif ()

target_link_libraries (persalysbase PUBLIC ${PERSALYS_PUBLIC_LIBRARIES})
target_link_libraries (persalysbase PRIVATE ${PERSALYS_PRIVATE_LIBRARIES})

install (TARGETS persalysbase
  RUNTIME DESTINATION ${PERSALYS_BIN_PATH}
  LIBRARY DESTINATION ${PERSALYS_LIBRARY_PATH}
  ARCHIVE DESTINATION ${PERSALYS_LIBRARY_PATH}
)
