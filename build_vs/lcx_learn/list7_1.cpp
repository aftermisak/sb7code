
// Include the "sb7.h" header file
#include "sb7.h"
#include "vmath.h"
#include "sb7color.h"
#include <cassert>
#include "common.h"
#include "common_nonsense.hpp"
#include <iostream>

namespace {
	typedef struct VERTEX_t
	{
		vmath::vec4 position;
		vmath::vec3 normal;
		vmath::vec2 tex_coord;
		GLubyte color[3];
		int material_id;
	} VERTEX;
}



class my_application7_1 : public sb7::application
{
public:


	static GLuint compile_shaders(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint program;
		//static const GLchar * vertex_shader_source[] = {
		//	"#version 430 core\n"
		//	"// Declare a number of vertex attributes\n"
		//	"layout (location = 0) in vec4 position;\n"
		//	"layout (location = 1) in vec3 normal;\n"
		//	"layout (location = 2) in vec2 tex_coord;\n"
		//	"// Note that we intentionally skip location 3 here\n"
		//	"layout (location = 4) in vec4 color;\n"
		//	"layout (location = 5) in int material_id;\n"

		//	"void main(){\n"
		//	"gl_Position = position;\n"
		//	"}\n"
		//};

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

		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback((GLDEBUGPROC)MessageCallback, NULL);
		rendering_program = compile_shaders();

		mv_location = glGetUniformLocation(rendering_program, "mv_matrix"); CheckGLError();
		proj_location = glGetUniformLocation(rendering_program, "proj_matrix"); CheckGLError();
		assert(mv_location != -1);
		assert(proj_location != -1);
		//glCreateVertexArrays(1, &vertex_array_object);//need 4.5
		glGenVertexArrays(1, &vertex_array_object);
		glBindVertexArray(vertex_array_object);


		static const GLfloat vertex_positions[] =
		{
			-0.25f, -0.25f, -0.25f,
			-0.25f, 0.25f, -0.25f,
			0.25f, -0.25f, -0.25f,
			0.25f, 0.25f, -0.25f,
			0.25f, -0.25f, 0.25f,
			0.25f, 0.25f, 0.25f,
			-0.25f, -0.25f, 0.25f,
			-0.25f, 0.25f, 0.25f,
		};
		static const GLushort vertex_indices[] =
		{
			0, 1, 2,
			2, 1, 3,
			2, 3, 4,
			4, 3, 5,
			4, 5, 6,
			6, 5, 7,
			6, 7, 0,
			0, 7, 1,
			6, 0, 2,
			2, 4, 6,
			7, 5, 3,
			7, 3, 1
		};
	

		GLuint vertexbuffer, indecebuffer ;

		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer); CheckGLError();
		if ( false ) {
			glBindVertexBuffer( bindingindex_gp 0, vertexbuffer, 0, sizeof( GLfloat ) * 3 );//bind vertexbuffer with a name(index)
			glVertexAttribFormat( attribindex_gp 0, 3, GL_FLOAT, GL_FALSE, 0);//create a vertex attribute with a name(index)
			glVertexAttribBinding( attribindex_gp 0, bindingindex_gp 0);//bind vertexbuffer with a created vertex attribute
		}
		else {
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, NULL);
		}
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW); CheckGLError();
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &indecebuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indecebuffer); CheckGLError();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW); CheckGLError();


		/* eg.
		VERTEX vertex;
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), &vertex, GL_STATIC_DRAW);
		glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, offsetof(VERTEX,position));
		glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, offsetof(VERTEX,normal));
		glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(VERTEX, tex_coord));
		glVertexAttribFormat(4, 3, GL_UNSIGNED_BYTE, GL_TRUE, offsetof(VERTEX, color));
		glVertexAttribIFormat(5, 1, GL_INT, offsetof(VERTEX, material_id));
		glVertexAttribBinding(0, 0); // position
		glVertexAttribBinding(1, 0); // normal
		glVertexAttribBinding(2, 0); // tex_coord
		glVertexAttribBinding(4, 0); // color
		glVertexAttribBinding(5, 0); // material_id
		glBindVertexBuffer(0, arrayBuffers[0], 0, sizeof(VERTEX));
		*/
		

		float aspect = (float)info.windowWidth / (float)info.windowHeight;
		proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
		bool cull = true;
		if (cull) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
		bool ztest = true;
		if (ztest) {
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
		
		
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
#define equal_operation_1432t3490 1

#ifdef equal_operation_1432t3490
		float depv = 1.0f;
		//int stencilv = 0;
		glClearBufferfv(GL_DEPTH, 0, &depv);
		//glClearBufferiv(GL_STENCIL, 0, &stencilv);
#else 
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
#endif
		
		

		glUseProgram(rendering_program);
		glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);
		const auto drawcount = 12400;
		for (int i = 0; i < drawcount; ++i) {
			float f = (float)i + (float)currentTime * 0.3f;
			vmath::mat4 mv_matrix =
				vmath::translate(0.0f, 0.0f, -20.0f) *
				vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
				vmath::rotate((float)currentTime * 21.0f, 1.0f, 0.0f, 0.0f) *
				vmath::translate(sinf(2.1f * f) * 2.0f,
					cosf(1.7f * f) * 2.0f,
					sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);
			glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
			
			//glDrawElementsInstanced( GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0, drawcount );
			//glDrawElementsInstancedBaseVertex(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0, drawcount, 0);
			//glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0, drawcount, 0, 0);
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

DECLARE_MAIN(my_application7_1);