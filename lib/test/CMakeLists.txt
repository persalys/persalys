
set ( PATH_SEP ":" )
if (WIN32)
  set ( PATH_SEP "\\;" )
endif ()

set (CHECK_ENVIRONMENT
     "PYTHONPATH=${OPENTURNS_PYTHON_MODULE_PATH}${PATH_SEP}${OTMORRIS_PYTHON_MODULE_PATH}${PATH_SEP}${INSTALL_DESTDIR}${PYTHON_MODULE_PATH}${PATH_SEP}$ENV{PYTHONPATH}"
    )

set ( CHECK_TO_BE_RUN )
macro ( ot_check_test TESTNAME )
  set (TEST_TARGET t_${TESTNAME})
  add_executable (${TEST_TARGET} EXCLUDE_FROM_ALL ${TEST_TARGET}.cxx)
  add_dependencies ( tests ${TEST_TARGET} )
  target_link_libraries ( ${TEST_TARGET} persalysbase )
  set_target_properties ( ${TEST_TARGET} PROPERTIES
                                          UNITY_BUILD OFF
                                          INSTALL_RPATH "${CMAKE_BINARY_DIR}/lib/src;${CMAKE_INSTALL_RPATH}" )
  set (_IGNOREOUT OFF)
  foreach (_ARG ${ARGN})
    if (${_ARG} MATCHES IGNOREOUT)
      set (_IGNOREOUT ON)
    endif ()
  endforeach ()

  if (_IGNOREOUT)
    add_test (NAME cppcheck_${TESTNAME} COMMAND ${TEST_TARGET})
  else ()
    set (OUTFILE_LOC ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_TARGET}.expout)
    if (NOT EXISTS ${OUTFILE_LOC})
      message (SEND_ERROR "Missing file ${OUTFILE_LOC}")
    endif ()
    add_test (NAME cppcheck_${TESTNAME} COMMAND "sh" "-c" "(${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:${TEST_TARGET}> > ${CMAKE_CURRENT_BINARY_DIR}/${TEST_TARGET}.out) && diff -u --strip-trailing-cr ${OUTFILE_LOC} ${CMAKE_CURRENT_BINARY_DIR}/${TEST_TARGET}.out")
  endif ()

  set_tests_properties (cppcheck_${TESTNAME} PROPERTIES ENVIRONMENT "${CHECK_ENVIRONMENT}")
  list (APPEND CHECK_TO_BE_RUN ${TEST_TARGET})
endmacro ()


macro (ot_qt_check_test TESTNAME)
  set (TEST_TARGET t_${TESTNAME})
  qt_generate_moc (${TEST_TARGET}.cxx ${TEST_TARGET}.moc)
  add_executable (${TEST_TARGET} EXCLUDE_FROM_ALL ${TEST_TARGET}.cxx ${TEST_TARGET}.moc)
  add_dependencies (tests ${TEST_TARGET} )
  target_link_libraries (${TEST_TARGET} persalysview)
  set_target_properties (${TEST_TARGET} PROPERTIES UNITY_BUILD OFF)
  set (TEST_ARGUMENTS "${ARGV}")
  list (REMOVE_AT TEST_ARGUMENTS 0)

  # for MinGW from Linux QTest does not print anything when stdout is redirected, see QPlainTestLogger::outputMessage
  if (MINGW AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    list (APPEND TEST_ARGUMENTS "-o;/dev/stderr,txt")
  endif ()

  add_test (NAME cppcheck_${TESTNAME} COMMAND ${TEST_TARGET} ${TEST_ARGUMENTS})
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
