#pragma once

#include "Entity.h"
#include <glm.hpp>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <glew.h>
#include <glfw3.h>

class World : public Entity {
private:
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 rotation = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(.5, .5, .5);

	std::vector<Entity*> entities = std::vector<Entity*>();
public:
	World() : Entity(NULL) {}
	virtual void draw(GLenum drawMode, GLuint mm_addr, GLuint color_addr);
	inline void addEntity(Entity* entity) { entities.push_back(entity); }
};