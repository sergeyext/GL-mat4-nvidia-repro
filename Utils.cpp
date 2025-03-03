#include "Utils.h"

#include <iostream>
#include <random>
#include <sstream>


std::string getErrorDescr(const GLenum errCode)
{
	// English descriptions are from
	// https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGetError.xml
	switch (errCode) {
		case GL_NO_ERROR: return "No error has been recorded. THIS message is the error itself.";
		case GL_INVALID_ENUM: return "An unacceptable value is specified for an enumerated argument.";
		case GL_INVALID_VALUE: return "A numeric argument is out of range.";
		case GL_INVALID_OPERATION: return "The specified operation is not allowed in the current state.";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "The framebuffer object is not complete.";
		case GL_OUT_OF_MEMORY: return "There is not enough memory left to execute the command.";
		case GL_STACK_UNDERFLOW: return "An attempt has been made to perform an operation that would cause an internal stack to underflow.";
		case GL_STACK_OVERFLOW: return "An attempt has been made to perform an operation that would cause an internal stack to overflow.";
		default:;
	}
	return "No description available.";
}

std::string getErrorMessage()
{
	const GLenum error = glGetError();
	if (GL_NO_ERROR == error) return "";

	std::stringstream ss;
	ss << "OpenGL error: " << static_cast<int>(error) << std::endl;
	ss << "Error string: ";
	ss << getErrorDescr(error);
	ss << std::endl;
	return ss.str();
}

bool error()
{
	const auto message = getErrorMessage();
	if (message.length() == 0) return false;
	std::cerr << message;
	return true;
}

static bool compileShader(const GLuint shader, const std::string& source)
{
	unsigned int linesCount = 0;
	for (const auto c: source) linesCount += static_cast<unsigned int>(c == '\n');
	const auto sourceLines = new const char*[linesCount];
	const auto lengths = new int[linesCount];

	int idx = 0;
	const char* lineStart = source.data();
	int lineLength = 1;
	const auto len = source.length();
	for (unsigned int i = 0; i < len; ++i) {
		if (source[i] == '\n') {
			sourceLines[idx] = lineStart;
			lengths[idx] = lineLength;
			lineLength = 1;
			lineStart = source.data() + i + 1;
			++idx;
		}
		else ++lineLength;
	}

	glShaderSource(shader, linesCount, sourceLines, lengths);
	glCompileShader(shader);
	GLint logLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		auto* const log = new GLchar[logLength + 1];
		glGetShaderInfoLog(shader, logLength, nullptr, log);
		std::cout << "Log: " << std::endl;
		std::cout << log;
		delete[] log;
	}

	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	delete[] sourceLines;
	delete[] lengths;
	return bool(compileStatus);
}

GLuint createProgram(const std::string& vertSource, const std::string& fragSource)
{
	const auto vs = glCreateShader(GL_VERTEX_SHADER);
	if (vs == 0) {
		std::cerr << "Error: vertex shader is 0." << std::endl;
		return 2;
	}
	const auto fs = glCreateShader(GL_FRAGMENT_SHADER);
	if (fs == 0) {
		std::cerr << "Error: fragment shader is 0." << std::endl;
		return 2;
	}

	// Compile shaders
	if (!compileShader(vs, vertSource)) {
		std::cerr << "Error: could not compile vertex shader." << std::endl;
		return 5;
	}
	if (!compileShader(fs, fragSource)) {
		std::cerr << "Error: could not compile fragment shader." << std::endl;
		return 5;
	}

	// Link program
	const auto program = glCreateProgram();
	if (program == 0) {
		std::cerr << "Error: program is 0." << std::endl;
		return 2;
	}
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	// Get log
	GLint logLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength > 0) {
		auto* const log = new GLchar[logLength + 1];
		glGetProgramInfoLog(program, logLength, nullptr, log);
		std::cout << "Log: " << std::endl;
		std::cout << log;
		delete[] log;
	}
	GLint linkStatus = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus) {
		std::cerr << "Error: could not link." << std::endl;
		return 2;
	}
	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}

GLuint createComputeProgram(const std::string& compSource)
{
	const auto cs = glCreateShader(GL_COMPUTE_SHADER);
	if (cs == 0) {
		std::cerr << "Error: compute shader is 0." << std::endl;
		return 2;
	}

	// Compile shaders
	if (!compileShader(cs, compSource)) {
		std::cerr << "Error: could not compile compute shader." << std::endl;
		return 5;
	}

	// Link program
	const auto program = glCreateProgram();
	if (program == 0) {
		std::cerr << "Error: program is 0." << std::endl;
		return 2;
	}
	glAttachShader(program, cs);
	glLinkProgram(program);

	// Get log
	GLint logLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength > 0) {
		auto* const log = new GLchar[logLength + 1];
		glGetProgramInfoLog(program, logLength, nullptr, log);
		std::cout << "Log: " << std::endl;
		std::cout << log;
		delete[] log;
	}
	GLint linkStatus = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus) {
		std::cerr << "Error: could not link." << std::endl;
		return 2;
	}
	glDeleteShader(cs);
	return program;
}

void printOpenglDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void*)
{
	std::string sourceStr;
	switch (source) {
		case GL_DEBUG_SOURCE_API: sourceStr = "GL API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceStr = "window system"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "shader compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: sourceStr = "third party"; break;
		case GL_DEBUG_SOURCE_APPLICATION: sourceStr = "application"; break;
		case GL_DEBUG_SOURCE_OTHER: sourceStr = "other"; break;
		default: sourceStr = "unknown"; break;
	}

	std::string typeStr;
	switch (type) {
		case GL_DEBUG_TYPE_ERROR: typeStr = "error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "deprecated behavior"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeStr = "undefined behavior"; break;
		case GL_DEBUG_TYPE_PORTABILITY: typeStr = "portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: typeStr = "performance"; break;
		case GL_DEBUG_TYPE_MARKER: typeStr = "marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP: typeStr = "push group"; break;
		case GL_DEBUG_TYPE_POP_GROUP: typeStr = "pop group"; break;
		case GL_DEBUG_TYPE_OTHER: typeStr = "other"; break;
		default: typeStr = "unknown"; break;
	};

	std::string severityStr;
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: severityStr = "high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: severityStr = "medium"; break;
		case GL_DEBUG_SEVERITY_LOW: severityStr = "low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "notification"; break;
		default: severityStr = "unknown"; break;
	};

	std::stringstream ss;
	ss << "*** GL debug ***" << std::endl;
	ss << "Source: " << sourceStr << std::endl;
	ss << "Type: " << typeStr << std::endl;
	ss << "Severity: " << severityStr << std::endl;
	ss << "Id: " << id << std::endl;
	ss << "Message: " << std::string(msg, length) << std::endl;
	ss << std::endl;
	std::cout << ss.str();
}
