#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 lightSpaceMatrix;
uniform mat4 m_m;

void main()
{
    gl_Position = lightSpaceMatrix * m_m * vec4(pos, 1.0);
}