#shader vertex
#version 410 core
        
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 instancePosition;
layout(location = 2) in vec4 instanceColor;
layout(location = 3) in float instanceRadius;

uniform mat4 u_MVP;

out vec4 fragColor;

void main()
{
    vec4 scaledPosition = position;
    scaledPosition.xyz *= instanceRadius;
    scaledPosition.xyz += instancePosition;

    gl_Position = u_MVP * scaledPosition;
    fragColor = instanceColor;
}

#shader fragment
#version 410 core

layout(location = 0) out vec4 color;

in vec4 fragColor;

void main()
{
	color = fragColor;
}