#include "Entity.h"

Entity::Entity(Entity* parent) {
	this->parent = parent;
}

const glm::mat4 Entity::getModelMatrix() {
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, position);
	// Construct the rotation matrix from the unit vectors
	glm::mat4 rotationMatrix = glm::mat4(glm::vec4(right, 0), glm::vec4(up, 0), glm::vec4(direction, 0), glm::vec4(0, 0, 0, 1));
	modelMatrix = modelMatrix * rotationMatrix;
	modelMatrix = glm::scale(modelMatrix, scale);

	if (parent != NULL) {
		modelMatrix = parent->getModelMatrix() * modelMatrix;
	}
	return modelMatrix;
}

void Entity::resetRotation() {
	angle = 0;
	direction = glm::vec3(0, 0, 1);
	up = glm::vec3(0, 1, 0);
	right = glm::vec3(1, 0, 0);
}

void Entity::rotate(float angle, bool up) {
	if (up) {
		// rotating up changes the direction of up and forwards
		glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(angle), this->right);
		this->up = glm::normalize(glm::vec3(rotation * glm::vec4(this->up, 1)));
		this->direction = glm::normalize(glm::vec3(rotation * glm::vec4(this->direction, 1)));
	}
	else {
		// rotating sideways changes the direction of forwards and right
		glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(angle), this->up);
		this->right = glm::normalize(glm::vec3(rotation * glm::vec4(this->right, 1)));
		this->direction = glm::normalize(glm::vec3(rotation * glm::vec4(this->direction, 1)));
	}
}

void Entity::rotateDelta(float angle, bool up, float deltaTime) {
	float velocity = this->speed * deltaTime;
	if (up) {
		// rotating up changes the direction of up and forwards
		glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(angle)*velocity, this->right);
		this->up = glm::normalize(glm::vec3(rotation * glm::vec4(this->up, 1)));
		this->direction = glm::normalize(glm::vec3(rotation * glm::vec4(this->direction, 1)));
	}
	else {
		// rotating sideways changes the direction of forwards and right
		glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(angle)*velocity, this->up);
		this->right = glm::normalize(glm::vec3(rotation * glm::vec4(this->right, 1)));
		this->direction = glm::normalize(glm::vec3(rotation * glm::vec4(this->direction, 1)));
	}
}
