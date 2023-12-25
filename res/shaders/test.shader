#shader vertex
#version 410 core
#define PI 3.14159265359

layout(location = 0) in vec4 position;
out vec4 fragPosition;

uniform mat4 u_MVP;
uniform float u_Time;

void main()
{
    float x = position.x;
    float z = position.z;
    float y = sin(0.5 * (x + z + 10.0 * u_Time)) * 1.0;

    vec4 displacedPosition = position;
    displacedPosition.y += y;

    gl_Position = u_MVP * displacedPosition;
    fragPosition = displacedPosition;
}


#shader fragment
//NOT MY OWN SHADER, COPIED FROM https://www.shadertoy.com/view/lscBRf

#version 410 core

#define FALLING_SPEED  0.25
#define STRIPES_FACTOR 2.0

layout(location = 0) in vec4 fragPosition;
out vec4 fragColor;

uniform vec3 iResolution;
uniform float iTime;


//get sphere
float sphere(vec2 coord, vec2 pos, float r) {
    vec2 d = pos - coord; 
    return smoothstep(60.0, 0.0, dot(d, d) - r * r);
}

//main
void main()
{
    vec2 fragCoord  = fragPosition.xz;
    //normalize pixel coordinates
    vec2 uv         = fragCoord / iResolution.xy;
    //pixellize uv
    vec2 clamped_uv = (round(fragCoord / STRIPES_FACTOR) * STRIPES_FACTOR) / iResolution.xy;
    //get pseudo-random value for stripe height
    float value		= fract(sin(clamped_uv.x) * 43758.5453123);
    //create stripes
    vec3 col        = vec3(1.0 - mod(uv.y * 0.5 + (iTime * (FALLING_SPEED + value / 5.0)) + value, 0.5));
    //add color
         col       *= clamp(cos(iTime * 2.0 + uv.xyx + vec3(0, 2, 4)), 0.0, 1.0);
    //add glowing ends
    	 col 	   += vec3(sphere(fragCoord, 
                                  vec2(clamped_uv.x, (1.0 - 2.0 * mod((iTime * (FALLING_SPEED + value / 5.0)) + value, 0.5))) * iResolution.xy, 
                                  0.9)) / 2.0; 
    //add screen fade
         col       *= vec3(exp(-pow(abs(uv.y - 0.5), 6.0) / pow(2.0 * 0.05, 2.0)));
    // Output to screen
    fragColor       = vec4(col,1.0);
}


