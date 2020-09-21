find_package(OpenGL REQUIRED)
# find_package(SDL2 REQUIRED)

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
  find_package(X11 REQUIRED)

  if (NOT X11_Xi_FOUND)
    message(FATAL_ERROR "X11 Xi library is required")
  endif ()
endif ()

include(cmake/plog.cmake)
include(cmake/glfw.cmake)
include(cmake/stb.cmake)
include(cmake/glad.cmake)
include(cmake/imgui.cmake)
