#version 430 core

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec4 color;

out VS_OUT {
	vec4 color;
	vec2 maybe_outside;
	centroid vec2 certainly_inside;
} vs_out;
void main(){
	gl_Position = vec4( position, 1.0f );
	vs_out.color = color;

	maybe_outside = vec2( 10.0f, 5.0f );
	certainly_inside = maybe_outside;
}