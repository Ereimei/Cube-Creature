#version 330 core

layout (location = 0) in vec3 pos;

out vec4 col;

uniform mat4 m_m;
uniform mat4 v_m;
uniform mat4 p_m;
uniform vec4 color;

void main()
{  
	gl_Position = p_m * v_m * m_m * vec4(pos, 1.0f);
	col = vec4(color);
}