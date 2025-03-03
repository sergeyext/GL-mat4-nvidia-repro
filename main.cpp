#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

// #ifdef _MSC_VER
// #include <windows.h>
// extern "C" {
// 	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
// 	_declspec(dllexport) DWORD NvOptimusEnablement = 1;
// }
// #endif

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Utils.h"

extern std::string brokenCompSrc;
extern std::string goodCompSrc;

void printMat(const std::vector<float>& mat)
{
	assert(mat.size() > 11);
	std::cout << mat[3] << " " << mat[7] << " " << mat[11] << "\n";
}

int main()
{
	// Init
	if (!glfwInit()) {
		std::cerr << "Error: glfw init failed." << std::endl;
		return 3;
	}

	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_ALPHA_BITS, 0);
	glfwWindowHint(GLFW_STENCIL_BITS, 0);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	static constexpr int width = 100;
	static constexpr int height= 100;
	GLFWwindow* window = nullptr;
	window = glfwCreateWindow(width, height, "Terrain", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Error: window is null." << std::endl;
		glfwTerminate();
		return 6;
	}

	glfwMakeContextCurrent(window);

	const auto glewOk = glewInit();
	if (glewOk != GLEW_OK) {
		std::cerr << "Error: glew not OK." << std::endl;
		glfwTerminate();
		return 7;
	}

	// glDebugMessageCallback(printOpenglDebugMessage, nullptr);
	// glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	const auto brokenShader = createComputeProgram(brokenCompSrc);
	const auto goodShader = createComputeProgram(goodCompSrc);

	assert(!error());

	std::cout << (char*)glGetString(GL_VERSION) << "\n";
	std::cout << (char*)glGetString(GL_RENDERER) << "\n";
	std::cout << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

	constexpr auto outSizeBytes = 65536;
	GLuint outSsbo0 = 0;
	glCreateBuffers(1, &outSsbo0);
	assert(outSsbo0 != 0);
	glNamedBufferStorage(outSsbo0, outSizeBytes, nullptr, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, outSsbo0);
	assert(!error());

	static constexpr auto fetchSize = 12;
	std::vector<float> outData(fetchSize);

	glUseProgram(brokenShader);
	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glGetNamedBufferSubData(outSsbo0, 0, fetchSize * sizeof(float), outData.data());
	printMat(outData);

	glUseProgram(goodShader);
	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glGetNamedBufferSubData(outSsbo0, 0, fetchSize * sizeof(float), outData.data());
	printMat(outData);

	// Finalize
	glfwMakeContextCurrent(nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
