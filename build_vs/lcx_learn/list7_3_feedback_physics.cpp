#include "sb7.h"
#include "vmath.h"
#include "sb7color.h"
#include "object.h"
#include <cassert>
#include <vector>
#include "common.h"
#include <iostream>

namespace {
	constexpr const auto POINTS_X = 20;
	constexpr const auto POINTS_Y = 20;
	constexpr const auto POINTS_TOTAL = POINTS_X*POINTS_Y;
}

class my_application7_3 : public sb7::application {
private:
	static GLuint compile_shaders(void) {
		GLuint program = glCreateProgram();
		std::string vs_res = loadFileContentAsString("glsls/list7_3_feedback_physics_vs.glsl");
		std::string fs_res = loadFileContentAsString("glsls/list7_3_feedback_physics_fs.glsl");

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		{
			const char* vs_ress[] = {
				vs_res.c_str()
			};
			glShaderSource(vs, 1, vs_ress, NULL);
			glCompileShader(vs);
			std::cout << getShaderInfoLog(vs) << std::endl;
			CheckGLError();
		}
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		{
			const char* fs_ress[] = {
				fs_res.c_str()
			};
			glShaderSource(fs, 1, fs_ress, NULL);
			glCompileShader(fs);
			std::cout << getShaderInfoLog(fs) << std::endl;
			CheckGLError();
		}
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		std::cout << getPorgramInfoLog(program) << std::endl;
		glDeleteShader(vs);
		glDeleteShader(fs);
		CheckGLError();
		return program;
	}
	void updateProjMatrix() {
		float aspect = (float)info.windowWidth / (float)info.windowHeight;
		proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
	}
public:

	void startup() override {
		AllocConsole();
		freopen("conout$", "w", stdout);
		freopen("conout$", "w", stderr);
		freopen("conout$", "r", stdin);
		printGPUInfo();

		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback((GLDEBUGPROC)MessageCallback, NULL);

		program = this->compile_shaders();
		vm_location = glGetUniformLocation(program, "view_matrix");
		pm_location = glGetUniformLocation(program, "proj_matrix");
		assert(vm_location != -1);
		assert(pm_location != -1);

		vmath::vec4* initial_position = new vmath::vec4[POINTS_TOTAL];//{position.xyz, weight}
		vmath::vec3* initial_velocitys = new vmath::vec3[POINTS_TOTAL];
		vmath::ivec4* connection_vectors = new vmath::ivec4[POINTS_TOTAL];
		for (int i = 0; i < POINTS_X; ++i) {
			for (int j = 0; j < POINTS_Y; ++j) {
				int index = i * POINTS_X + j;
				initial_position[index] = vmath::vec4(0.8f / POINTS_X*i, 0.8f / POINTS_Y*j, 0.0f, 1.0f);

				initial_velocitys[index] = vmath::vec3(0.0f);

				connection_vectors[index][0] = index - POINTS_X >= 0 ? index - POINTS_X : -1 ; //up
				connection_vectors[index][1] = j < POINTS_Y - 1 ? index + 1 : -1;//right
				connection_vectors[index][2] = index + POINTS_X < POINTS_TOTAL-1 ? index+POINTS_X:-1 ; //down
				connection_vectors[index][3] = j != 0 ? index-1 : -1; //left
			}
		}
		//auto a = std::vector<vmath::vec4>(initial_position, initial_position + POINTS_TOTAL);
		//auto b = std::vector<vmath::vec3>(initial_velocitys, initial_velocitys + POINTS_TOTAL);
		//auto c = std::vector<vmath::ivec4>(connection_vectors, connection_vectors + POINTS_TOTAL);
		glGenVertexArrays(2, vao);
		glGenBuffers(5, vbo);

		
		for (int i = 0; i < 2; ++i) {
			glBindVertexArray(vao[i]);

			glBindBuffer(GL_ARRAY_BUFFER, i == 0 ? vbo[VBO_P1_A]: vbo[VBO_P2_A]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(initial_position[0])*POINTS_TOTAL, initial_position, GL_DYNAMIC_COPY);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(0);
			CheckGLError();

			glBindBuffer(GL_ARRAY_BUFFER, i == 0 ? vbo[VBO_V1_A] : vbo[VBO_V2_A]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(initial_velocitys[0])*POINTS_TOTAL, initial_velocitys, GL_DYNAMIC_COPY);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(1);
			CheckGLError();

			glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_CONN]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(connection_vectors[0])*POINTS_TOTAL, connection_vectors, GL_DYNAMIC_COPY);
			glVertexAttribIPointer(2, 4, GL_INT, 0, NULL);
			glEnableVertexAttribArray(2);
			CheckGLError();
		}
		delete[] initial_position;
		delete[] initial_velocitys;
		delete[] connection_vectors;

		updateProjMatrix();
	}
	void shutdown()override{
		glDeleteProgram(program);
		glDeleteVertexArrays(2, vao);
	}
	void onResize(int w, int h) {
		sb7::application::onResize(w, h);
		updateProjMatrix();
	}

	void render(double currentTime)override {

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

		view_matrix = vmath::lookat(vmath::vec3(0.0f), vmath::vec3(0.0f, 0.0f, 1.0f), vmath::vec3( 0.0f, 1.0f, 0.0f ));
		
		glPointSize(10.0);

		glUseProgram(program);

		glUniformMatrix4fv(vm_location, 1, GL_FALSE, view_matrix);
		glUniformMatrix4fv(pm_location, 1, GL_FALSE, proj_matrix);

		glBindVertexArray(vao[0]);
		
		glDrawArrays(GL_POINTS, 0, POINTS_TOTAL);
		//glDrawArrays(GL_TRIANGLES, 0, POINTS_TOTAL);
	}
	
private:
	GLuint program;
	GLuint vao[2];
	GLuint vbo[5];

	GLint vm_location;
	GLint pm_location;
	vmath::mat4 view_matrix;
	vmath::mat4 proj_matrix;

	constexpr static int VBO_P1_A = 0;
	constexpr static int VBO_P2_A = 1;
	constexpr static int VBO_V1_A = 2;
	constexpr static int VBO_V2_A = 3;
	constexpr static int VBO_CONN = 4;
};

DECLARE_MAIN(my_application7_3);
