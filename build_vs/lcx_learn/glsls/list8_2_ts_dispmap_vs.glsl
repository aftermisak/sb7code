#version 420 core

out VS_OUT
{
    vec2 tc;
} vs_out;

//gl_InstanceID: instance draw当前instance的ID
//gl_VertexID : 当前instance的顶点ID
void main(){
	vec4 points[] = {
		vec4( 0.0f, 0.0f, -0.5f, 1.0f ),
		vec4( 0.5f, 0.0f, -0.5f, 1.0f ),
		vec4( 0.5f, 0.0f, 0.0f, 1.0f ),
		vec4( 0.0f, 0.0f, 0.0f, 1.0f )
	};
	vec2 tex_coords[] = {
		vec2( 0.0f, 1.0f ),
		vec2( 1.0f, 1.0f ),
		vec2( 1.0f, 0.0f ),
		vec2( 0.0f, 0.0f )
	};
	int row = gl_InstanceID / 64; //gl_InstanceID >> 6;
	int column = gl_InstanceID % 64; //gl_InstanceID & 63;
	
	vec4 op = points[gl_VertexID];

	gl_Position = vec4( op.x + 0.5 * row, op.y, op.z + 0.5*column, op.w );
	vs_out.tc = tex_coords[gl_VertexID];
}