find_path(GLFW3_INCLUDE_DIR GLFW/glfw3.h)
find_library(GLFW3_LIBRARY NAMES glfw3 glfw PATH_SUFFIXES lib lib64)

if(NOT GLFW3_LIBRARY AND WIN32)
	set(GLFW3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/usr/include")
	set(GLFW3_LIBRARY "${CMAKE_SOURCE_DIR}/usr/lib/glfw3.lib")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(glfw3 DEFAULT_MSG GLFW3_LIBRARY GLFW3_INCLUDE_DIR)

mark_as_advanced(GLFW3_INCLUDE_DIR GLFW3_LIBRARY)
