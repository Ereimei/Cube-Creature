#pragma once

#include "Entity.h"

#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <glm.hpp>
#include <glew.h>
#include <time.h>
#include <vector>
#include <string>
#include <iostream>

class Cube : public Entity {
private:
	const float* color;

	// only load the vertices and texture once
	static bool loaded;

	//VAO
	static GLuint vertexArrayID;
	//VBO
	static GLuint vertexBufferID;
	//VBO_NORMAL
	static GLuint vertexBufferNorm;
	//VBUVO
	static GLuint vertexBufferUV;
	//Texture Offset
	static GLuint textureIDHorse;
	static GLuint textureIDGrass;

	//vector
	static std::vector<glm::vec3> v;
	//normalize vector
	static std::vector<glm::vec3> nv;
	//unit vector
	static std::vector<glm::vec2> uv;

	std::string type;

public:
	Cube(Entity* parent, const float* color, std::string type);
	void randomLocation();
	virtual void draw(GLenum mode, GLuint mm_addr, GLuint color_addr, bool depthPass);
};