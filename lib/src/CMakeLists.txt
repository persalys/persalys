
add_subdirectory (base)

if (BUILD_GUI)
  add_subdirectory (model)
  add_subdirectory (view)
endif ()
