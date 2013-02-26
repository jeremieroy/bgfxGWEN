$input v_color0, v_texcoord0

#include "common.sh"

SAMPLER2D(u_texColor, 0);

void main()
{
	float a = texture2D(u_texColor, v_texcoord0.xy).r;
	gl_FragColor = vec4(v_color0.rgb, v_color0.a * a);
    //gl_FragColor = gl_Color * pow( a, 1.0/vgamma );
}
