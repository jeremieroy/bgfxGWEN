$input a_position, a_color0, a_texcoord0
$output v_color0, v_texcoord0

#include "common.sh"

void main()
{

	vec2 pos =  a_position;
	//pos.x-=0.5;
	//pos.y-=0.5;
    gl_Position = mul(u_modelViewProj, vec4(pos, 0.0, 1.0) );    
    v_texcoord0 = a_texcoord0;
	v_color0 = a_color0;
}
