$input v_color0, v_texcoord0

#include "common.sh"

SAMPLER2D(u_texColor, 0);

void main()
{
    float dist = texture2D(u_texColor, v_texcoord0.xy).r;
    float width = fwidth(dist);
    float alpha = smoothstep(0.5-width, 0.5+width, dist);
    gl_FragColor = vec4(v_color0.rgb, alpha*v_color0.a);
}
