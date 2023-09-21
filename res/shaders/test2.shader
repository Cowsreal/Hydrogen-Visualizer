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
#version 410 core

//NOT MY OWN SHADER, COPIED FROM https://www.shadertoy.com/view/ldcyR4

#define      PI 3.14159265358979323846264338327950288419716939937511 // mm pie
#define     TAU 6.28318530717958647692528676655900576839433879875021 // pi * 2
#define HALF_PI 1.57079632679489661923132169163975144209858469968755 // pi / 2

//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise
//

layout(location = 0) in vec4 fragPosition;
out vec4 fragColor;

uniform vec3 iResolution;
uniform float iTime;

vec3 mod289(vec3 x) {
    return x - floor(x * (1. / 289.)) * 289.;
}

vec2 mod289(vec2 x) {
    return x - floor(x * (1. / 289.)) * 289.;
}

vec3 permute(vec3 x) {
    return mod289(((x * 34.) + 1.) * x);
}

float snoise(vec2 v) {
  const vec4 C = vec4(.211324865405187,.366025403784439,-.577350269189626,.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);
  vec2 i1 = (x0.x > x0.y) ? vec2(1., 0.) : vec2(0., 1.);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod289(i);
  vec3 p = permute( permute( i.y + vec3(0., i1.y, 1. )) + i.x + vec3(0., i1.x, 1. ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.);
  m = m*m;
  m = m*m;
  vec3 x = 2. * fract(p * C.www) - 1.;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - .85373472095314 * ( a0*a0 + h*h );
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130. * dot(m, g);
}

vec3 hsv2rgb(vec3 c) {
  vec3 rgb = clamp(abs(mod(c.x*6.+vec3(0.,4.,2.),6.)-3.)-1.,0.,1.);
  rgb = rgb * rgb * (3. - 2. * rgb);
  return c.z * mix(vec3(1.), rgb, c.y);
}

vec2 pq(vec2 uv) {
  return vec2(atan(uv.x, uv.y) / TAU + .5, length(uv));;
}

vec4 glorb(vec2 uv, vec2 offset, float radius) {
  vec2 pq = pq(uv + offset);
  float r = radius * snoise(uv + iTime * .2);
  float s = 8. / iResolution.y;
  float m = smoothstep(r + s, r - s, pq.y);
  vec3 c = hsv2rgb(vec3(pq.x, 1., 1.));
  return vec4(c, 1.) * m;
}

vec4 field(vec2 uv, vec2 offset, float radius) {
  vec4 c0 = glorb(uv, offset, radius);
  vec4 c1 = glorb(uv, offset, radius * .92);
  return c0 - c1;
}

void main() {
  vec2 fragCoord = fragPosition.xz;
  vec2 uv = (2. * fragCoord.xy - iResolution.xy) / iResolution.y;
  float rainbowHue = fract(iTime * 0.2);
  vec3 rainbowColor = hsv2rgb(vec3(rainbowHue, 1.0, 1.0));

  // Set the background color to the rainbow color
  fragColor = vec4(rainbowColor, 1.0);

  vec4 r0 = field(uv, vec2( .0, .0), 1.66);
  vec4 r1 = field(uv, vec2( .33, .33), 1.66);
  vec4 r2 = field(uv, vec2( .33, -.33), 1.66);
  vec4 r3 = field(uv, vec2(-.33, -.33), 1.66);
  vec4 r4 = field(uv, vec2(-.33, .33), 1.66);
  fragColor = r0+r1+r2+r3+r4;
}

