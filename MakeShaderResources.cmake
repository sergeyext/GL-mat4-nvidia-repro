
if (NOT SHADER_FILENAME)
	message(FATAL_ERROR "No SHADER_FILENAME.")
endif ()

if (NOT STR_VAR_NAME)
	message(FATAL_ERROR "No STR_VAR_NAME.")
endif ()

file(READ ${SRC_PATH}/${SHADER_FILENAME} SHADER_SOURCE)
file(WRITE ${SHADER_FILENAME}.cpp "#include <string>\n")
file(APPEND ${SHADER_FILENAME}.cpp "std::string ${STR_VAR_NAME} = R\"(\n")
file(APPEND ${SHADER_FILENAME}.cpp "${SHADER_SOURCE}")
file(APPEND ${SHADER_FILENAME}.cpp ")\";\n")
