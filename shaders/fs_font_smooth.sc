$input v_color0, v_texcoord0, v_shift, v_gamma

#include "common.sh"
SAMPLER2D(u_texColor, 0);

void main()
{
    vec2 uv = v_texcoord0.xy;
    float shift = v_shift;

    // LCD Off
    //if( pixel.z == 1.0)
    //{
        float a = texture2D(u_texColor, uv).a;
        gl_FragColor = v_color0 * pow( a, 1.0/v_gamma );
    //  return;
    //}
/*
    // LCD On
    vec4 current = texture2D(texture, uv);
    vec4 previous= texture2D(texture, uv+vec2(-1.,0.)*pixel.xy);
    vec4 next    = texture2D(texture, uv+vec2(+1.,0.)*pixel.xy);

    float r = current.r;
    float g = current.g;
    float b = current.b;

    if( shift <= 0.333 )
    {
        float z = shift/0.333;
        r = mix(current.r, previous.b, z);
        g = mix(current.g, current.r,  z);
        b = mix(current.b, current.g,  z);
    } 
    else if( shift <= 0.666 )
    {
        float z = (shift-0.33)/0.333;
        r = mix(previous.b, previous.g, z);
        g = mix(current.r,  previous.b, z);
        b = mix(current.g,  current.r,  z);
    }
   else if( shift < 1.0 )
    {
        float z = (shift-0.66)/0.334;
        r = mix(previous.g, previous.r, z);
        g = mix(previous.b, previous.g, z);
        b = mix(current.r,  previous.b, z);
    }

    vec3 color = pow( vec3(r,g,b), vec3(1.0/vgamma));
    gl_FragColor.rgb = color*gl_Color.rgb;
    gl_FragColor.a = (color.r+color.g+color.b)/3.0 * gl_Color.a;
*/
}
