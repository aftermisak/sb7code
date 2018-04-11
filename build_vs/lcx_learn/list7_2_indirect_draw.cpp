
// Include the "sb7.h" header file
#include "sb7.h"
#include "vmath.h"
#include "sb7color.h"
#include "object.h"
#include <cassert>
#include "common.h"
#include "common_nonsense.hpp"
#include <iostream>

namespace {

	typedef struct {
		GLuint vertexCount;
		GLuint instanceCount;
		GLuint firstVertex;
		GLuint baseInstance;
	} DrawArraysIndirectCommand;

	typedef struct {
		GLuint vertexCount;
		GLuint instanceCount;
		GLuint firstIndex;
		GLint baseVertex;
		GLuint baseInstance;
	} DrawElementsIndirectCommand;

	const int MODE_MULTIDRAW = 1;
	const int MODE_SEPARATE_DRAWS = 2;
	const int NUM_DRAWS = 30000;
}



class my_application7_2 : public sb7::application
{
public:


	static GLuint compile_shaders(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint program;

		static const GLchar * vertex_shader_source[] = {
			"#version 430 core\n"
			"layout (location = 0) in vec4 position;\n"
			"layout (location = 1) in vec3 normal;\n"
			"layout (location = 10) in uint draw_id;\n"
			"out VS_OUT\n"
			"{\n"
			"vec3 normal;\n"
			"vec4 color;\n"
			"} vs_out;\n"
			"uniform float time = 0.0;\n"
			"uniform mat4 view_matrix;\n"
			"uniform mat4 proj_matrix;\n"
			"uniform mat4 viewproj_matrix;\n"
			"const vec4 color0 = vec4(0.29, 0.21, 0.18, 1.0);\n"
			"const vec4 color1 = vec4(0.58, 0.55, 0.51, 1.0);\n"
			"void main(void)\n"
			"{\n"
			"mat4 m1;\n"
			"mat4 m2;\n"
			"mat4 m;\n"
			"float t = time * 0.1;\n"
			"float f = float(draw_id) / 30.0;\n"
			"float st = sin(t * 0.5 + f * 5.0);\n"
			"float ct = cos(t * 0.5 + f * 5.0);\n"
			"float j = fract(f);\n"
			"float d = cos(j * 3.14159);\n"
			"// Rotate around Y\n"
			"m[0] = vec4(ct, 0.0, st, 0.0);\n"
			"m[1] = vec4(0.0, 1.0, 0.0, 0.0);\n"
			"m[2] = vec4(-st, 0.0, ct, 0.0);\n"
			"m[3] = vec4(0.0, 0.0, 0.0, 1.0);\n"
			"// Translate in the XZ plane\n"
			"m1[0] = vec4(1.0, 0.0, 0.0, 0.0);\n"
			"m1[1] = vec4(0.0, 1.0, 0.0, 0.0);\n"
			"m1[2] = vec4(0.0, 0.0, 1.0, 0.0);\n"
			"m1[3] = vec4(260.0 + 30.0 * d, 5.0 * sin(f * 123.123), 0.0, 1.0);\n"
			"m = m * m1;\n"
			"// Rotate around X\n"
			"st = sin(t * 2.1 * (600.0 + f) * 0.01);\n"
			"ct = cos(t * 2.1 * (600.0 + f) * 0.01);\n"
			"m1[0] = vec4(ct, st, 0.0, 0.0);\n"
			"m1[1] = vec4(-st, ct, 0.0, 0.0);\n"
			"m1[2] = vec4(0.0, 0.0, 1.0, 0.0);\n"
			"m1[3] = vec4(0.0, 0.0, 0.0, 1.0);\n"
			"m = m * m1;\n"
			"// Rotate around Z\n"
			"st = sin(t * 1.7 * (700.0 + f) * 0.01);\n"
			"ct = cos(t * 1.7 * (700.0 + f) * 0.01);\n"
			"m1[0] = vec4(1.0, 0.0, 0.0, 0.0);\n"
			"m1[1] = vec4(0.0, ct, st, 0.0);\n"
			"m1[2] = vec4(0.0, -st, ct, 0.0);\n"
			"m1[3] = vec4(0.0, 0.0, 0.0, 1.0);\n"
			"m = m * m1;\n"
			"// Non-uniform scale\n"
			"float f1 = 0.65 + cos(f * 1.1) * 0.2;\n"
			"float f2 = 0.65 + cos(f * 1.1) * 0.2;\n"
			"float f3 = 0.65 + cos(f * 1.3) * 0.2;\n"
			"m1[0] = vec4(f1, 0.0, 0.0, 0.0);\n"
			"m1[1] = vec4(0.0, f2, 0.0, 0.0);\n"
			"m1[2] = vec4(0.0, 0.0, f3, 0.0);\n"
			"m1[3] = vec4(0.0, 0.0, 0.0, 1.0);\n"
			"m = m * m1;\n"
			"gl_Position = viewproj_matrix * m * position;\n"
			"vs_out.normal = mat3(view_matrix * m) * normal;\n"
			"vs_out.color = mix(color0, color1, fract(j * 313.431));\n"
			"}\n"
		};

		// Source code for fragment shader
		static const GLchar * fragment_shader_source[] =
		{
			"#version 430 core\n"
			"\n"
			"in VS_OUT {\n"
			"vec3 normal;\n"
			"vec4 color;\n"
			"} fs_in;\n"
			"out vec4 out_color;\n"
			"void main(){\n"
			"out_color = fs_in.color;\n"
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

		time_location = glGetUniformLocation(rendering_program, "time"); CheckGLError();
		mv_location = glGetUniformLocation(rendering_program, "view_matrix"); CheckGLError();
		//proj_location = glGetUniformLocation(rendering_program, "proj_matrix"); CheckGLError();
		mvproj_location = glGetUniformLocation(rendering_program, "viewproj_matrix"); CheckGLError();

		assert(mv_location != -1);
		//assert(proj_location != -1);
		assert(time_location != -1);
		assert(mvproj_location != -1);
		

		object.load("media/objects/asteroids.sbm");
		glBindVertexArray(object.get_vao());

		GLuint indirect_draw_buffer;
		glGenBuffers(1, &indirect_draw_buffer);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_draw_buffer);
		const size_t dib_size = NUM_DRAWS * sizeof(DrawArraysIndirectCommand);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, NUM_DRAWS * sizeof(DrawArraysIndirectCommand), NULL, GL_STATIC_DRAW);
		DrawArraysIndirectCommand* cmd = (DrawArraysIndirectCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0,
			dib_size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		for (int i = 0; i < NUM_DRAWS; ++i) {
			object.get_sub_object_info(i%object.get_sub_object_count(), cmd[i].firstVertex, cmd[i].vertexCount);
			cmd[i].instanceCount = 1;
			cmd[i].baseInstance = i;
		}
		glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
		
		GLuint draw_index_buffer;
		glGenBuffers(1, &draw_index_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, draw_index_buffer);
		const size_t ibsize = NUM_DRAWS * sizeof(GLuint);
		glBufferData(GL_ARRAY_BUFFER, ibsize, NULL, GL_STATIC_DRAW);
		GLuint* draw_index = (GLuint*)glMapBufferRange(GL_ARRAY_BUFFER, 0, ibsize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		/* GL_MAP_INVALIDATE_BUFFER_BIT: indicates that the previous contents of the entire buffer may be discarded. 
		Data within the entire buffer are undefined with the exception of subsequently written data. 
		*/
		for (int i = 0; i < NUM_DRAWS; ++i) { draw_index[i] = i; }
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glVertexAttribIPointer(10, 1, GL_UNSIGNED_INT, 0, NULL);
		/*
			 modify the rate at which generic vertex attributes advance during instanced rendering
			 用于 instanced rendering。
			 第一个参数为vertex attribute location
			 第二个参数为顶点数据使用频率，比如为10，表示instanced draw前10个用buffer中第一批数据，再19个用第二批数据
			以此类推
			
		*/
		glVertexAttribDivisor(10, 1);
		glEnableVertexAttribArray(10);


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
		glDeleteProgram(rendering_program);
	}

	void onResize(int w, int h) {
		sb7::application::onResize(w, h);
		float aspect = (float)info.windowWidth / (float)info.windowHeight;
		proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
	}

	// Our rendering function
	void render(double currentTime)
	{


		glClearBufferfv(GL_COLOR, 0, sb7::color::Black);
//#define equal_operation_1432t3490 1

#ifdef equal_operation_1432t3490
		float depv = 1.0f;
		//int stencilv = 0;
		glClearBufferfv(GL_DEPTH, 0, &depv);
		//glClearBufferiv(GL_STENCIL, 0, &stencilv);
#else 
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
#endif
		
		//vmath::mat4 mv_matrix = vmath::lookat(vmath::vec3(1.0f, 1.0f, 1.0f), vmath::vec3(0.5f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));

		vmath::mat4 mv_matrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 10.0f, 10.0f), vmath::vec3(0.0f, 1.0f, -1.0f));

