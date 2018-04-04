
// Include the "sb7.h" header file
#include "sb7.h"





// Derive my_application from sb7::application
class my_application2_3 : public sb7::application
{
public:
	static GLuint compile_shaders(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint program;
		// Source code for vertex shader
		//static const GLchar * vertex_shader_source[] =
		//{
		//	"#version 450 core \n"
		//	" \n"
		//	"void main(void) \n"
		//	"{ \n"
		//	" gl_Position = vec4(0.0, 0.0, 0.5, 1.0); \n"
		//	"} \n"
		//};
		static const GLchar * vertex_shader_source[] = {
			"#version 450 core\n"
			"void main(void)\n"
			"{\n"
			"// Declare a hard-coded array of positions\n"
			"const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, 0.5, 1.0),\n"
			"vec4(-0.25, -0.25, 0.5, 1.0),\n"
			"vec4(0.25, 0.25, 0.5, 1.0));\n"
			"// Index into our array using gl_VertexID\n"
			"gl_Position = vertices[gl_VertexID];\n"
			"}\n"
		};
		

		// Source code for fragment shader
		static const GLchar * fragment_shader_source[] =
		{
			"#version 450 core \n"
			" \n"
			"out vec4 color; \n"
			" \n"
			"void main(void) \n"
			"{ \n"
			" color = vec4(0.0, 0.8, 1.0, 1.0); \n"
			"} \n"
		};
		// Create and compile vertex shader
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
		glCompileShader(vertex_shader);
		// Create and compile fragment shader
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
		glCompileShader(fragment_shader);
		// Create program, attach shaders to it, and link it
		program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);
		// Delete the shaders as the program has them now
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		return program;
	}

	void startup()
	{
		AllocConsole();
		freopen("conout$", "w", stdout);
		const GLubyte* name = glGetString(GL_VENDOR); //���ظ���ǰOpenGLʵ�ֳ��̵�����  
		const GLubyte* biaoshifu = glGetString(GL_RENDERER); //����һ����Ⱦ����ʶ����ͨ���Ǹ�Ӳ��ƽ̨  
		const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //���ص�ǰOpenGLʵ�ֵİ汾��  

		printf("OpenGLʵ�ֳ��̵����֣�%s\n", name);
		printf("��Ⱦ����ʶ����%s\n", biaoshifu);
		printf("OpenGLʵ�ֵİ汾�ţ�%s\n", OpenGLVersion);

		rendering_program = compile_shaders();
		glGenVertexArrays(1, &vertex_array_object);
		//glCreateVertexArrays(1, &vertex_array_object);//need 4.5
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
		
		// Draw one point
		//glPointSize(40.0f);
		//glDrawArrays(GL_POINTS, 0, 1);

		// Draw one triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
private:
	GLuint rendering_program;
	GLuint vertex_array_object;
};

DECLARE_MAIN(my_application2_3);