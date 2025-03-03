cmake_minimum_required(VERSION 3.21)
project(gl-mat4)

function(add_shader FILE_NAME VAR_NAME)
	add_custom_command(
		OUTPUT ${FILE_NAME}.cpp
		COMMAND ${CMAKE_COMMAND} -DSRC_PATH=${CMAKE_CURRENT_SOURCE_DIR} -DSHADER_FILENAME=${FILE_NAME} -DSTR_VAR_NAME=${VAR_NAME} -P ${CMAKE_CURRENT_SOURCE_DIR}/MakeShaderResources.cmake
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}
	)
endfunction()

add_shader("Broken.comp" "brokenCompSrc")
add_shader("Good.comp" "goodCompSrc")
add_library(
	shaders
	${CMAKE_CURRENT_BINARY_DIR}/Broken.comp.cpp
	${CMAKE_CURRENT_BINARY_DIR}/Good.comp.cpp
)

if (WIN32)
	include_directories(usr/include)
	set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/usr")
	set(CMAKE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/usr/lib")
	set(CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/usr/include")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/usr/cmake")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(gl-mat4 main.cpp Utils.cpp)
target_link_libraries(gl-mat4 shaders OpenGL::GL GLEW::GLEW ${GLFW3_LIBRARY})

target_link_options(gl-mat4 PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/SUBSYSTEM:CONSOLE>")
