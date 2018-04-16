
// Include the "sb7.h" header file

#include "sb7.h"


#include "common.h"
#include "vmath.h"
#include "sb7ktx.h"
#include <iostream>

// Derive my_application from sb7::application
class my_application9_2 : public sb7::application
{
public:


	static GLuint compile_shaders(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint program;

		std::string str_vs = loadFileContentAsString("glsls/list9_2_stencil_test_vs.glsl");
		static const GLchar * vs[] = {
			str_vs.c_str()
		};

		std::string str_fs = loadFileContentAsString("glsls/list9_2_stencil_test_fs.glsl");
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

		// Create and compile fragment shader
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, fs, NULL);
		glCompileShader(fragment_shader);
		std::cout << getShaderInfoLog(fragment_shader) << std::endl;
		CheckGLError();

		// Create program, attach shaders to it, and link it
		program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);
		std::cout << getPorgramInfoLog(program) << std::endl;
		// Delete the shaders as the program has them now
		glDeleteShader(vertex_shader);
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

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		//GLuint vbo;
		//glGenBuffers(1, &vbo);
		//glBindBuffer(GL_ARRAY_BUFFER, vbo);
		//const GLfloat pos[] = {
		//	0.0f, 0.0f, 0.0f,
		//	0.5f, 0.0f, 0.0f,
		//	0.0f, 0.5f, 0.0f,
		//};
		//glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		//glEnableVertexAttribArray(0);
		glBindVertexArray(0);

		float aspect = (float)info.windowWidth / (float)info.windowHeight;
		projMatrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);


		glEnable(GL_STENCIL_TEST);

		GLint stencilBits = 0;

		std::cout << "stencil buffer bit : " << stencilBits << std::endl;
		
		glClearStencil(1);
		//glStencilFunc(GL_NEVER, 0, 0x0000u);
		//glStencilFunc(GL_ALWAYS, 0, 0x0000u);
		glStencilFunc(GL_EQUAL, 0x0001, 0x00ffu);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		CheckGLError();
		
	}
	void shutdown()
	{
		glDeleteProgram(rendering_program);
	}

	void onResize(int w, int h) {
		sb7::application::onResize(w, h);
		float aspect = (float)info.windowWidth / (float)info.windowHeight;
		projMatrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
	}

	// Our rendering function
	void render(double currentTime)
	{
		viewMatrix = vmath::lookat(vmath::vec3(0.0f, 10.0f, 10.0f), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(-1.0f, -1.0f, -1.0f));

		const GLfloat color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, color);

		glPointSize(3.0f);
		glUseProgram(rendering_program);
		glBindVertexArray(vao);
		const static auto ins_count = 1000;
		for (int insi = 0; insi < ins_count; ++insi) {
			const GLfloat pos[] = {
				vmath::random<float>(),
				vmath::random<float>(),
				0.0f,
			};
			const GLfloat color[] = {
				vmath::random<float>(),
				vmath::random<float>(),
				vmath::random<float>(),
				vmath::random<float>(),
			};
			glVertexAttrib3fv(0, pos);
			glVertexAttrib3fv(1, color);
			glDrawArraysInstanced(GL_POINTS, 0, 1, ins_count);
		}
		
	}
private:
	GLuint vao;

	vmath::mat4 viewMatrix;
	vmath::mat4 projMatrix;

	GLuint rendering_program;
};

DECLARE_MAIN(my_application9_2);