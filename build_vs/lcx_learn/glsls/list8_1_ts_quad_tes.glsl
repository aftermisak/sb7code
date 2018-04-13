#version 430 core
layout (quads) in;
void main(void)
{
	// Interpolate along bottom edge using x component of the
	// tessellation coordinate
	vec4 p1 = mix(gl_in[0].gl_Position,
	gl_in[1].gl_Position,
	gl_TessCoord.x);
	// Interpolate along top edge using x component of the
	// tessellation coordinate
	vec4 p2 = mix(gl_in[2].gl_Position,
	gl_in[3].gl_Position,
	gl_TessCoord.x);
	// Now interpolate those two results using the y component
	// of tessellation coordinate
	gl_Position = mix(p1, p2, gl_TessCoord.y);
}