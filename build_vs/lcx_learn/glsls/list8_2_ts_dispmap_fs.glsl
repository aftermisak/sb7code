#version 430 core

layout( binding = 1 ) uniform sampler2D tex_color;

in TES_OUT
{
    vec2 tc;
} fs_in;

out vec4 color;

void main(){
	color = texture( tex_color, fs_in.tc );
	// color = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
}