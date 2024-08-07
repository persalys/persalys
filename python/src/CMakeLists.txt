
include (${SWIG_USE_FILE})

include_directories (${OPENTURNS_INCLUDE_DIRS})
include_directories (${OPENTURNS_SWIG_INCLUDE_DIRS})
add_definitions (${OPENTURNS_SWIG_DEFINITIONS})
if (OTMORRIS_FOUND)
  include_directories (${OTMORRIS_INCLUDE_DIR}/otmorris/swig )
endif ()
include_directories (${INTERNAL_INCLUDE_DIRS})
include_directories (${Python_INCLUDE_DIRS})

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
    elseif (${it} MATCHES ".*_doc\\.i\\.in$")
      list (APPEND docstring_sources ${it})

      get_filename_component (DOCSTRING_FILE_BASENAME ${it} NAME_WE)
      ot_install_swig_file (${CMAKE_CURRENT_BINARY_DIR}/${DOCSTRING_FILE_BASENAME}.i)
    else ()
      list (APPEND swig_other_sources "${it}")
    endif ()
  endforeach ()

  # https://github.com/swig/swig/issues/1273
  option (SWIG_FORCE_DOUBLE_ESCAPE_BACKSLASH "Force double escaping of backslashes" OFF)
  mark_as_advanced (SWIG_FORCE_DOUBLE_ESCAPE_BACKSLASH)
  set (TIMES x1)
  if ((SWIG_VERSION VERSION_LESS 4) OR SWIG_FORCE_DOUBLE_ESCAPE_BACKSLASH)
    set (TIMES x2)
  endif ()

  # handle docstring generation
  add_custom_target (generate_${MODULENAME}_docstrings
                      COMMAND ${CMAKE_COMMAND} -DDOCSTRING_SOURCES="${docstring_sources}"
                      -DCURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DTIMES=${TIMES}
                      -P ${CMAKE_SOURCE_DIR}/cmake/escape_backslash.cmake
                      DEPENDS ${docstring_sources})
  list (APPEND SWIG_MODULE_${MODULENAME}_python_EXTRA_DEPS generate_${MODULENAME}_docstrings)

  # swig_add_module is deprecated
  swig_add_library (${MODULENAME}_python LANGUAGE python SOURCES ${SOURCEFILE} ${swig_other_sources})

  # UseSWIG generates now standard target names
  set (module_target ${MODULENAME}_python)

  swig_link_libraries (${module_target} persalysbase)
  target_include_directories (${module_target} PRIVATE ${Python_INCLUDE_DIRS})

  if (TARGET Python::Module)
    target_link_libraries (${module_target} Python::Module)
  else ()
    target_link_libraries_with_dynamic_lookup (${module_target} ${Python_LIBRARIES})
  endif ()

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
                      Variable.i Variable_doc.i.in
                      Input.i Input_doc.i.in
                      Output.i Output_doc.i.in
                      DataSample.i DataSample_doc.i.in
                      DataImport.i DataImport_doc.i.in
                      MeshModelImplementation.i MeshModelImplementation_doc.i.in
                      MeshModel.i MeshModel_doc.i.in
                      GridMeshModel.i GridMeshModel_doc.i.in
                      ImportedMeshModel.i ImportedMeshModel_doc.i.in
                      PhysicalModelImplementation.i PhysicalModelImplementation_doc.i.in
                      SymbolicPhysicalModel.i SymbolicPhysicalModel_doc.i.in
                      PythonPhysicalModel.i PythonPhysicalModel_doc.i.in
                      SymbolicFieldModel.i SymbolicFieldModel_doc.i.in
                      PythonFieldModel.i PythonFieldModel_doc.i.in
                      MetaModel.i MetaModel_doc.i.in
                      PhysicalModel.i PhysicalModel_doc.i.in
                      YACSPhysicalModel.i YACSPhysicalModel_doc.i.in
                      YACSCouplingPhysicalModel.i YACSCouplingPhysicalModel_doc.i.in
                      FMIPhysicalModel.i FMIPhysicalModel_doc.i.in
                      FMUInfo.i FMUInfo_doc.i.in
                      LimitStateImplementation.i LimitStateImplementation_doc.i.in
                      LimitState.i LimitState_doc.i.in
                      AnalysisResult.i AnalysisResult_doc.i.in
                      EvaluationResult.i EvaluationResult_doc.i.in
                      AnalysisImplementation.i AnalysisImplementation_doc.i.in
                      DesignOfExperimentAnalysis.i DesignOfExperimentAnalysis_doc.i.in
                      PhysicalModelAnalysis.i PhysicalModelAnalysis_doc.i.in
                      Analysis.i Analysis_doc.i.in
                      WithStopCriteriaAnalysis.i WithStopCriteriaAnalysis_doc.i.in
                      SimulationAnalysis.i SimulationAnalysis_doc.i.in
                      DesignOfExperimentEvaluation.i DesignOfExperimentEvaluation_doc.i.in
                      DataAnalysisResult.i DataAnalysisResult_doc.i.in
                      DataAnalysis.i DataAnalysis_doc.i.in
                      ModelEvaluation.i ModelEvaluation_doc.i.in
                      FieldModelEvaluation.i FieldModelEvaluation_doc.i.in
                      DesignOfExperimentImplementation.i DesignOfExperimentImplementation_doc.i.in
                      DesignOfExperiment.i DesignOfExperiment_doc.i.in
                      FixedDesignOfExperiment.i FixedDesignOfExperiment_doc.i.in
                      GridDesignOfExperiment.i GridDesignOfExperiment_doc.i.in
                      ImportedDesignOfExperiment.i ImportedDesignOfExperiment_doc.i.in
                      ProbabilisticDesignOfExperiment.i ProbabilisticDesignOfExperiment_doc.i.in
                      DataModel.i DataModel_doc.i.in
                      Observations.i Observations_doc.i.in
                      MonteCarloAnalysis.i MonteCarloAnalysis_doc.i.in
                      FieldMonteCarloResult.i FieldMonteCarloResult_doc.i.in
                      FieldMonteCarloAnalysis.i FieldMonteCarloAnalysis_doc.i.in
                      TaylorExpansionMomentsResult.i TaylorExpansionMomentsResult_doc.i.in
                      TaylorExpansionMomentsAnalysis.i TaylorExpansionMomentsAnalysis_doc.i.in
                      SobolResult.i SobolResult_doc.i.in
                      SobolAnalysis.i SobolAnalysis_doc.i.in
                      SRCResult.i SRCResult_doc.i.in
                      SRCAnalysis.i SRCAnalysis_doc.i.in
                      MorrisResult.i MorrisResult_doc.i.in
                      MorrisAnalysis.i MorrisAnalysis_doc.i.in
                      ReliabilityAnalysis.i ReliabilityAnalysis_doc.i.in
                      SimulationReliabilityAnalysis.i SimulationReliabilityAnalysis_doc.i.in
                      SimulationReliabilityResult.i SimulationReliabilityResult_doc.i.in
                      MonteCarloReliabilityAnalysis.i MonteCarloReliabilityAnalysis_doc.i.in
                      ImportanceSamplingAnalysis.i ImportanceSamplingAnalysis_doc.i.in
                      ApproximationAnalysis.i ApproximationAnalysis_doc.i.in
                      FORMAnalysisResult.i FORMAnalysisResult_doc.i.in
                      FORMAnalysis.i FORMAnalysis_doc.i.in
                      SORMAnalysisResult.i SORMAnalysisResult_doc.i.in
                      SORMAnalysis.i SORMAnalysis_doc.i.in
                      FORMImportanceSamplingAnalysis.i FORMImportanceSamplingAnalysis_doc.i.in
                      MetaModelValidationResult.i MetaModelValidationResult_doc.i.in
                      MetaModelAnalysisResult.i MetaModelAnalysisResult_doc.i.in
                      MetaModelAnalysis.i MetaModelAnalysis_doc.i.in
                      FunctionalChaosAnalysisResult.i FunctionalChaosAnalysisResult_doc.i.in
                      FunctionalChaosAnalysis.i FunctionalChaosAnalysis_doc.i.in
                      KrigingAnalysisResult.i KrigingAnalysisResult_doc.i.in
                      KrigingAnalysis.i KrigingAnalysis_doc.i.in
                      PolynomialRegressionAnalysisResult.i PolynomialRegressionAnalysisResult_doc.i.in
                      PolynomialRegressionAnalysis.i PolynomialRegressionAnalysis_doc.i.in
                      FittingTestResult.i FittingTestResult_doc.i.in
                      InferenceResult.i InferenceResult_doc.i.in
                      InferenceAnalysis.i InferenceAnalysis_doc.i.in
                      CopulaInferenceSetResult.i CopulaInferenceSetResult_doc.i.in
                      CopulaInferenceResult.i CopulaInferenceResult_doc.i.in
                      CopulaInferenceAnalysis.i CopulaInferenceAnalysis_doc.i.in
                      OptimizationAnalysis.i OptimizationAnalysis_doc.i.in
                      MultiObjectiveOptimizationAnalysis.i MultiObjectiveOptimizationAnalysis_doc.i.in
                      CalibrationAnalysis.i CalibrationAnalysis_doc.i.in
                      CalibrationAnalysisResult.i CalibrationAnalysisResult_doc.i.in
                      StudyImplementation.i
                      Study.i Study_doc.i.in
                      CouplingInputFile.i CouplingInputFile_doc.i.in
                      CouplingResourceFile.i CouplingResourceFile_doc.i.in
                      CouplingOutputFile.i CouplingOutputFile_doc.i.in
                      CouplingStep.i CouplingStep_doc.i.in
                      CouplingPhysicalModel.i CouplingPhysicalModel_doc.i.in
                      FileMemoizeEvaluation.i
                      FileMemoizeFunction.i FileMemoizeFunction_doc.i.in
                      DataCleaning.i DataCleaning_doc.i.in
                      AnsysParser.i AnsysParser_doc.i.in
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
