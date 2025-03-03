#pragma once

#include <string>
//#define GLEW_STATIC
#include <GL/glew.h>

std::string getErrorDescr(GLenum errCode);
std::string getErrorMessage();
bool error();
GLuint createProgram(const std::string& vertSource, const std::string& fragSource);
GLuint createComputeProgram(const std::string& compSource);
void printOpenglDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void*);
