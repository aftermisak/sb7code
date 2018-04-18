
// Include the "sb7.h" header file

#include "sb7.h"
#include "sb7color.h"
#include "common.h"
#include "vmath.h"
#include "sb7ktx.h"
#include <iostream>

// Derive my_application from sb7::application
class my_application10_1 : public sb7::application
{
public:


	static GLuint compile_shaders(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint program;

		std::string str_vs = loadFileContentAsString("glsls/list10_1_vs.glsl");
		static const GLchar * vs[] = {
			str_vs.c_str()
		};

		std::string str_fs = loadFileContentAsString("glsls/list10_1_fs.glsl");
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

	GLuint compileComputeProgram() {
		GLuint compute_shader;
		GLuint program;
		std::string str_cs = loadFileContentAsString("glsls/list10_1_cs.glsl");
		static const GLchar * cs[] =
		{
			str_cs.c_str()
		};
		compute_shader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute_shader, 1, cs, NULL);
		glCompileShader(compute_shader);
		std::cout << getShaderInfoLog(compute_shader) << std::endl;
		CheckGLError();

		program = glCreateProgram();
		glAttachShader(program, compute_shader);
		glLinkProgram(program);
		std::cout << getPorgramInfoLog(program) << std::endl;
		// Delete the shaders as the program has them now
		glDeleteShader(compute_shader);
		CheckGLError();
		return program;
	}

	void init() override {
		sb7::application::init();

		info.samples = 4;
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

		compute_program = compileComputeProgram();


		rendering_program = compile_shaders();

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		const GLfloat pos[] = {
			0.0f, 0.0f, 0.0f,
			0.5f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);

		GLuint tex_input;
		glGenTextures(1, &tex_input);
		glBindTexture(GL_TEXTURE_2D, tex_input);
		glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA32F, 512, 512 );
		//glTextureStorage2D(tex_input, 1, GL_RGBA32F, 512, 512);
		glBindImageTexture(0, tex_input, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F); CheckGLError();
		glBindImageTexture(1, tex_input, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F); CheckGLError();

		float aspect = (float)info.windowWidth / (float)info.windowHeight;
		projMatrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);

		GLint integerV;
		glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &integerV);
		std::cout << "compute shader shared memory size:" << integerV/1024 << "kb" << std::endl;

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
		viewMatrix = vmath::lookat(vmath::vec3(0.0f, 10.0f, 10.0f), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(-1.0f, -1.0f, -1.0f));

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, 512, 512);
		glClearBufferfv(GL_COLOR, 0, sb7::color::Green);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glUseProgram(rendering_program);
		glBindVertexArray(vao);
		glVertexAttrib3fv(1, sb7::color::Purple);
		glDrawArrays(GL_TRIANGLES, 0, 3);


		glUseProgram(compute_program);
		glDispatchCompute(512 / 32, 512 / 32, 1);

	}
private:
	GLuint fbo;
	GLuint color_texture[3];
	GLuint depth_texture;

	GLuint vao;

	vmath::mat4 viewMatrix;
	vmath::mat4 projMatrix;

	GLuint rendering_program;
	GLuint compute_program;
};

DECLARE_MAIN(my_application10_1);