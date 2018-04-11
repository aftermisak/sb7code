#version 430 core

layout( location = 0 ) vec4 position_weight;
layout( location = 1 ) vec3 velocity;
layout( location = 2 ) ivec4 connection;

uniform mat4 view_matrix;
uniform mat4 proj_matrix;


void main(){
	float dt = 0.1;
	vec3 old_pos = position_weight.xyz;
	float weight = position_weight.w;

	vec3 move = velocity * dt;
	vec3 new_pos = old_pos + move;

	gl_Position = view_matrix * proj_matrix * vec4( new_pos, 1.0f );

	gl_Position = vec4( position_weight.xyz, 1.0f );
}