		vmath::mat4 mvp_matrix = mv_matrix * proj_matrix;
		
		glUseProgram(rendering_program);
		//Specifies whether to transpose the matrix as the values are loaded into the uniform variable. Must be GL_FALSE.
		//glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);
		glUniformMatrix4fv(mvproj_location, 1, GL_FALSE, mvp_matrix);
		GLfloat time = GLfloat(currentTime); glUniform1fv(time_location, 1, &time);
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
		
		glBindVertexArray(object.get_vao());
		
		int mode = MODE_MULTIDRAW;
		//mode = MODE_SEPARATE_DRAWS;
		
		if (mode == MODE_MULTIDRAW) {
			glMultiDrawArraysIndirect(GL_TRIANGLES, NULL, NUM_DRAWS, 0);
		}
		else if (mode == MODE_SEPARATE_DRAWS) {
			for (int j = 0; j < NUM_DRAWS; ++j) {
				GLuint first, count;
				object.get_sub_object_info(j%object.get_sub_object_count(), first, count);
				glDrawArraysInstancedBaseInstance(GL_TRIANGLES, first, count, 1, j);
			}
		}

		//CheckGLError();




	}
private:
	sb7::object object;
	GLuint rendering_program;
	vmath::mat4  proj_matrix;
	GLint mv_location;
	GLint mvproj_location;
	GLint proj_location;
	GLint time_location;
};

//DECLARE_MAIN(my_application7_2);