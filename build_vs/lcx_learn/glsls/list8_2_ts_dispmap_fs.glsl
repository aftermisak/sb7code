#version 420 core

layout( binding = 1 ) uniform sampler2D tex_color;

in TES_OUT
{
    vec2 tc;
} fs_in;

out vec4 color;

void main(){
	color = texture( tex_color, fs_in.tc );
}