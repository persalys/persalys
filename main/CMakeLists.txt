add_executable (gui main.cxx)
include_directories (${CMAKE_SOURCE_DIR}/lib/src/base)
include_directories (${CMAKE_SOURCE_DIR}/lib/src/model)

include_directories (${CMAKE_SOURCE_DIR}/lib/src/view)
include_directories (${CMAKE_SOURCE_DIR}/lib/src/view/pythonConsole)

target_link_libraries (gui otguiview)


install (TARGETS gui DESTINATION bin)
