//[VS]
#version 450 core
layout ( location = 0 ) in vec3 position;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec4 vs_color;

void main(){
	gl_Position = proj_matrix * mv_matrix * vec4( position, 1.0f );

	const vec4 colors[] = {
		vec4( 1.0f, 0.0f, 0.0f, 1.0f ),
		vec4( 0.0f, 1.0f, 0.0f, 1.0f ),
		vec4( 0.0f, 0.0f, 1.0f, 1.0f ),
	};
	
	vs_color = colors[glVertexId%3];
}


//[FS]
#version 450 core

in vec4 vs_color;

out vec4 color;

void main() {
	color = vs_color;
}