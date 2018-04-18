
// Include the "sb7.h" header file

#include "sb7.h"
#include "sb7color.h"
#include "common.h"
#include "vmath.h"
#include "sb7ktx.h"
#include <iostream>

// Derive my_application from sb7::application
class my_application9_4_1 : public sb7::application
{
public:


	static GLuint compile_shaders(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint program;

		std::string str_vs = loadFileContentAsString("glsls/list9_4_1_offscreen_vs.glsl");
		static const GLchar * vs[] = {
			str_vs.c_str()
		};

		std::string str_fs = loadFileContentAsString("glsls/list9_4_1_offscreen_fs.glsl");
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

		info.samples = 8;
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

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		const auto ct_count = 3;
		glGenTextures(ct_count, color_texture);
		for (int i = 0; i < ct_count; ++i) {
			glBindTexture(GL_TEXTURE_2D, color_texture[i]);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		

		glGenTextures(1, &depth_texture);
		glBindTexture(GL_TEXTURE_2D, depth_texture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 512, 512);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture[0], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, color_texture[1], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, color_texture[2], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

		static const GLenum draw_buffers[] =
		{
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2
		};
		glDrawBuffers(3, draw_buffers);
		//glDrawBuffer(GL_COLOR_ATTACHMENT0);//appoint draw buffer for binding frame buffer.
		//glDrawBuffer(GL_COLOR_ATTACHMENT1);
		//glDrawBuffer(GL_COLOR_ATTACHMENT2);

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

		float aspect = (float)info.windowWidth / (float)info.windowHeight;
		projMatrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);


		GLenum fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			switch (fboStatus)
			{
			case GL_FRAMEBUFFER_UNDEFINED:
				// Oops, no window exists?
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				// Check the status of each attachment
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				// Attach at least one buffer to the FBO
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				// Check that all attachments enabled via
				// glDrawBuffers exist in FBO
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				// Check that the buffer specified via
				// glReadBuffer exists in FBO
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				// Reconsider formats used for attached buffers
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				// Make sure the number of samples for each
				// attachment is the same
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				// Make sure the number of layers for each
				// attachment is the same
				break;
			}
		}

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

		//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, 512, 512);
		glClearBufferfv(GL_COLOR, 0, sb7::color::Green);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glUseProgram(rendering_program);
		glBindVertexArray(vao);
		glVertexAttrib3fv(1, sb7::color::Purple);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
private:
	GLuint fbo;
	GLuint color_texture[3];
	GLuint depth_texture;

	GLuint vao;

	vmath::mat4 viewMatrix;
	vmath::mat4 projMatrix;

	GLuint rendering_program;
};

//DECLARE_MAIN(my_application9_4_1);