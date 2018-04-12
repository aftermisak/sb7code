
// Include the "sb7.h" header file
#include "sb7.h"
#include "common.h"
#include <iostream>

// Derive my_application from sb7::application
class my_application8_1 : public sb7::application
{
public:


	static GLuint compile_shaders(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint tsctl_shader;
		GLuint tsevl_shader;
		GLuint program;

		std::string str_vs = loadFileContentAsString("glsls/list8_1_ts_quad_vs.glsl");
		static const GLchar * vs[] = {
			str_vs.c_str()
		};

		std::string str_tcs = loadFileContentAsString("glsls/list8_1_ts_quad_tcs.glsl");
		static const GLchar* tcs[] = {
			str_tcs.c_str()
		};
		std::string str_tes = loadFileContentAsString("glsls/list8_1_ts_quad_tes.glsl");
		static const GLchar* tes[] = {
			str_tes.c_str()
		};

		std::string str_fs = loadFileContentAsString("glsls/list8_1_ts_quad_fs.glsl");
		static const GLchar * fs[] =
		{
			str_fs.c_str()
		};
		// Create and compile vertex shader
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, vs, NULL);
		glCompileShader(vertex_shader);
		std::cout << getShaderInfoLog(vertex_shader) << std::endl;
		CheckGLError();

		// create tessellation control shader
		tsctl_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tsctl_shader, 1, tcs, NULL);
		glCompileShader(tsctl_shader);
		std::cout << getShaderInfoLog(tsctl_shader) << std::endl;
		CheckGLError();

		//create tessellation evaluation shader
		tsevl_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(tsevl_shader, 1, tes, NULL);
		glCompileShader(tsevl_shader);
		std::cout << getShaderInfoLog(tsevl_shader) << std::endl;
		CheckGLError();

		// Create and compile fragment shader
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, fs, NULL);
		glCompileShader(fragment_shader);
		std::cout << getShaderInfoLog(fragment_shader) << std::endl;
		CheckGLError();

		// Create program, attach shaders to it, and link it
		program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, tsctl_shader); CheckGLError();
		glAttachShader(program, tsevl_shader); CheckGLError();
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);
		std::cout << getPorgramInfoLog(program) << std::endl;
		// Delete the shaders as the program has them now
		glDeleteShader(vertex_shader);
		glDeleteShader(tsctl_shader);
		glDeleteShader(tsevl_shader);
		glDeleteShader(fragment_shader);
		CheckGLError();
		return program;
	}

	void startup()
	{
		AllocConsole();
		freopen("conout$", "w", stdout);
		freopen("conout$", "w", stderr);
		freopen("conout$", "r", stdin);
		printGPUInfo();

		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback((GLDEBUGPROC)MessageCallback, NULL);
		rendering_program = compile_shaders();
		glGenVertexArrays(1, &vertex_array_object);
		glBindVertexArray(vertex_array_object);

	}
	void shutdown()
	{
		glDeleteVertexArrays(1, &vertex_array_object);
		glDeleteProgram(rendering_program);
		glDeleteVertexArrays(1, &vertex_array_object);
	}

	// Our rendering function
	void render(double currentTime)
	{
		const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
			(float)cos(currentTime) * 0.5f + 0.5f,
			0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, color);
		// Use the program object we created earlier for rendering
		glUseProgram(rendering_program);

		GLfloat  quad[] = {
			0.2f, 0.2f, 0.0f,
			0.8f, 0.2f, 0.0f,
			0.8f, 0.8f, 0.0f,
			0.2f, 0.8f, 0.0f
		};
		//vmath::vec3 quad[] = {
		//	vmath::vec3(0.2f, 0.2f, 0.0f),
		//	vmath::vec3(0.8f, 0.2f, 0.0f),
		//	vmath::vec3(0.8f, 0.8f, 0.0f),
		//	vmath::vec3(0.2f, 0.8f, 0.0f)
		//};
		glVertexAttrib4fv(0, quad);

		glPatchParameteri(GL_PATCH_VERTICES, 4); CheckGLError();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINES); CheckGLError();
		glDrawArrays(GL_PATCHES, 0, 12);
	}
private:
	GLuint rendering_program;
	GLuint vertex_array_object;
};

DECLARE_MAIN(my_application8_1);