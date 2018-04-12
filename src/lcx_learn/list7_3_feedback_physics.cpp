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
		//vm_location = glGetUniformLocation(program, "view_matrix");assert(vm_location != -1);
		//pm_location = glGetUniformLocation(program, "proj_matrix");assert(pm_location != -1);
		rest_len_location = glGetUniformLocation(program, "resting_len"); assert(rest_len_location != -1);
		for_calculate_location = glGetUniformLocation(program, "for_calculate"); assert(for_calculate_location != -1);

		static const char* feed_back_varyings[] = {
			"tf_position_weight", "tf_velocity"
		};
		glTransformFeedbackVaryings(program, sizeof( feed_back_varyings )/sizeof(feed_back_varyings[0]), 
			feed_back_varyings, GL_SEPARATE_ATTRIBS); CheckGLError();
		glLinkProgram(program); CheckGLError();

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
		vmath::vec3 p1 = vmath::vec3(initial_position[0][0], initial_position[0][1], initial_position[0][2]);
		vmath::vec3 p2 = vmath::vec3(initial_position[1][0], initial_position[1][1], initial_position[1][2]);
		rest_len = vmath::length(p1 - p2);
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

		glGenTextures(2, posTexture);
		glBindTexture(GL_TEXTURE_BUFFER, posTexture[0]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbo[VBO_P1_A]);
		glBindTexture(GL_TEXTURE_BUFFER, posTexture[1]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vbo[VBO_P2_A]);

		//glGenBuffers(1, &fbBuffer);
		//glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, fbBuffer, 0, sizeof(vmath::vec4)*POINTS_TOTAL);
		//glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(vmath::vec4)*POINTS_TOTAL, NULL, GL_DYNAMIC_COPY);
		//glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 1, fbBuffer, sizeof(vmath::vec4)*POINTS_TOTAL, sizeof(vmath::vec3)*POINTS_TOTAL);
		//glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(vmath::vec3)*POINTS_TOTAL, NULL, GL_DYNAMIC_COPY);

		CheckGLError();

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
		
		glPointSize(3.0);

		glUseProgram(program);
		glUniform1f(rest_len_location, rest_len);
		{
			glEnable(GL_RASTERIZER_DISCARD);//disable rasterizer
			glBindVertexArray(vao[0]);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[VBO_P2_A]);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, vbo[VBO_V2_A]);
			glBeginTransformFeedback(GL_POINTS);
			glUniform1i(for_calculate_location, 0);
			glDrawArrays(GL_POINTS, 0, POINTS_TOTAL);
			glEndTransformFeedback();
		}
		{
			glDisable(GL_RASTERIZER_DISCARD);
			glBindVertexArray(vao[1]);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[VBO_P1_A]);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, vbo[VBO_V1_A]);
			glBeginTransformFeedback(GL_POINTS);
			//glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, fbBuffer);
			//glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_P2_A]);
			//glCopyBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, GL_ARRAY_BUFFER, 0, 0, sizeof(vmath::vec4)*POINTS_TOTAL);
			//glCopyNamedBufferSubData(fbBuffer, vbo[VBO_P2_A], 0, 0, sizeof(vmath::vec4)*POINTS_TOTAL);
			//glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_V2_A]);
			//glCopyBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, GL_ARRAY_BUFFER, sizeof(vmath::vec4)*POINTS_TOTAL, 0, sizeof(vmath::vec3)*POINTS_TOTAL);
			//glCopyNamedBufferSubData(fbBuffer, vbo[VBO_V2_A], sizeof(vmath::vec4)*POINTS_TOTAL, 0, sizeof(vmath::vec3)*POINTS_TOTAL);
			glUniform1i(for_calculate_location, 1);
			glDrawArrays(GL_POINTS, 0, POINTS_TOTAL);
			glEndTransformFeedback();
		}
		
		
	}
	
private:
	GLuint program;
	GLuint vao[2];
	GLuint vbo[5];
	GLuint posTexture[2];
	//GLuint fbBuffer;

	GLint vm_location;
	GLint pm_location;
	GLint rest_len_location;
	GLint for_calculate_location;
	vmath::mat4 view_matrix;
	vmath::mat4 proj_matrix;
	float rest_len;

	constexpr static int VBO_P1_A = 0;
	constexpr static int VBO_P2_A = 1;
	constexpr static int VBO_V1_A = 2;
	constexpr static int VBO_V2_A = 3;
	constexpr static int VBO_CONN = 4;
};

//DECLARE_MAIN(my_application7_3);
