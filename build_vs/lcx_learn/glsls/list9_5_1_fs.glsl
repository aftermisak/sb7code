#version 430 core

in VS_OUT {
	vec4 color;
	vec2 maybe_outside;
	centroid vec2 certainly_inside;
} fs_in;


out vec4 fragColor;

void main(){
	fragColor = fs_in.color;

	bool maby_on_edge = any( notEqual( fs_in.maybe_outside, fs_in.certainly_inside ) );
	if( !maby_on_edge ){
		discard;
	}
}