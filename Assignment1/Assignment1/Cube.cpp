#include "Cube.h"
#include "ObjectLoader.h"
#include "Colors.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool Cube::loaded = false;
GLuint Cube::vertexArrayID;
GLuint Cube::vertexBufferID;
GLuint Cube::vertexBufferUV;
GLuint Cube::vertexBufferNorm;
GLuint Cube::textureIDHorse;
GLuint Cube::textureIDGrass;
std::vector<glm::vec3> Cube::v;
std::vector<glm::vec3> Cube::nv;
std::vector<glm::vec2> Cube::uv;

Cube::Cube(Entity* parent, const float* color, std::string type) : Entity(parent) {
	this->color = color;
	this->type= type;

	if (!loaded) {
		loadOBJ("UnitCube.obj", v, nv, uv);

		// store the vertices in memory so the cube can be drawn
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*v.size(), &v[0], GL_STATIC_DRAW);

		glGenBuffers(1, &vertexBufferNorm);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferNorm);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*nv.size(), &nv[0], GL_STATIC_DRAW);

		//--TEXTURE--
		glGenBuffers(1, &vertexBufferUV);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferUV);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*uv.size(), &uv[0], GL_STATIC_DRAW);

		//----HORSE----
		glGenTextures(1, &textureIDHorse);
		glBindTexture(GL_TEXTURE_2D, textureIDHorse);
		// load and generate the texture
		int width, height, nrChannels;
		unsigned char *dataHorse = stbi_load("horse_texture.jpg", &width, &height, &nrChannels, 0);
		if (dataHorse)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, dataHorse);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		//----GRASS----
		glGenTextures(1, &textureIDGrass);
		glBindTexture(GL_TEXTURE_2D, textureIDGrass);
		// load and generate the texture
		unsigned char *dataGrass = stbi_load("grass_texture.jpg", &width, &height, &nrChannels, 0);
		if (dataGrass)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, dataGrass);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(dataHorse);
		stbi_image_free(dataGrass);

		loaded = true;
	}
}


void Cube::draw(GLenum mode, GLuint mm_addr, GLuint color_addr, bool depthPass) {
	glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(getModelMatrix()));

	if (!depthPass)
		glUniform4fv(color_addr, 1, color);

	// tell opengl to use the vertices found from loading the unit cube
	glBindVertexArray(vertexArrayID);
	// bind the buffer so it can be passed to the shader
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// bind the uv buffer
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferUV);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	if (!depthPass) 
	{
		// bind the texture to be used by the shader
		glActiveTexture(GL_TEXTURE1);
		if (type.compare("grass") == 0) {
			glBindTexture(GL_TEXTURE_2D, textureIDGrass);
		}
		if (type.compare("horse") == 0) {
			glBindTexture(GL_TEXTURE_2D, textureIDHorse);
		}
	}

	// bind normals
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferNorm);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);

	glDrawArrays(mode, 0, v.size());

	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(0);
}

void Cube::randomLocation() {
	// Random number between 0-100
	int n1 = rand() % 101;
	int n2 = rand() % 101;
	// Shift the number so the range is -50 to 50
	n1 -= 50;
	n2 -= 50; 
	setPosition(glm::vec3(n1, 7.5, n2));
}
