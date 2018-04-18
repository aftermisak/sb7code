
// Include the "sb7.h" header file

#include "sb7.h"
#include "sb7color.h"
#include "common.h"
#include "vmath.h"
#include "sb7ktx.h"
#include <iostream>

// Derive my_application from sb7::application
class my_application9_7_1 : public sb7::application
{
public:


	static GLuint compile_shaders(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint program;

		std::string str_vs = loadFileContentAsString("glsls/list9_7_1_vs.glsl");
		static const GLchar * vs[] = {
			str_vs.c_str()
		};

		std::string str_fs = loadFileContentAsString("glsls/list9_7_1_fs.glsl");
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

	void init() override {
		sb7::application::init();

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

		uniloc_vm = glGetUniformLocation(rendering_program, "viewMatrix"); CheckGLError();
		uniloc_pm = glGetUniformLocation(rendering_program, "projMatrix"); CheckGLError();
		uniloc_time = glGetUniformLocation(rendering_program, "time"); CheckGLError();
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		pointCount = 200;
		vmath::vec3* pos = new vmath::vec3 [pointCount];
		for (decltype(pointCount) i = 0; i < pointCount; ++i) {
			pos[i][0] = vmath::random<float>();
			pos[i][1] = vmath::random<float>();
			pos[i][2] = vmath::random<float>();
		}
		glBufferData(GL_ARRAY_BUFFER, sizeof(pos[0])*pointCount, pos, GL_STATIC_DRAW);
		delete[] pos;
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
		const GLfloat color[] = {
			1.0f, 1.0f, 1.0f, 1.0f,
		};
		GLuint vbo_color;
		glGenBuffers(1, &vbo_color);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

		
		glActiveTexture(GL_TEXTURE0);
		GLuint vbo_star = sb7::ktx::file::load("media/textures/star.ktx");

		glBindVertexArray(0);

		float aspect = (float)info.windowWidth / (float)info.windowHeight;
		projMatrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);

		glEnable(GL_PROGRAM_POINT_SIZE);
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
		static const float one = 1.0f;
		viewMatrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 1.0f), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));

		float f = (float)currentTime;


		glClearBufferfv(GL_COLOR, 0, sb7::color::Black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glUseProgram(rendering_program);
		glBindVertexArray(vao);
		glUniformMatrix4fv(uniloc_vm, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(uniloc_pm, 1, GL_FALSE, projMatrix);
		glUniform1fv(uniloc_time, 1, &f);
		glDrawArrays(GL_POINTS, 0, pointCount);

	}
private:
	GLuint vao;

	GLint uniloc_vm;
	GLint uniloc_pm;
	GLint uniloc_time;

	vmath::mat4 viewMatrix;
	vmath::mat4 projMatrix;

	GLuint rendering_program;

	size_t pointCount;
};

//DECLARE_MAIN(my_application9_7_1);