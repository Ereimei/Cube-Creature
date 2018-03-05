#pragma once

#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <glm.hpp>
#include <glew.h>
#include <time.h>
#include <vector>

class Entity {
private:
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(.5, .5, .5);

	glm::vec3 scaleRatio = glm::vec3(1, 1, 1);

	float angle = 0;
	glm::vec3 direction = glm::vec3(0, 0, 1);
	glm::vec3 up = glm::vec3(0, 1, 0);
	glm::vec3 right = glm::vec3(1, 0, 0);

	Entity* parent;
public:
	Entity(Entity* parent);

	inline void setScale(glm::vec3 scale) { this->scale = scale; }
	inline void setPosition(glm::vec3 position) { this->position = position; }
	inline void translate(float x, float y, float z) { this->position = glm::vec3(position.x + x, position.y + y, position.z + z); }
	inline void scaling(float x) { this->scale = glm::vec3(x, x, x) * scale; }

	void resetRotation();
	void rotate(float angle, bool up);
	virtual void draw(GLenum drawMode, GLuint mm_addr, GLuint color_addr) = 0;
	const glm::mat4 getModelMatrix();
};