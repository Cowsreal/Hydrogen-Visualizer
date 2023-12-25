#shader vertex
#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 vertexColor;

uniform mat4 u_MVP;

out vec4 fragmentColor;

void main()
{
    gl_Position = u_MVP * position;
    fragmentColor = vertexColor;
}

#shader fragment
#version 410 core

in vec4 fragmentColor;

out vec4 color;

void main()
{
    color = fragmentColor;
}