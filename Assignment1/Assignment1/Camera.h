#pragma once

#include <glm.hpp>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <glew.h>
#include <glfw3.h>

class Camera {
private:
	//camera position
	glm::vec3 c_pos = glm::vec3(0, 30, 0);
	//camera direction
	glm::vec3 c_dir = glm::vec3(0, -1, 0);
	//camera up axis
	glm::vec3 c_up = glm::vec3(0, 0, -1);
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix = glm::perspective(45.f, 800.f / 800.f, 0.1f, 100.f);
public:
	void initial();
	void update_position(double x, double y);
	void update_view();
	void pan_camera(float rad);
	void tilt_camera(float rad);
	void zoom_camera(float magnification);
	inline void setProjectionMatrix(glm::mat4 projectionMatrix) { this->projectionMatrix = projectionMatrix; };
	const glm::mat4 getViewMatrix() { return viewMatrix; };
	const glm::mat4 getProjectionMatrix() { return projectionMatrix; };
};