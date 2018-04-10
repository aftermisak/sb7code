
// Include the "sb7.h" header file
#include "sb7.h"
#include "vmath.h"
#include "sb7color.h"
#include <cassert>
#include "common.h"
#include <iostream>

// Derive my_application from sb7::application
class my_application5_uutg : public sb7::application
{
public:


	static GLuint compile_shaders(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint program;
		static const GLchar * vertex_shader_source[] = {
			"#version 430 core\n"
			"layout ( location = 0 ) in vec3 in_position;\n"
			//"in vec4 in_position;\n"
			"\n"
			"uniform mat4 mv_matrix;\n"
			"uniform mat4 proj_matrix;\n"
			"\n"
			"out vec4 vs_color;\n"
			"\n"
			"void main(){\n"
			"vec4 position = vec4( in_position, 1.0f );\n"
			//"vec4 position = in_position;\n"
			"gl_Position = proj_matrix * mv_matrix * position;\n"
			"vs_color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);\n"
			"}\n"
		};

		// Source code for fragment shader
		static const GLchar * fragment_shader_source[] =
		{
			"#version 430 core\n"
			"\n"
			"in vec4 vs_color;\n"
			"\n"
			"out vec4 color;\n"
			"\n"
			"void main() {\n"
			"color = vs_color;\n"
			"}\n"
		};
		// Create and compile vertex shader
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
		glCompileShader(vertex_shader); 
		std::cout << getShaderInfoLog(vertex_shader) << std::endl;
		CheckGLError();

		// Create and compile fragment shader
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
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

	

	void startup() override
	{
		AllocConsole();
		freopen("conout$", "w", stdout);
		freopen("conout$", "w", stderr);
		freopen("conout$", "r", stdin);
		printGPUInfo();

		//glEnable(GL_DEBUG_OUTPUT);
		//glDebugMessageCallback((GLDEBUGPROC)MessageCallback, NULL);
		rendering_program = compile_shaders();

		mv_location = glGetUniformLocation(rendering_program, "mv_matrix"); CheckGLError();
		proj_location = glGetUniformLocation(rendering_program, "proj_matrix"); CheckGLError();
		assert(mv_location != -1);
		assert(proj_location != -1);
		//glCreateVertexArrays(1, &vertex_array_object);//need 4.5
		glGenVertexArrays(1, &vertex_array_object);
		glBindVertexArray(vertex_array_object);

		static const vmath::vec3 vertex[] = {
			//front
			vmath::vec3(-0.25, 0.25, -0.25),
			vmath::vec3(-0.25, 0.25, -0.25) + vmath::vec3(0.5f, 0.0f, 0.0f),
			vmath::vec3(-0.25, 0.25, -0.25) + vmath::vec3(0.0f, 0.0f, 0.5f),
			vmath::vec3(-0.25, 0.25, -0.25) + vmath::vec3(0.5f, 0.0f, 0.5f),
			//left
			vmath::vec3(-0.25, 0.25, -0.25),
			vmath::vec3(-0.25, 0.25, -0.25) + vmath::vec3(0.0f, -0.5f, 0.0f),
			vmath::vec3(-0.25, 0.25, -0.25) + vmath::vec3(0.0f, 0.0f, 0.5f),
			vmath::vec3(-0.25, 0.25, -0.25) + vmath::vec3(0.0f, -0.5f, 0.5f),
			//bottom
			vmath::vec3(-0.25, 0.25, -0.25),
			vmath::vec3(-0.25, 0.25, -0.25) + vmath::vec3(0.5f, 0.0f, 0.0f),
			vmath::vec3(-0.25, 0.25, -0.25) + vmath::vec3(0.0f, -0.5f, 0.0f),
			vmath::vec3(-0.25, 0.25, -0.25) + vmath::vec3(0.5f, -0.5f, 0.0f),
			//back
			vmath::vec3(0.25f, -0.25f, -0.25f),
			vmath::vec3(0.25f, -0.25f, -0.25f) + vmath::vec3(-0.5f, 0.0f, 0.0f),
			vmath::vec3(0.25f, -0.25f, -0.25f) + vmath::vec3(0.0f, 0.0f, 0.5f),
			vmath::vec3(0.25f, -0.25f, -0.25f) + vmath::vec3(-0.5f, 0.0f, 0.5f),
			//right
			vmath::vec3(0.25f, -0.25f, -0.25f),
			vmath::vec3(0.25f, -0.25f, -0.25f) + vmath::vec3(0.0f, 0.5f, 0.0f),
			vmath::vec3(0.25f, -0.25f, -0.25f) + vmath::vec3(0.0f, 0.0f, 0.5f),
			vmath::vec3(0.25f, -0.25f, -0.25f) + vmath::vec3(0.0f, 0.5f, 0.5f),
			//top
			vmath::vec3(0.25f, -0.25f, 0.25f),
			vmath::vec3(0.25f, -0.25f, 0.25f) + vmath::vec3(-0.5f, 0.0f, 0.0f),
			vmath::vec3(0.25f, -0.25f, 0.25f) + vmath::vec3(0.0f, 0.5f, 0.0f),
			vmath::vec3(0.25f, -0.25f, 0.25f) + vmath::vec3(-0.5f, 0.5f, 0.0f),
		};

		static GLfloat vertex_positions[6*2*3*3];
		for (int i = 0, j = 0; i < 4 * 6; i += 4, j+=18) {
			//first triangle
			memcpy(vertex_positions + j , vertex + i, sizeof(GLfloat) * 9);

			//secondd triangle
			memcpy(vertex_positions + j+9 , vertex + i+1, sizeof(GLfloat) * 9);
		}
		
		GLuint arrayBuffer;
		//glCreateBuffers(1, &arrayBuffer);
		glGenBuffers(1, &arrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer);

		//glBufferStorage(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3, vertex_positions, 0);
		//CheckGLError();
		//glVertexArrayVertexBuffer(vertex_array_object, 0, arrayBuffer, 0, sizeof(GLfloat) * 3);
		//glVertexArrayAttribFormat(vertex_array_object, 0, 3, GL_FLOAT, GL_TRUE, 0);
		//glVertexArrayAttribBinding(vertex_array_object, 0, 0);
		//CheckGLError();

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
		glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		CheckGLError();

		glEnableVertexAttribArray(0u);

		float aspect = (float)info.windowWidth / (float)info.windowHeight;
		proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		CheckGLError();
	}
	void shutdown()
	{
		glDeleteVertexArrays(1, &vertex_array_object);
		glDeleteProgram(rendering_program);
		glDeleteVertexArrays(1, &vertex_array_object);
	}

	void onResize(int w, int h) {
		sb7::application::onResize(w, h);
		float aspect = (float)info.windowWidth / (float)info.windowHeight;
		proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
	}

	// Our rendering function
	void render(double currentTime)
	{

		
		glClearBufferfv(GL_COLOR, 0, sb7::color::Green);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

		glUseProgram(rendering_program);
		glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);
		for (int i = 0; i < 24; ++i) {
			float f = (float)i + (float)currentTime * 0.3f;
			vmath::mat4 mv_matrix =
				vmath::translate(0.0f, 0.0f, -20.0f) *
				vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
				vmath::rotate((float)currentTime * 21.0f, 1.0f, 0.0f, 0.0f) *
				vmath::translate(sinf(2.1f * f) * 2.0f,
					cosf(1.7f * f) * 2.0f,
					sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);
			glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//CheckGLError();
		

		
		
	}
private:
	GLuint rendering_program;
	GLuint vertex_array_object;
	vmath::mat4  proj_matrix;
	GLint mv_location;
	GLint proj_location;
};

//DECLARE_MAIN(my_application5_uutg);