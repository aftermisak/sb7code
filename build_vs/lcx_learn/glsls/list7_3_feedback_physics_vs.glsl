#version 430 core

layout( location = 0 ) in vec4 position_weight;
layout( location = 1 ) in vec3 velocity;
layout( location = 2 ) in ivec4 connection;

layout( binding = 0 ) uniform samplerBuffer tex_position;

out vec4 tf_position_weight;
out vec3 tf_velocity;

// uniform mat4 view_matrix;
// uniform mat4 proj_matrix;
uniform float resting_len; //弹簧原长
uniform bool for_calculate;

const float spring_k = 7.1f;//弹性劲度系数 
const float spring_c = 2.8f;//阻尼系数
const vec3 global_G = vec3( 0.0f, -0.08f, 0.0f );//重力加速度
const float dt = 0.07;

void calculate_feedback(){
	vec3 old_pos = position_weight.xyz;
	float weight = position_weight.w;

	bool fixed_node = true;
	vec3 F = vec3( 0.0f );
	F += global_G * weight; //apply gravity
	F += velocity * -spring_c; //apply friction
	for( int i = 0; i < 4; ++i ){
		if(connection[i] != -1){
			vec3 pos = texelFetch( tex_position, connection[i] ).xyz;
			vec3 d = pos - old_pos;
			float len = length( d );
			F += spring_k * (len - resting_len) * normalize( d );//apply string 
			fixed_node = false;
		}
	}
	if( fixed_node ){
		F = vec3( 0.0f );
	}
	vec3 acc = F / weight;
	float t = dt;
	vec3 move = velocity*t + 0.5f * acc * t*t;
	
	move = clamp( move, vec3( -25.0f ), vec3( 25.0f ) );
	vec3 new_pos = old_pos + move;


	tf_position_weight = vec4( new_pos, weight );
	tf_velocity = velocity + acc*t;
}

void simply_draw(){
	gl_Position = vec4( position_weight.xyz, 1.0f );
	tf_position_weight = position_weight;
	tf_velocity = velocity;
}

void main(){
	if( for_calculate ){
		calculate_feedback();
	}else{
		simply_draw();
	}
}

/*
	对于弹性物体
	F = -kx
	k : 弹簧劲度系数；x:弹簧伸长长度

	F = dF
	d: 弹簧方向(标准化)

	摩擦消耗(阻尼)模拟
	Fd = -cv
	c : 阻尼系数

	重力 G
*/