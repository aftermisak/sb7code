#version 420 core

layout (quads, fractional_odd_spacing) in;

uniform sampler2D tex_displacement;

uniform mat4 mvp_matrix;


in TCS_OUT
{
    vec2 tc;
} tes_in[];

out TES_OUT
{
    vec2 tc;
} tes_out;

//gl_TessCoord  //https://www.khronos.org/opengl/wiki/Tessellation_Evaluation_Shader/Defined_Inputs
void main(void)
{
	
	vec4 mix_up = mix( gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x );
	vec4 mix_down = mix( gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x );
	vec4 mix_pos = mix( mix_up, mix_down, gl_TessCoord.y );
	

	vec2 tc_mix_up = mix( tes_in[0].tc, tes_in[1].tc, gl_TessCoord.x );
	vec2 tc_mix_down = mix( tes_in[2].tc, tes_in[3].tc, gl_TessCoord.x );
	vec2 tc_mix = mix( tc_mix_up, tc_mix_down, gl_TessCoord.y );
	

	mix_pos = mix_pos + vec4( 0.0f, texture( tex_displacement, tc_mix ).r, 0.0f, 0.0f );
	mix_pos = mvp_matrix * mix_pos;
	mix_pos = mix_pos / mix_pos.w;

	gl_Position = mix_pos;
	tes_out.tc = tc_mix;

}

/*
	equal_spacing
	fractional_even_spacing
	fractional_odd_spacing
*/