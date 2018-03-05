// Assignment1.cpp : Defines the entry point for the console application.
//


#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "Colors.h"
#include "Camera.h"
#include "Cube.h"
#include "World.h"
#include "ObjectLoader.h"

#include <glm.hpp>
#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>


GLFWwindow* window;
Camera* camera;
Cube* cubeBody;
Cube* cubeFLU;
Cube* cubeFLW;
Cube* cubeFLK;
World* world;
GLenum drawMode = GL_TRIANGLES;

//Shader loader function
GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path) {
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, std::ios::in);

	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_shader_path.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_shader_path.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);

	glBindAttribLocation(ProgramID, 0, "in_Position");

	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID); //free up memory
	glDeleteShader(FragmentShaderID);

	return ProgramID;

}

//varibles for mousebuttons
bool rightMouseButton = false;
bool middleMouseButton = false;
bool leftMouseButton = false;
glm::vec2* previousCursorPosition;

//If mouse button is active, put relative variable to true
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		rightMouseButton = action == GLFW_PRESS;
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		middleMouseButton = action == GLFW_PRESS;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		leftMouseButton = action == GLFW_PRESS;
	}
}

//locate the position of the cursor and activate whichever button is pressed
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (previousCursorPosition == nullptr)
		previousCursorPosition = new  glm::vec2(xpos, ypos);

	//right button is pressed → use mouse movement in x direction to pan
	if (rightMouseButton) {
		float horizontalMovement = previousCursorPosition->x - xpos;
		camera->pan_camera(horizontalMovement/100);
	}
	//middle button is pressed → use mouse movement in y direction to tilt
	if (middleMouseButton) {
		float verticalMovement = previousCursorPosition->y - ypos;
		camera->tilt_camera(verticalMovement / 100);
	}

	//left button is pressed → use mouse movement to move into/out of the scene
	if (leftMouseButton) {
		float zoomMovement = previousCursorPosition->y - ypos;
		camera->zoom_camera(zoomMovement/10);
	}

	previousCursorPosition->x = xpos;
	previousCursorPosition->y = ypos;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	//down arrow → R​ -y, down arrow + shift will move the camera down
	if (key == GLFW_KEY_DOWN) {
		if (mods == GLFW_MOD_SHIFT)
			camera->update_position(0, 1);
		else
			world->rotate(-5, true);
	}

	//up arrow → R​y​, up arrow + shift will move the camera up
	if (key == GLFW_KEY_UP) {
		if (mods == GLFW_MOD_SHIFT)
			camera->update_position(0, -1);
		else
			world->rotate(5, true);
	}

	//left arrow → R​x​, left arrow + shift will move the camera left
	if (key == GLFW_KEY_LEFT) {
		if (mods == GLFW_MOD_SHIFT)
			camera->update_position(-1, 0);
		else
			world->rotate(-5, false);
	}

	//right arrow → R​ -x, right arrow + shift will move the camera right
	if (key == GLFW_KEY_RIGHT) {
		if (mods == GLFW_MOD_SHIFT)
			camera->update_position(1, 0);
		else
			world->rotate(5, false);
	}

	//“Home” button should reset to the initial world position and orientation
	if (key == GLFW_KEY_HOME) {
		camera->initial();
		world->resetRotation();
		cubeBody->resetRotation();
		cubeBody->rotate(-90, false);
		cubeBody->setPosition(glm::vec3(0, 6, 0));
	}

	//pressing ‘U’ for scale-up
	if (key == GLFW_KEY_U) {
		cubeBody->scaling(1.2);
	}

	//‘J’ for scale-down
	if (key == GLFW_KEY_J) {
		cubeBody->scaling(0.8);
	}

	//A ​→ move left 1 grid unit, ​a ​→ rotate left 5 degrees about Y axis
	if (key == GLFW_KEY_A) {
		if (mods == GLFW_MOD_SHIFT)
			cubeBody->translate(-1, 0, 0);
		else
			cubeBody->rotate(-5, false);
	}

	//D → move right 1 grid unit, d → rotate right 5 degrees about Y axis
	if (key == GLFW_KEY_D) {
		if (mods == GLFW_MOD_SHIFT)
			cubeBody->translate(1, 0, 0);
		else
			cubeBody->rotate(5, false);
	}

	//W → move up 1 grid unit, w → rotate upwards 5 degrees raising the front legs
	if (key == GLFW_KEY_W) {
		if (mods == GLFW_MOD_SHIFT)
			cubeBody->translate(0, 0, -1);
		else
			cubeBody->rotate(5, true); 
	}

	//S → move down 1 grid unit, s → rotate downwards 5 degrees raising the hind legs
	if (key == GLFW_KEY_S) {
		if (mods == GLFW_MOD_SHIFT)
			cubeBody->translate(0, 0, 1);
		else
			cubeBody->rotate(-5, true); 
	}

	//Cube object draw in points
	if (key == GLFW_KEY_P) {
		drawMode = GL_POINTS;
	}

	//Cube object drawn in lines
	if (key == GLFW_KEY_L) {
		drawMode = GL_LINES;
	}

	//Cube object drawn in triangles
	if (key == GLFW_KEY_T) {
		drawMode = GL_TRIANGLES;
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		cubeBody->randomLocation();
	}

	if (key == GLFW_KEY_1) {
		if (mods == GLFW_MOD_SHIFT) {
			cubeFLK->rotate(5, true);
			cubeFLK->setScale(glm::vec3(0.3, 0.2, 0.2));
		}
		else {
			cubeFLK->rotate(-5, true);
		}
	}
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	camera->setProjectionMatrix(glm::perspective(45.f, ((float)width / (float)height), 0.1f, 100.f));
	glViewport(0, 0, width, height);
}

