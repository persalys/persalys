
include (${SWIG_USE_FILE})

include_directories (${OPENTURNS_INCLUDE_DIRS})
include_directories (${OPENTURNS_SWIG_INCLUDE_DIRS})
add_definitions (${OPENTURNS_SWIG_DEFINITIONS})
if (OTMORRIS_FOUND)
  include_directories (${OTMORRIS_INCLUDE_DIR}/otmorris/swig)
endif ()
include_directories (${INTERNAL_INCLUDE_DIRS})

set (CMAKE_SWIG_FLAGS "" CACHE STRING "SWIG flags for generating wrapper code")

# allows one to pass compile flags like -O1 to reduce memory usage
set (SWIG_COMPILE_FLAGS "" CACHE STRING "Additional flags used when compiling swig generated code")

macro ( ot_add_python_module MODULENAME SOURCEFILE )
  set_source_files_properties (${SOURCEFILE} PROPERTIES CPLUSPLUS ON)
  set_source_files_properties (${SOURCEFILE} PROPERTIES SWIG_MODULE_NAME ${MODULENAME})
  ot_install_swig_file ( ${SOURCEFILE} )
  if (${ARGC} GREATER 2)
    set (SWIG_MODULE_${MODULENAME}_python_EXTRA_DEPS ${ARGN})
  endif ()

  set (swig_other_sources)
  set (docstring_sources)
  foreach (it ${ARGN})
    if (${it} MATCHES ".*\\.i$")
      ot_install_swig_file (${it})
    else ()
      list (APPEND swig_other_sources "${it}")
    endif ()
  endforeach ()

  swig_add_library (${MODULENAME}_python LANGUAGE python SOURCES ${SOURCEFILE} ${swig_other_sources})

  # UseSWIG generates now standard target names
  set (module_target ${MODULENAME}_python)

  target_link_libraries (${module_target} persalysbase)
  target_link_libraries (${module_target} Python::Module)

  set_target_properties (${module_target} PROPERTIES OUTPUT_NAME ${MODULENAME})
  set_target_properties (${module_target} PROPERTIES COMPILE_FLAGS "${SWIG_COMPILE_FLAGS}")

  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULENAME}.py
           DESTINATION ${PERSALYS_PYTHON_MODULE_PATH}/${MODULENAME})
  install (TARGETS ${module_target}
           LIBRARY DESTINATION ${PERSALYS_PYTHON_MODULE_PATH}/${MODULENAME})

  list (APPEND PERSALYS_PYTHON_MODULES ${MODULENAME})
endmacro ()

