$input a_position, a_color0, a_texcoord0, a_shift, a_gamma
$output v_color0, v_texcoord0, v_shift, v_gamma

#include "common.sh"

void main()
{
    v_shift = a_shift;
    v_gamma = a_gamma;
    v_color0 = a_color0;
    v_texcoord0 =  a_texcoord0;
    gl_Position = mul(u_modelViewProj, vec4(a_position, 0.0, 1.0) );
}
