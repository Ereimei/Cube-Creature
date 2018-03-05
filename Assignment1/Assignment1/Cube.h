#pragma once

#include "Entity.h"

#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <glm.hpp>
#include <glew.h>
#include <time.h>
#include <vector>

class Cube : public Entity {
private:
	const float* color;

	GLuint vertexArrayID;
	GLuint vertexBufferID;

	//vector
	std::vector<glm::vec3> v;
	//normalize vector
	std::vector<glm::vec3> nv;
	//unit vector
	std::vector<glm::vec2> uv;
public:
	Cube(Entity* parent, const float* color);
	
	void randomLocation();
	virtual void draw(GLenum mode, GLuint mm_addr, GLuint color_addr);
};