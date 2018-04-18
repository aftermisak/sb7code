#version 430 core

in VS_OUT {
	vec4 color;
	flat float angle;
} fs_in;

layout( binding = 0 ) uniform sampler2D starImage;

out vec4 fragColor;
void main(){
	const float sinv = sin( fs_in.angle );
	const float cosv = cos( fs_in.angle );
	const mat2 rotation = mat2( 
		cosv, sinv, 
		-sinv, cosv);

	const vec2 pt =gl_PointCoord - vec2( 0.5f );

	// vec2 p = gl_PointCoord * 2.0 - vec2(1.0);
	// if (dot(p, p) > sin(atan(p.y, p.x) * 5.0)){
 //        discard;   
	// }
	
	fragColor = texture( starImage, rotation * pt + vec2( 0.5f )  ) * fs_in.color;
	
}