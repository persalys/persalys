
include_directories (${CMAKE_SOURCE_DIR}/lib/include)

include_directories (${CMAKE_SOURCE_DIR}/lib/src/base)
include_directories (${CMAKE_SOURCE_DIR}/lib/src/model)

include_directories (${CMAKE_SOURCE_DIR}/lib/src/view)
include_directories (${CMAKE_SOURCE_DIR}/lib/src/view/PyConsole)

add_executable (persalys WIN32 main.cxx gui.rc)

target_link_libraries (persalys persalysview)
set_target_properties (persalys PROPERTIES UNITY_BUILD OFF)

if (UNIX)
  set (launch_file ${CMAKE_BINARY_DIR}/persalys.sh)
  file (WRITE ${launch_file} "#!/bin/sh\n")
  file (APPEND ${launch_file} "export PERSALYS_HTML_DOCUMENTATION_PATH=${CMAKE_INSTALL_PREFIX}/share/persalys/doc/html/\n")
  file (APPEND ${launch_file} "PYTHONPATH=$PYTHONPATH:${OPENTURNS_PYTHON_MODULE_PATH}\n")
  file (APPEND ${launch_file} "PYTHONPATH=$PYTHONPATH:${PYTHON_MODULE_PATH}\n")
  file (APPEND ${launch_file} "export PYTHONPATH\n")
  if (ParaView_FOUND)
    file (APPEND ${launch_file} "PV_PLUGIN_PATH=$PV_PLUGIN_PATH:${ParaView_PREFIX_PATH}/lib/${PARAVIEW_PLUGIN_SUBDIR}/BagPlotViewsAndFilters\n")
    file (APPEND ${launch_file} "export PV_PLUGIN_PATH\n")
  endif ()
  file (APPEND ${launch_file} "${CMAKE_CURRENT_BINARY_DIR}/persalys &\n")
  execute_process (COMMAND chmod a+rx ${launch_file})
endif ()

install (TARGETS persalys DESTINATION bin)

# desktop shortcut
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
  install (FILES persalys.desktop DESTINATION ${PERSALYS_DATA_PATH}/applications)
  file (COPY ../images/Ps-icon-64.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file (RENAME ${CMAKE_CURRENT_BINARY_DIR}/Ps-icon-64.png ${CMAKE_CURRENT_BINARY_DIR}/persalys-icon.png)
  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/persalys-icon.png DESTINATION ${PERSALYS_DATA_PATH}/icons/hicolor/64x64/apps/)
endif ()
