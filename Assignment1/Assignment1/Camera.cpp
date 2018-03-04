#include "Camera.h"

//update the view of the camera
void Camera::update_view() {
	viewMatrix = glm::lookAt(c_pos, c_pos + c_dir, c_up);
}

//camera moves on up and down on y axis, and left and right on x axis
void Camera::update_position(double x, double y) {
	glm::vec4 left = glm::rotate(glm::mat4(1), glm::pi<float>() / 2, c_up) * glm::vec4(c_dir, 1);
	left = glm::normalize(left);
	c_pos -= glm::vec3(x * left.x, x * left.y, x * left.z);
	c_pos -= glm::vec3(y * c_up.x, y * c_up.y, y * c_up.z);
	update_view();
}

//setting the position and orientation back to original
void Camera::initial() {
	c_pos = glm::vec3(0, 30, 0);
	c_dir = glm::vec3(0, -1, 0);
	c_up = glm::vec3(0, 0, -1);
	update_view();
}

//rotate around camera up axis to pan
void Camera::pan_camera(float rad) {
	glm::vec4 newDirection = glm::rotate(glm::mat4(1), rad, c_up) * glm::vec4(c_dir, 1);
	c_dir = glm::normalize(glm::vec3(newDirection.x, newDirection.y, newDirection.z));
	update_view();
}

//rotate around camera side axis to tilt
void Camera::tilt_camera(float rad) {
	// get a vector pointing to the right to rotate around
	glm::vec3 side = glm::cross(c_dir, c_up);
	// rotate both the direction and up by the same amount around the side
	glm::vec4 newDirection = glm::rotate(glm::mat4(1), rad, side) * glm::vec4(c_dir, 1);
	glm::vec4 newUp = glm::rotate(glm::mat4(1), rad, side) * glm::vec4(c_up, 1);
	c_up = glm::normalize(glm::vec3(newUp.x, newUp.y, newUp.z));
	c_dir = glm::normalize(glm::vec3(newDirection.x, newDirection.y, newDirection.z));
	update_view();
}

//move carema on the camera direction
void Camera::zoom_camera(float amount) {
	c_pos += c_dir * amount;
	update_view();
}
