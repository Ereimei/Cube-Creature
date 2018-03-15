#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 3) in vec3 normal;

out vec4 col;
out vec3 nrm;
out vec3 fpo;
out vec4 fpoLightSpace;

uniform mat4 m_m;
uniform mat4 v_m;
uniform mat4 p_m;
uniform vec4 color;
uniform mat4 lightSpaceMatrix;

void main()
{  
	fpo = vec3(m_m * vec4(pos, 1.0f));
	nrm = vec3(transpose(inverse(m_m)) * vec4(normal, 1.0f));
	col = vec4(color);
	fpoLightSpace = lightSpaceMatrix * vec4(fpo, 1.0f);
	gl_Position = p_m * v_m * m_m * vec4(pos, 1.0f);
}