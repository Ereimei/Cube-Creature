#pragma once

#include "Entity.h"
#include <glm.hpp>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <glew.h>
#include <glfw3.h>

#include <iostream>

class World : public Entity {
private:
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 rotation = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(.5, .5, .5);

	//VBUVO
	static GLuint vertexBufferUV;
	//unit vector
	static std::vector<glm::vec2> uv;

	std::vector<Entity*> entities = std::vector<Entity*>();
	//Texture Offset
	GLuint textureID_World = 0;
	void drawGrid(GLuint mm_addr, GLuint color_addr);
public:
	World() : Entity(NULL) {};
	virtual void draw(GLenum drawMode, GLuint mm_addr, GLuint color_addr, bool depthPass);
	inline void addEntity(Entity* entity) { entities.push_back(entity); }
};