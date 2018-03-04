#include "Cube.h"
#include "ObjectLoader.h"

Cube::Cube(Entity* parent, const float* color) : Entity(parent) {
	this->color = color;
	loadOBJ("UnitCube.obj", v, nv, uv);

	// store the vertices in memory so the cube can be drawn
	vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	vertexBufferID;
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*v.size(), &v[0], GL_STATIC_DRAW);
}

void Cube::draw(GLenum mode, GLuint mm_addr, GLuint color_addr) {
	glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(getModelMatrix()));
	glUniform4fv(color_addr, 1, color);
	// tell opengl to use the vertices found from loading the unit cube
	glBindVertexArray(vertexArrayID);
	// bind the buffer so it can be passed to the shader
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(mode, 0, v.size());

	glDisableVertexAttribArray(0);
}

void Cube::randomLocation() {
	// Random number between 0-100
	int n1 = rand() % 101;
	int n2 = rand() % 101;
	// Shift the number so the range is -50 to 50
	n1 -= 50;
	n2 -= 50; 
	setPosition(glm::vec3(n1, 6, n2));
}
