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
#define PI 3.14159265359

layout(location = 0) in vec4 fragPosition;
out vec4 color;

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_ViewPos;

uniform float u_Wavelength;

float RainbowComponent(float coord)
{
    float phaseShift = 0.0;
    float amplitude = 1.0;

    return 0.5 + amplitude * sin(2.0 * PI * (coord + phaseShift) / u_Wavelength);
}

vec3 RainbowMapping(float xyzCoord)
{
    float normalizedCoord = (xyzCoord + 3.0) / 6.0;

    float r = RainbowComponent(normalizedCoord + 0.0);
    float g = RainbowComponent(normalizedCoord + 0.33);
    float b = RainbowComponent(normalizedCoord + 0.67);

    r = r * 0.5 + 0.5;
    g = g * 0.5 + 0.5;
    b = b * 0.5 + 0.5;

    return vec3(r, g, b);
}

void main()
{
    vec3 fragPos = fragPosition.xyz;
    vec3 normal = normalize(fragPos);

    vec3 lightDir = normalize(u_LightPos - fragPos);
    vec3 viewDir = normalize(u_ViewPos - fragPos);
    vec3 halfDir = normalize(lightDir + viewDir);

    vec3 ambient = u_LightColor * RainbowMapping(dot(normal, lightDir));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = u_LightColor * (RainbowMapping(dot(normal, lightDir)) * diff);

    float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);
    vec3 specular = u_LightColor * (RainbowMapping(dot(normal, halfDir)) * spec);

    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.0);
}