ot_add_python_module (persalys persalys_module.i
                      Observer.i
                      Observable.i
                      Variable.i Variable_doc.i
                      Input.i Input_doc.i
                      Output.i Output_doc.i
                      DataSample.i DataSample_doc.i
                      DataImport.i DataImport_doc.i
                      DataFieldModel.i DataFieldModel_doc.i
                      DataFieldModelImplementation.i DataFieldModelImplementation_doc.i
                      MeshModelImplementation.i MeshModelImplementation_doc.i
                      MeshModel.i MeshModel_doc.i
                      GridMeshModel.i GridMeshModel_doc.i
                      ImportedMeshModel.i ImportedMeshModel_doc.i
                      PhysicalModelImplementation.i PhysicalModelImplementation_doc.i
                      SymbolicPhysicalModel.i SymbolicPhysicalModel_doc.i
                      PythonPhysicalModel.i PythonPhysicalModel_doc.i
                      SymbolicFieldModel.i SymbolicFieldModel_doc.i
                      PythonFieldModel.i PythonFieldModel_doc.i
                      MetaModel.i MetaModel_doc.i
                      PhysicalModel.i PhysicalModel_doc.i
                      YACSPhysicalModel.i YACSPhysicalModel_doc.i
                      YACSCouplingPhysicalModel.i YACSCouplingPhysicalModel_doc.i
                      FMIPhysicalModel.i FMIPhysicalModel_doc.i
                      FMUInfo.i FMUInfo_doc.i
                      LimitStateImplementation.i LimitStateImplementation_doc.i
                      LimitState.i LimitState_doc.i
                      AnalysisResult.i AnalysisResult_doc.i
                      EvaluationResult.i EvaluationResult_doc.i
                      AnalysisImplementation.i AnalysisImplementation_doc.i
                      DesignOfExperimentAnalysis.i DesignOfExperimentAnalysis_doc.i
                      PhysicalModelAnalysis.i PhysicalModelAnalysis_doc.i
                      Analysis.i Analysis_doc.i
                      WithStopCriteriaAnalysis.i WithStopCriteriaAnalysis_doc.i
                      SimulationAnalysis.i SimulationAnalysis_doc.i
                      DesignOfExperimentEvaluation.i DesignOfExperimentEvaluation_doc.i
                      DataAnalysisResult.i DataAnalysisResult_doc.i
                      DataAnalysis.i DataAnalysis_doc.i
                      QuantileAnalysisResult.i QuantileAnalysisResult_doc.i
                      QuantileAnalysis.i QuantileAnalysis_doc.i
                      ModelEvaluation.i ModelEvaluation_doc.i
                      FieldModelEvaluation.i FieldModelEvaluation_doc.i
                      DesignOfExperimentImplementation.i DesignOfExperimentImplementation_doc.i
                      DesignOfExperiment.i DesignOfExperiment_doc.i
                      FixedDesignOfExperiment.i FixedDesignOfExperiment_doc.i
                      GridDesignOfExperiment.i GridDesignOfExperiment_doc.i
                      ImportedDesignOfExperiment.i ImportedDesignOfExperiment_doc.i
                      ProbabilisticDesignOfExperiment.i ProbabilisticDesignOfExperiment_doc.i
                      DataModel.i DataModel_doc.i
                      Observations.i Observations_doc.i
                      MonteCarloAnalysis.i MonteCarloAnalysis_doc.i
                      FieldMonteCarloResult.i FieldMonteCarloResult_doc.i
                      FieldMonteCarloAnalysis.i FieldMonteCarloAnalysis_doc.i
                      FieldKarhunenLoeveAnalysis.i FieldKarhunenLoeveAnalysis_doc.i
                      TaylorExpansionMomentsResult.i TaylorExpansionMomentsResult_doc.i
                      TaylorExpansionMomentsAnalysis.i TaylorExpansionMomentsAnalysis_doc.i
                      SobolResult.i SobolResult_doc.i
                      SobolAnalysis.i SobolAnalysis_doc.i
                      SRCResult.i SRCResult_doc.i
                      SRCAnalysis.i SRCAnalysis_doc.i
                      MorrisResult.i MorrisResult_doc.i
                      MorrisAnalysis.i MorrisAnalysis_doc.i
                      ReliabilityAnalysis.i ReliabilityAnalysis_doc.i
                      SimulationReliabilityAnalysis.i SimulationReliabilityAnalysis_doc.i
                      SimulationReliabilityResult.i SimulationReliabilityResult_doc.i
                      MonteCarloReliabilityAnalysis.i MonteCarloReliabilityAnalysis_doc.i
                      ImportanceSamplingAnalysis.i ImportanceSamplingAnalysis_doc.i
                      ApproximationAnalysis.i ApproximationAnalysis_doc.i
                      FORMAnalysisResult.i FORMAnalysisResult_doc.i
                      FORMAnalysis.i FORMAnalysis_doc.i
                      SORMAnalysisResult.i SORMAnalysisResult_doc.i
                      SORMAnalysis.i SORMAnalysis_doc.i
                      FORMImportanceSamplingAnalysis.i FORMImportanceSamplingAnalysis_doc.i
                      MetaModelValidationResult.i MetaModelValidationResult_doc.i
                      MetaModelAnalysisResult.i MetaModelAnalysisResult_doc.i
                      MetaModelAnalysis.i MetaModelAnalysis_doc.i
                      FunctionalChaosAnalysisResult.i FunctionalChaosAnalysisResult_doc.i
                      FunctionalChaosAnalysis.i FunctionalChaosAnalysis_doc.i
                      KrigingAnalysisResult.i KrigingAnalysisResult_doc.i
                      KrigingAnalysis.i KrigingAnalysis_doc.i
                      PolynomialRegressionAnalysisResult.i PolynomialRegressionAnalysisResult_doc.i
                      PolynomialRegressionAnalysis.i PolynomialRegressionAnalysis_doc.i
                      FittingTestResult.i FittingTestResult_doc.i
                      InferenceResult.i InferenceResult_doc.i
                      InferenceAnalysis.i InferenceAnalysis_doc.i
                      CopulaInferenceSetResult.i CopulaInferenceSetResult_doc.i
                      CopulaInferenceResult.i CopulaInferenceResult_doc.i
                      CopulaInferenceAnalysis.i CopulaInferenceAnalysis_doc.i
                      OptimizationAnalysis.i OptimizationAnalysis_doc.i
                      MultiObjectiveOptimizationAnalysis.i MultiObjectiveOptimizationAnalysis_doc.i
                      CalibrationAnalysis.i CalibrationAnalysis_doc.i
                      CalibrationAnalysisResult.i CalibrationAnalysisResult_doc.i
                      StudyImplementation.i
                      Study.i Study_doc.i
                      CouplingInputFile.i CouplingInputFile_doc.i
                      CouplingResourceFile.i CouplingResourceFile_doc.i
                      CouplingOutputFile.i CouplingOutputFile_doc.i
                      CouplingStep.i CouplingStep_doc.i
                      CouplingPhysicalModel.i CouplingPhysicalModel_doc.i
                      FileMemoizeEvaluation.i
                      FileMemoizeFunction.i FileMemoizeFunction_doc.i
                      DataCleaning.i DataCleaning_doc.i
                      AnsysParser.i AnsysParser_doc.i
                    )

