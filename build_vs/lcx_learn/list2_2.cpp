// Include the "sb7.h" header file
#include "sb7.h"
// Derive my_application from sb7::application
class my_application2_2 : public sb7::application
{
public:
	// Our rendering function
	void render(double currentTime)
	{
		const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
			(float)cos(currentTime) * 0.5f + 0.5f,
			0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, color);
	}
};
