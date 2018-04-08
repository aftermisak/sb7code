
// Include the "sb7.h" header file
#include "sb7.h"
#include "common.h"
#include <iostream>

// Derive my_application from sb7::application
class my_application3_3 : public sb7::application
{
public:


	static GLuint compile_shaders(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint tsctl_shader;
		GLuint tsevl_shader;
		GLuint geometry_shader;
		GLuint program;
		static const GLchar * vertex_shader_source[] = {
			"#version 450 core\n"
			" \n"
			"layout (location = 0) in vec4 offset;\n"
			"layout (location = 1) in vec4 color;\n"
			"out VS_OUT{ vec4 color; } vs_out; \n"
			"void main(void)\n"
			"{\n"
			"// Declare a hard-coded array of positions\n"
			"const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, 0.5, 1.0),\n"
			"vec4(-0.25, -0.25, 0.5, 1.0),\n"
			"vec4(0.25, 0.25, 0.5, 1.0));\n"
			"// Index into our array using gl_VertexID\n"
			"gl_Position = vertices[gl_VertexID] + offset ;\n"
			"vs_out.color = color; \n"
			"}\n"
		};
		static GLchar* tsctl_shader_source[] = {
			"#version 450 core\n"
			"layout (vertices = 3) out;\n"
			"void main(void)\n"
			"{\n"
			"// Only if I am invocation 0 ...\n"
			"if (gl_InvocationID == 0)\n"
			"{\n"
			"gl_TessLevelInner[0] = 5.0;\n"
			"gl_TessLevelOuter[0] = 5.0;\n"
			"gl_TessLevelOuter[1] = 5.0;\n"
			"gl_TessLevelOuter[2] = 5.0;\n"
			"}\n"
			"// Everybody copies their input to their output\n"
			"gl_out[gl_InvocationID].gl_Position =\n"
			"gl_in[gl_InvocationID].gl_Position;\n"
			"}\n"
		};
		static GLchar* tsevl_shader_source[] = {
			"#version 450 core\n"
			"layout (triangles, equal_spacing, cw) in;\n"
			"void main(void)\n"
			"{\n"
			"gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position +\n"
			"gl_TessCoord.y * gl_in[1].gl_Position +\n"
			"gl_TessCoord.z * gl_in[2].gl_Position);\n"
			"}\n"
		};
		static GLchar* geometry_shader_source[] = {
			"#version 450 core\n"
			"layout (triangles) in;\n"
			"layout (points, max_vertices = 3) out;\n"
			"void main(void)\n"
			"{\n"
			"int i;\n"
			"for (i = 0; i < gl_in.length(); i++)\n"
			"{\n"
			"gl_Position = gl_in[i].gl_Position;\n"
			"EmitVertex();\n"
			"}\n"
			"}\n"
		};

		// Source code for fragment shader
		static const GLchar * fragment_shader_source[] =
		{
			"#version 450 core \n"
			" \n"
			"in VS_OUT { vec4 color; } fs_in; \n"
			"out vec4 color; \n"
			" \n"
			"void main(void) \n"
			"{ \n"
			" color = fs_in.color; \n"
			"} \n"
		};
		// Create and compile vertex shader
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
		glCompileShader(vertex_shader);

		// create tessellation control shader
		tsctl_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tsctl_shader, 1, tsctl_shader_source, NULL);
		glCompileShader(tsctl_shader); CheckGLError();
		//create tessellation evaluation shader
		tsevl_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(tsevl_shader, 1, tsevl_shader_source, NULL);
		glCompileShader(tsevl_shader); CheckGLError();
		// Create and compile fragment shader

		//create geometry shader
		geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry_shader, 1, geometry_shader_source, NULL);
		glCompileShader(geometry_shader); CheckGLError();

		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
		glCompileShader(fragment_shader);
		// Create program, attach shaders to it, and link it
		program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		//glAttachShader(program, tsctl_shader);
		//glAttachShader(program, tsevl_shader); 
		glAttachShader(program, geometry_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);
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
		//glDebugMessageCallback((GLDEBUGPROC)MessageCallback, NULL);
		rendering_program = compile_shaders();
		glCreateVertexArrays(1, &vertex_array_object);//need 4.5
		glBindVertexArray(vertex_array_object);

	}
	void shutdown()
	{
		glDeleteVertexArrays(1, &vertex_array_object);
		glDeleteProgram(rendering_program);
		glDeleteVertexArrays(1, &vertex_array_object);
	}

	// Our rendering function
	void render(double currentTime)
	{
		const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
			(float)cos(currentTime) * 0.5f + 0.5f,
			0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, color);
		// Use the program object we created earlier for rendering
		glUseProgram(rendering_program);

		GLfloat attrib[] = { (float)sin(currentTime) * 0.5f,
			(float)cos(currentTime) * 0.6f,
			0.0f, 0.0f };
		GLfloat attrib_color[] = {
			1.0f - color[0],  1.0f - color[1], 1.0f - color[2], 1.0
		};
		glVertexAttrib4fv(0, attrib);
		glVertexAttrib4fv(1, attrib_color);
		glPointSize(5.0f);
		//glPatchParameteri(GL_PATCH_VERTICES, 3); CheckGLError();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINES); CheckGLError();
		// Draw one triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
private:
	GLuint rendering_program;
	GLuint vertex_array_object;
};

//DECLARE_MAIN(my_application3_3);