if (SalomeYACS_FOUND)
  set_property (TARGET persalys_python APPEND PROPERTY SWIG_COMPILE_DEFINITIONS PERSALYS_HAVE_YACS=1)
endif ()

install ( FILES __init__.py
          DESTINATION ${PERSALYS_PYTHON_MODULE_PATH}/persalys
        )

if (SPHINX_FOUND)

  # create a build site-package from which sphinx can import
  set (PERSALYS_PYTHON_BUILD_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/site-packages)
  execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory ${PERSALYS_PYTHON_BUILD_MODULE_PATH}/persalys)
  foreach (module ${PERSALYS_PYTHON_MODULES})
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/${module}.py ${PERSALYS_PYTHON_BUILD_MODULE_PATH}/persalys/${module}.py)
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/_${module}.so ${PERSALYS_PYTHON_BUILD_MODULE_PATH}/persalys/_${module}.so)
  endforeach ()
  foreach (file __init__.py)
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/${file} ${PERSALYS_PYTHON_BUILD_MODULE_PATH}/persalys/${file})
  endforeach ()

  # sphinx configuration
  set (doc_formats)
  list (APPEND doc_formats html)
  list (APPEND doc_formats htmlhelp)
  list (APPEND doc_formats latex)
  list (APPEND doc_formats doctest)
  list (APPEND doc_formats qthelp)
  list (APPEND doc_formats devhelp)

  set (SPHINX_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/sphinx_build)
  set (SPHINX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../doc)

  file (MAKE_DIRECTORY ${SPHINX_BINARY_DIR}/_static) # currently empty

  configure_file (${SPHINX_SOURCE_DIR}/conf.py.in ${SPHINX_BINARY_DIR}/conf.py @ONLY)

  # get sphinx sources
  file (GLOB_RECURSE sphinx_sources
    ${SPHINX_SOURCE_DIR}/*.rst
    ${SPHINX_SOURCE_DIR}/*.rst_t
    ${SPHINX_SOURCE_DIR}/*.css_t
    ${SPHINX_SOURCE_DIR}/*.conf
    ${SPHINX_SOURCE_DIR}/*.py
    ${SPHINX_SOURCE_DIR}/*.png
    ${SPHINX_SOURCE_DIR}/*.html
    ${SPHINX_SOURCE_DIR}/*.css
    ${SPHINX_SOURCE_DIR}/*.jpg
    ${SPHINX_SOURCE_DIR}/*.ico
    ${SPHINX_SOURCE_DIR}/*.sty
    ${SPHINX_SOURCE_DIR}/*.txt
    ${SPHINX_SOURCE_DIR}/*.in
    ${SPHINX_SOURCE_DIR}/*.inv
  )

  # copy sphinx sources
  set (sphinx_clones)
  foreach (file ${sphinx_sources})
    if (EXISTS ${file})
      file ( RELATIVE_PATH rel_file ${SPHINX_SOURCE_DIR} ${file} )
      get_filename_component (rel_path ${rel_file} PATH)
      file (MAKE_DIRECTORY ${SPHINX_BINARY_DIR}/${rel_path})
      set (sphinx_clone ${SPHINX_BINARY_DIR}/${rel_file})
      add_custom_command (OUTPUT ${sphinx_clone}
        COMMAND ${CMAKE_COMMAND} -E copy ${file} ${sphinx_clone}
        DEPENDS ${file}
      )
      list (APPEND sphinx_clones ${sphinx_clone})
    endif ()
  endforeach ()

  # copy python tests in root
  file (GLOB_RECURSE python_tests
    ${CMAKE_SOURCE_DIR}/python/test/t_*.py
  )
  foreach (file ${python_tests})
    if (EXISTS ${file})
      get_filename_component (name ${file} NAME)
      set (sphinx_clone ${SPHINX_BINARY_DIR}/${name})
      add_custom_command (OUTPUT ${sphinx_clone}
        COMMAND ${CMAKE_COMMAND} -E copy ${file} ${sphinx_clone}
        DEPENDS ${file}
      )
      list (APPEND sphinx_clones ${sphinx_clone})
    endif ()
  endforeach()

  set (SPHINX_FLAGS "-W" CACHE STRING "sphinx flags")
  separate_arguments (SPHINX_FLAGS)

  # generate sphinx rule for each format
  foreach (format ${doc_formats})
    add_custom_target ( sphinx_${format}
                        COMMAND ${SPHINX_EXECUTABLE}
                                  -b ${format}
                                  -d ${SPHINX_BINARY_DIR}/.doctrees
                                  ${SPHINX_FLAGS}
                                  ${SPHINX_BINARY_DIR}
                                  ${SPHINX_BINARY_DIR}/${format}
                        DEPENDS ${sphinx_clones}
                      )
    foreach (MODULENAME ${PERSALYS_PYTHON_MODULES})
      add_dependencies (sphinx_${format} ${MODULENAME}_python)
    endforeach ()
  endforeach ()

  # build html by default
  set_target_properties (sphinx_html PROPERTIES EXCLUDE_FROM_ALL OFF)

  add_custom_command (OUTPUT ${SPHINX_BINARY_DIR}/latex/OpenTURNS.pdf
                      WORKING_DIRECTORY ${SPHINX_BINARY_DIR}/latex
                      COMMAND make all-pdf
                    )
  add_custom_target (sphinx_pdf DEPENDS ${SPHINX_BINARY_DIR}/latex/OpenTURNS.pdf)
  add_dependencies (sphinx_pdf sphinx_latex)

  install (DIRECTORY ${SPHINX_BINARY_DIR}/html DESTINATION ${PERSALYS_DOC_PATH})
endif ()
