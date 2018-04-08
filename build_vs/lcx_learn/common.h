#pragma once

#include "sb7.h"
#include <string>
void MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

void printGPUInfo();

bool _CheckGLError(const char* file, int line);

#define CheckGLError() _CheckGLError(__FILE__, __LINE__)

std::string getPorgramInfoLog(GLuint program);
