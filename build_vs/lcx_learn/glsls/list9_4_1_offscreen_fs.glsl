#version 430 core

in VS_OUT {
	vec4 color;
} fs_in;

layout (location = 0) out vec4 color0;
layout (location = 1) out vec4 color1;
layout (location = 2) out vec4 color2;

void main(){
	color0 = fs_in.color;
	color1 = fs_in.color;
	color2 = fs_in.color;
}