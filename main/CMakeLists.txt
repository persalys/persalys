
include_directories (${CMAKE_SOURCE_DIR}/lib/include)

include_directories (${CMAKE_SOURCE_DIR}/lib/src/base)
include_directories (${CMAKE_SOURCE_DIR}/lib/src/model)

include_directories (${CMAKE_SOURCE_DIR}/lib/src/view)
include_directories (${CMAKE_SOURCE_DIR}/lib/src/view/PyConsole)

if (OTFMI_FOUND)
  # fmi make gui application crash, so use a console application
  add_executable (otgui main.cxx gui.rc)
else ()
  add_executable (otgui WIN32 main.cxx gui.rc)
endif ()

target_link_libraries (otgui otguiview)

if (UNIX AND NOT APPLE)
  set (launch_file ${CMAKE_BINARY_DIR}/gui_launch.sh)
  file (WRITE ${launch_file} "#!/bin/sh\n")
  if (OPENTURNS_PYTHON_MODULE_PATH)
    file (APPEND ${launch_file} "PYTHONPATH=$PYTHONPATH:${OPENTURNS_PYTHON_MODULE_PATH}\n")
  endif ()
  if (OPENTURNS_PYTHON2_MODULE_PATH)
    file (APPEND ${launch_file} "PYTHONPATH=$PYTHONPATH:${OPENTURNS_PYTHON2_MODULE_PATH}\n")
  endif ()
  foreach (PYTHON_MODULE_PATH ${OTGUI_PYTHON_MODULE_PATH})
    file (APPEND ${launch_file} "PYTHONPATH=$PYTHONPATH:${PYTHON_MODULE_PATH}\n")
  endforeach ()
  file (APPEND ${launch_file} "export PYTHONPATH\n")

  file (APPEND ${launch_file} "${CMAKE_CURRENT_BINARY_DIR}/otgui\n")

  execute_process (COMMAND chmod a+rx ${launch_file})
endif ()

install (TARGETS otgui DESTINATION bin)
