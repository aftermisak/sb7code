#version 430 core

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec4 color;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float time;

out VS_OUT {
	vec4 color;
	flat float angle;
} vs_out;

void main(){
	vec3 pos = position;
	pos.z += time * 0.00001 ;
	gl_Position =  projMatrix * viewMatrix * vec4( pos, 1.0f );
	vs_out.color = vec4( 1.0f, 1.0f, 1.0f, 1.0f );

	gl_PointSize = 50.0 * abs( cos( time / 3 ) );
	vs_out.angle = time * 3;
	
}