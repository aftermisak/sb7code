// lcx_learn.cpp : Defines the entry point for the application.
//

#include "lcx_learn.h"
#include "common.h"
#include "cstdio"
#include <iostream>
#include <strstream>

void MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
	fflush(stderr);
	int i = 1;
	++i;
} 
 
void printGPUInfo() {
	const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字  
	const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台  
	const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //返回当前OpenGL实现的版本号  

	printf("OpenGL实现厂商的名字：%s\n", name);
	printf("渲染器标识符：%s\n", biaoshifu);
	printf("OpenGL实现的版本号：%s\n", OpenGLVersion);
	fflush(stdout);
}

bool _CheckGLError(const char* file, int line) {
	GLenum err(glGetError());
	bool ret = err == GL_NO_ERROR;//any error return false
	if (err == GL_NO_ERROR) {
		std::cout << "no opengl error" << std::endl;
		return true;
	}
	if (!ret)
		std::cout << "_____________________________GL_ check begin_____________________________" << std::endl;
	while (err != GL_NO_ERROR)
	{
		std::string error("unknow");
		switch (err)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
		err = glGetError();
	}
	if (!ret)
		std::cout << "_____________________________GL_ check end_____________________________" << std::endl;
	
	return ret;
}

std::string getPorgramInfoLog(GLuint program) {
	std::string info;
	GLint logLen;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
	info.reserve(logLen + 1);
	glGetProgramInfoLog(program, logLen, NULL, &info[0]);
	return info;
}

std::string getShaderInfoLog(GLuint shader) {
	std::string info;
	GLint logLen;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
	info.reserve(logLen + 1);
	glGetShaderInfoLog(shader, logLen, NULL, &info[0]);
	return info;
}

std::string loadFileContentAsString(const std::string& path) {
	auto pf = fopen(path.c_str(), "r");
	if (pf == NULL) {
		return "";
	}

	//std::strstream strio;
	std::string finalstr;
	char buff[65535];
	while (!feof(pf)) {
		memset(buff, 0, sizeof(buff));
		fgets(buff, sizeof(buff), pf);
		std::string str(buff);
		//strio << str;
		finalstr += str;
	}
	
	fclose(pf);
	//return strio.str();
	return finalstr;
}

//#include "list2_1.cpp"
//DECLARE_MAIN(my_application2_1);

//#include "list2_2.cpp"
//DECLARE_MAIN(my_application2_2);

//#include "list2_3.h"
//DECLARE_MAIN(my_application2_3); 
