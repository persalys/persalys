
set ( PATH_SEP ":" )
if (WIN32)
  # replace dos eols
  set (REPLACE_EOL " | tr -d '\\\\015' ")
  set ( PATH_SEP "\\;" )
endif ()

set (CHECK_ENVIRONMENT
     "PYTHONPATH=${OPENTURNS_PYTHON_MODULE_PATH}${PATH_SEP}${OTMORRIS_PYTHON_MODULE_PATH}${PATH_SEP}${INSTALL_DESTDIR}${PYTHON_MODULE_PATH}${PATH_SEP}$ENV{PYTHONPATH}"
    )

set ( CHECK_TO_BE_RUN )
macro ( ot_check_test TESTNAME )
  set ( TESTNAME_TGT t_${TESTNAME} )
  set ( TESTNAME_SRC ${TESTNAME_TGT}.cxx )
  set ( TESTNAME_SRCS ${TESTNAME_SRC} )
  add_executable ( ${TESTNAME_TGT} EXCLUDE_FROM_ALL ${TESTNAME_SRCS} )
  add_dependencies ( tests ${TESTNAME_TGT} )
  target_link_libraries ( ${TESTNAME_TGT} persalysbase )
  set_target_properties ( ${TESTNAME_TGT} PROPERTIES
                                          UNITY_BUILD OFF
                                          INSTALL_RPATH "${CMAKE_BINARY_DIR}/lib/src;${CMAKE_INSTALL_RPATH}" )
  set ( TESTNAME_LOC ${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME_TGT}${CMAKE_EXECUTABLE_SUFFIX} )
  set (_IGNOREOUT OFF)
  foreach (_ARG ${ARGN})
    if (${_ARG} MATCHES IGNOREOUT)
      set (_IGNOREOUT ON)
    endif ()
  endforeach ()

  if (_IGNOREOUT)
    set ( COMMAND "( ${CMAKE_CROSSCOMPILING_EMULATOR} ${TESTNAME_LOC} ${_PARAMS} > /dev/null )" )
  else ()
    set ( TESTNAME_OUT ${TESTNAME_TGT}.expout )
    set ( OUTFILE_LOC ${CMAKE_CURRENT_SOURCE_DIR}/${TESTNAME_OUT} )
    set ( COMMAND "( ${CMAKE_CROSSCOMPILING_EMULATOR} ${TESTNAME_LOC} ${_PARAMS} ${REPLACE_EOL} > ${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME_TGT}.out ) && diff -u ${OUTFILE_LOC} ${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME_TGT}.out" )
  endif ()

  add_test ( cppcheck_${TESTNAME} "sh" "-c" "${COMMAND}" )
  set_tests_properties ( cppcheck_${TESTNAME} PROPERTIES ENVIRONMENT "${CHECK_ENVIRONMENT}" )
  list ( APPEND CHECK_TO_BE_RUN ${TESTNAME_TGT} )
endmacro ()


macro (ot_qt_check_test TESTNAME)
  set (TESTNAME_TGT t_${TESTNAME})
  set (TESTNAME_MOC ${TESTNAME_TGT}.moc)
  set (TESTNAME_SRC ${TESTNAME_TGT}.cxx)
  set (TESTNAME_SRCS ${TESTNAME_SRC} ${TESTNAME_MOC})
  qt_generate_moc (${TESTNAME_SRC} ${TESTNAME_MOC})
  add_executable (${TESTNAME_TGT} EXCLUDE_FROM_ALL ${TESTNAME_SRCS})
  add_dependencies (tests ${TESTNAME_TGT} )
  target_link_libraries (${TESTNAME_TGT} persalysview)
  set_target_properties (${TESTNAME_TGT} PROPERTIES UNITY_BUILD OFF)
  set (TEST_ARGUMENTS "${ARGV}")
  list (REMOVE_AT TEST_ARGUMENTS 0)

  # for MinGW from Linux QTest does not print anything when stdout is redirected, see QPlainTestLogger::outputMessage
  if (MINGW AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    list (APPEND TEST_ARGUMENTS "-o;/dev/stderr,txt")
  endif ()

  add_test (NAME cppcheck_${TESTNAME} COMMAND ${TESTNAME_TGT} ${TEST_ARGUMENTS})
  set_tests_properties (cppcheck_${TESTNAME} PROPERTIES ENVIRONMENT "${CHECK_ENVIRONMENT}")
endmacro ()


include_directories ( ${INTERNAL_INCLUDE_DIRS} )


ot_check_test ( PhysicalModel_std )
ot_check_test ( Cogeneration_std )
if (NOT CMAKE_CROSSCOMPILING)
  ot_check_test ( PythonScriptEvaluation_std )
endif()


if (BUILD_GUI)
  include_directories (${CMAKE_CURRENT_BINARY_DIR}) # for mocs
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test_deterministic_analyses.xml ${CMAKE_CURRENT_BINARY_DIR})
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test_probabilistic_analyses.xml ${CMAKE_CURRENT_BINARY_DIR})
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test_field_analyses.xml ${CMAKE_CURRENT_BINARY_DIR})
  ot_qt_check_test (WelcomeWindow_std)
  ot_qt_check_test (MainWindow_std -platform minimal)
  ot_qt_check_test (OptimizationWizard_std)
  ot_qt_check_test (CentralTendencyWizard_std)
  ot_qt_check_test (SensitivityAnalysisWizard_std)
  if (OTMORRIS_FOUND)
    ot_qt_check_test (ScreeningAnalysisWizard_std)
  endif ()
  ot_qt_check_test (ModelEvaluationWizard_std)
  ot_qt_check_test (DesignOfExperimentEvaluationWizard_std)
  ot_qt_check_test (ReliabilityAnalysisWizard_std)
  ot_qt_check_test (MetaModelAnalysisWizard_std)
  ot_qt_check_test (DesignOfExperimentWizard_std)
  ot_qt_check_test (FieldMonteCarloWizard_std)
  ot_qt_check_test (MeshDefinitionWizard_std -platform minimal)
  ot_qt_check_test (CalibrationAnalysisWizard_std -platform minimal)
  ot_qt_check_test (ObservationsWizard_std)
  ot_qt_check_test (SymbolicPhysicalModelWindow_std)
  ot_qt_check_test (ProbabilisticModelWindow_std -platform minimal)
  ot_qt_check_test (DataModelWindow_std)
  ot_qt_check_test (QtTools_std)
  if (UNIX)
    set (PLATFORM_OFFSCREEN -platform offscreen)
  endif ()
  ot_qt_check_test (MonteCarloResultWindow_std ${PLATFORM_OFFSCREEN})
endif ()

add_custom_target ( cppcheck COMMAND ${CMAKE_CTEST_COMMAND} -R "^cppcheck_"
                    DEPENDS ${CHECK_TO_BE_RUN}
                    COMMENT "Run C++ pre-installation tests" )

if (SalomeYACS_FOUND)
  ot_check_test (YACSPhysicalModel_std IGNOREOUT)
  set_tests_properties (cppcheck_YACSPhysicalModel_std PROPERTIES RUN_SERIAL ON)
endif ()
