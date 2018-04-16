#version 430 core


const int nn = 32;

void main(){
	int row = gl_InstanceID / nn*2;
	int column = gl_InstanceID % nn*2;
	vec4 position = vec4( row * 1.0f / nn - 1.0f , column * 1.0f / nn - 1.0f, 0.0f, 1.0f );
	gl_Position = position;
}