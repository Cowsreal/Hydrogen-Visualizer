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
    float y = sin(0.5 * (x + z + 10.0 * u_Time)) * 0.5;

    vec4 displacedPosition = position;
    displacedPosition.y += y;

    gl_Position = u_MVP * displacedPosition;
    fragPosition = displacedPosition;
}

#shader fragment
#version 410 core
#define PI 3.14159265359

layout(location = 0) in vec4 fragPosition;
out vec4 color;

uniform float u_Wavelength;
uniform float u_PhaseShift;
uniform float u_Amplitude;
uniform float u_Norm;

float SmoothRainbowComponent(float coord)
{
    float wavelength = 2.0; // Adjust this value to control the smoothness
    float phaseShift = 0.0; // Adjust this value to control the phase shift
    float amplitude = 0.5; // Adjust this value to control the intensity

    // Calculate a smooth sine wave with a phase shift
    return 0.5 + u_Amplitude * sin(6.283185 * (coord + phaseShift) / u_Wavelength);
}

vec3 RainbowMapping(float radius)
{
    float hue = radius / u_PhaseShift;

    // Calculate rainbow colors using hue
    float r = SmoothRainbowComponent(hue);
    float g = SmoothRainbowComponent(hue + 0.33);
    float b = SmoothRainbowComponent(hue + 0.67);

    // Increase the saturation and vibrancy of colors
    r = r * 0.5 + 0.5;
    g = g * 0.5 + 0.5;
    b = b * 0.5 + 0.5;
    
    r = r / u_Norm;
    g = g / u_Norm;
    b = b / u_Norm;

    return vec3(r, g, b);
}

void main()
{
    vec2 position = fragPosition.xz; // Extract the x and y components of the position
    float radius = length(position);

    // Map x and y coordinates to RGB colors for a circular rainbow fade
    vec3 colorRGB = RainbowMapping(radius);

    color = vec4(colorRGB, 1.0); // Set the color with full opacity
}


