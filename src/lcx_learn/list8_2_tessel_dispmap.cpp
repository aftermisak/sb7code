
// Include the "sb7.h" header file
#include "sb7.h"
#include "common.h"
#include "vmath.h"
#include "sb7ktx.h"
#include <iostream>

// Derive my_application from sb7::application
class my_application8_2 : public sb7::application
{
public:


	static GLuint compile_shaders(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint tsctl_shader;
		GLuint tsevl_shader;
		GLuint program;

		std::string str_vs = loadFileContentAsString("glsls/list8_2_ts_dispmap_vs.glsl");
		static const GLchar * vs[] = {
			str_vs.c_str()
		};

		std::string str_tcs = loadFileContentAsString("glsls/list8_2_ts_dispmap_tcs.glsl");
		static const GLchar* tcs[] = {
			str_tcs.c_str()
		};
		std::string str_tes = loadFileContentAsString("glsls/list8_2_ts_dispmap_tes.glsl");
		static const GLchar* tes[] = {
			str_tes.c_str()
		};

		std::string str_fs = loadFileContentAsString("glsls/list8_2_ts_dispmap_fs.glsl");
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

		mvp_matrix_loc = glGetUniformLocation(rendering_program, "mvp_matrix");
		tex_disp_loc = glGetUniformLocation(rendering_program, "tex_displacement");


		glPatchParameteri(GL_PATCH_VERTICES, 4); CheckGLError();

		glActiveTexture(GL_TEXTURE0);
		tex_displacement = sb7::ktx::file::load("media/textures/terragen1.ktx");
		glActiveTexture(GL_TEXTURE1);
		tex_color = sb7::ktx::file::load("media/textures/terragen_color.ktx");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		float aspect = (float)info.windowWidth / (float)info.windowHeight;
		projMatrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
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

		const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
			(float)cos(currentTime) * 0.5f + 0.5f,
			0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 0.0f, 0);

		glUseProgram(rendering_program);
		auto mvpM = projMatrix * viewMatrix * vmath::mat4(1.0f); 
		glUniformMatrix4fv(mvp_matrix_loc, 1, GL_FALSE, mvpM);
		
		glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64); CheckGLError();
	}
private:
	GLuint tex_displacement;
	GLuint tex_color;

	GLint mvp_matrix_loc;
	GLint tex_disp_loc;

	vmath::mat4 viewMatrix;
	vmath::mat4 projMatrix;

	GLuint rendering_program;
};

DECLARE_MAIN(my_application8_2);