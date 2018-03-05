#include "World.h"
#include "Colors.h"

void World::draw(GLenum drawMode, GLuint mm_addr, GLuint color_addr) {
	
	glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(getModelMatrix()));
	glUniform4fv(color_addr, 1, Colors::black);

	// Draw the grid
	glBegin(GL_LINES);
	for (int i = -50; i <= 50; i++) {
		glVertex3f(i, 0, -50);
		glVertex3f(i, 0, 50);
	};
	for (float i = -50; i <= 50; i++) {
		glVertex3f(-50, 0, i);
		glVertex3f(50, 0, i);
	};
	glEnd();

	//X-Axis
	glUniform4fv(color_addr, 1, Colors::red);
	glBegin(GL_LINES);
	glVertex3f(0, 0.01, 0);
	glVertex3f(5, 0.01, 0);
	glEnd();

	//Y-Axis
	glUniform4fv(color_addr, 1, Colors::green);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 5, 0);
	glEnd();

	//Z-Axis
	glUniform4fv(color_addr, 1, Colors::blue);
	glBegin(GL_LINES);
	glVertex3f(0, 0.01, 0);
	glVertex3f(0, 0.01, 5);
	glEnd();

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->draw(drawMode, mm_addr, color_addr);;
	}
}