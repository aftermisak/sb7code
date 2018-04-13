#version 420 core

layout (vertices = 4) out;

in VS_OUT
{
    vec2 tc;
} tcs_in[];

out TCS_OUT
{
    vec2 tc;
} tcs_out[];

//gl_InvocationID : 每个patch有N个顶点gl_InvocationID->[0, N-1]
void main(void)
{
	if( gl_InvocationID == 0 ){
		gl_TessLevelInner[0] = 5.0f;
		gl_TessLevelInner[1] = 5.0f;
		gl_TessLevelOuter[0] = 5.0f;
		gl_TessLevelOuter[1] = 5.0f;
		gl_TessLevelOuter[2] = 5.0f;
		gl_TessLevelOuter[3] = 5.0f;
	}
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;
}