int init()
{
	glfwInit();

	window = glfwCreateWindow(800, 800, "Horse", nullptr, nullptr);
	camera = new Camera();
	camera->update_view();

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != 0) {
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

    return 0;
}

int main() {

	if (init() != 0) {
		return -1;
	}

	GLuint shdr = loadShaders("Shader.glsl", "Fragment.glsl");
	glUseProgram(shdr);
	GLuint mm_addr = glGetUniformLocation(shdr, "m_m");
	GLuint vm_addr = glGetUniformLocation(shdr, "v_m");
	GLuint pm_addr = glGetUniformLocation(shdr, "p_m");
	GLuint color_addr = glGetUniformLocation(shdr, "color");

	//WORLD (contains the grid and axes)
	world = new World();

	//HORSE
	//F:Front, B:Back, L:Left, R:Right, U:Upper, W:Lower
	//Body
	cubeBody = new Cube(world, Colors::grey8);
	cubeBody->setPosition(glm::vec3(0, 6, 0));
	cubeBody->rotate(-90, false);
	cubeBody->setScale(glm::vec3(3, 3, 6));

	//Front left upper leg
	cubeFLU = new Cube(cubeBody, Colors::grey5);
	cubeFLU->setPosition(glm::vec3(0.3, -0.8, -0.35));
	cubeFLU->setScale(glm::vec3(0.3, 0.8, 0.2));

	//Front left lower knee
	cubeFLK = new Cube(cubeFLU, Colors::red);
	cubeFLK->setPosition(glm::vec3(0, -0.4, 0));
	cubeFLK->setScale(glm::vec3(1, 0.2, 1));

	//Front left lower leg
	cubeFLW = new Cube(cubeFLK, Colors::grey4);
	cubeFLW->setPosition(glm::vec3(0, -2, 0));
	cubeFLW->setScale(glm::vec3(1, 6, 1));

	//Front Right upper leg
	Cube* cubeFRU = new Cube(cubeBody, Colors::grey5);
	cubeFRU->setPosition(glm::vec3(-0.3, -0.5, -0.35));
	cubeFRU->setScale(glm::vec3(0.3, 1.3, 0.2));
	
	//Back left upper leg
	Cube* cubeBLU = new Cube(cubeBody, Colors::grey5);
	cubeBLU->setPosition(glm::vec3(0.3, -0.5, 0.35));
	cubeBLU->setScale(glm::vec3(0.3, 1.3, 0.2));
	
	//Back Right upper leg
	Cube* cubeBRU = new Cube(cubeBody, Colors::grey5);
	cubeBRU->setPosition(glm::vec3(-0.3, -0.5, 0.35));
	cubeBRU->setScale(glm::vec3(0.3, 1.3, 0.2));
	
	//Front Right lower leg
	Cube* cubeFRW = new Cube(cubeBody, Colors::grey4);
	cubeFRW->setPosition(glm::vec3(-0.3, -1.5, -0.35));
	cubeFRW->setScale(glm::vec3(0.3, 1, 0.2));
	
	//Back left lower leg
	Cube* cubeBLW = new Cube(cubeBody, Colors::grey4);
	cubeBLW->setPosition(glm::vec3(0.3, -1.5, 0.35));
	cubeBLW->setScale(glm::vec3(0.3, 1, 0.2));
	
	//Back Right lower leg
	Cube* cubeBRW = new Cube(cubeBody, Colors::grey4);
	cubeBRW->setPosition(glm::vec3(-0.3, -1.5, 0.35));
	cubeBRW->setScale(glm::vec3(0.3, 1, 0.2));
	
	//Neck
	Cube* cubeNeck = new Cube(cubeBody, Colors::grey7);
	cubeNeck->setPosition(glm::vec3(0, 0.15, -0.5));
	cubeNeck->rotate(45, true);
	cubeNeck->setScale(glm::vec3(0.8, 0.5, 1.1));
	
	//Head
	Cube* cubeHead = new Cube(cubeNeck, Colors::grey6);
	cubeHead->setPosition(glm::vec3(0, -0.8, -0.4));
	cubeHead->rotate(90, true);
	cubeHead->setScale(glm::vec3(0.95, 0.3, 1));

	// Add all cubes to the world to be drawn and so they are relative to the world
	world->addEntity(cubeFLU);
	world->addEntity(cubeFRU);
	world->addEntity(cubeBLU);
	world->addEntity(cubeBRU);
	world->addEntity(cubeFLK);
	world->addEntity(cubeFLW);
	world->addEntity(cubeFRW);
	world->addEntity(cubeBLW);
	world->addEntity(cubeBRW);
	world->addEntity(cubeHead);
	world->addEntity(cubeNeck);
	world->addEntity(cubeBody);

	//window background color
	glClearColor(0, .4, .4, 0);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(vm_addr, 1, false, glm::value_ptr(camera->getViewMatrix()));
		glUniformMatrix4fv(pm_addr, 1, false, glm::value_ptr(camera->getProjectionMatrix()));

		world->draw(drawMode, mm_addr, color_addr);
	}

	return 0;
}