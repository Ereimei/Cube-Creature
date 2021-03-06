// Assignment1.cpp : Defines the entry point for the console application.
//


#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "Colors.h"
#include "Camera.h"
#include "PointLight.h"
#include "Cube.h"
#include "World.h"
#include "ObjectLoader.h"
#include "stb_image.h"

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
PointLight* light;
//--BODY--
Cube* cubeBody;
Cube* bodyJoint;
//--FRONT LEFT--
Cube* cubeFLJ;
Cube* cubeFLU;
Cube* cubeFLK;
Cube* cubeFLW;
//--FRONT RIGHT--
Cube* cubeFRJ;
Cube* cubeFRU;
Cube* cubeFRK;
Cube* cubeFRW;
//--BACK LEFT--
Cube* cubeBLJ;
Cube* cubeBLU;
Cube* cubeBLK;
Cube* cubeBLW;
//--BACK RIGHT--
Cube* cubeBRJ;
Cube* cubeBRU;
Cube* cubeBRK;
Cube* cubeBRW;
//--HEAD & NECK--
Cube* cubeHead;
Cube* headJoint;
Cube* cubeNeck;
Cube* neckJoint;

World* world;
bool texture = false;
bool shadow = true;
bool shadowKey = false;
bool gallop = false;
GLenum drawMode = GL_TRIANGLES;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
int windowWidth = 800, windowHeight = 800;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

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

	if (action != GLFW_PRESS)
		return;

	if (key == GLFW_KEY_ESCAPE) {
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
		bodyJoint->resetRotation();
		bodyJoint->rotate(90, false);
		bodyJoint->setPosition(glm::vec3(0, 7.5, 0));
	}

	//pressing ‘U’ for scale-up
	if (key == GLFW_KEY_U) {
		bodyJoint->scaling(1.2);
	}

	//‘J’ for scale-down
	if (key == GLFW_KEY_J) {
		bodyJoint->scaling(0.8);
	}

	//A ​→ move left 1 grid unit, ​a ​→ rotate left 5 degrees about Y axis
	if (key == GLFW_KEY_A) {
		if (mods == GLFW_MOD_SHIFT)
			bodyJoint->translate(-1, 0, 0);
		else
			bodyJoint->rotate(-5, false);
	}

	//D → move right 1 grid unit, d → rotate right 5 degrees about Y axis
	if (key == GLFW_KEY_D) {
		if (mods == GLFW_MOD_SHIFT)
			bodyJoint->translate(1, 0, 0);
		else
			bodyJoint->rotate(5, false);
	}

	//W → move up 1 grid unit, w → rotate upwards 5 degrees raising the front legs
	if (key == GLFW_KEY_W) {
		if (mods == GLFW_MOD_SHIFT)
			bodyJoint->translate(0, 0, -1);
		else
			bodyJoint->rotate(5, true);
	}

	//S → move down 1 grid unit, s → rotate downwards 5 degrees raising the hind legs
	if (key == GLFW_KEY_S) {
		if (mods == GLFW_MOD_SHIFT)
			bodyJoint->translate(0, 0, 1);
		else
			bodyJoint->rotate(-5, true);
	}

	//Cube object draw in points
	if (key == GLFW_KEY_P) {
		drawMode = GL_POINTS;
	}

	//Cube object drawn in lines
	if (key == GLFW_KEY_L) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawMode = GL_TRIANGLES;
	}

	//Cube object drawn in triangles
	if (key == GLFW_KEY_T) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawMode = GL_TRIANGLES;
	}

	if (key == GLFW_KEY_SPACE) {
		bodyJoint->randomLocation();
	}

	//1. head_to_neck
	if (key == GLFW_KEY_1) {
		if (mods == GLFW_MOD_SHIFT) {
			headJoint->rotate(5, true);
		}
		else {
			headJoint->rotate(-5, true);
		}
	}
	//2. neck_to_torso
	if (key == GLFW_KEY_2) {
		if (mods == GLFW_MOD_SHIFT) {
			neckJoint->rotate(5, true);
		}
		else {
			neckJoint->rotate(-5, true);
		}
	}
	//3. torso_to_front_upper_right_leg
	if (key == GLFW_KEY_3) {
		if (mods == GLFW_MOD_SHIFT) {
			cubeFRJ->rotate(5, true);
		}
		else {
			cubeFRJ->rotate(-5, true);
		}
	}
	//4. front right_knee
	if (key == GLFW_KEY_4) {
		if (mods == GLFW_MOD_SHIFT) {
			cubeFRK->rotate(5, true);
		}
		else {
			cubeFRK->rotate(-5, true);
		}
	}
	//5. torso_to_hind_upper_right_leg
	if (key == GLFW_KEY_5) {
		if (mods == GLFW_MOD_SHIFT) {
			cubeBRJ->rotate(5, true);
		}
		else {
			cubeBRJ->rotate(-5, true);
		}
	}
	//6. hind_right_knee
	if (key == GLFW_KEY_6) {
		if (mods == GLFW_MOD_SHIFT) {
			cubeBRK->rotate(5, true);
		}
		else {
			cubeBRK->rotate(-5, true);
		}
	}
	//7. torso_to_front_upper_left_leg
	if (key == GLFW_KEY_7) {
		if (mods == GLFW_MOD_SHIFT) {
			cubeFLJ->rotate(5, true);
		}
		else {
			cubeFLJ->rotate(-5, true);
		}
	}
	//8. front_left_knee
	if (key == GLFW_KEY_8) {
		if (mods == GLFW_MOD_SHIFT) {
			cubeFLK->rotate(5, true);
		}
		else {
			cubeFLK->rotate(-5, true);
		}
	}
	//9. torso_to_hind_upper_left_leg
	if (key == GLFW_KEY_9) {
		if (mods == GLFW_MOD_SHIFT) {
			cubeBLJ->rotate(5, true);
		}
		else {
			cubeBLJ->rotate(-5, true);
		}
	}
	//10.hind_left_knee
	//Use key 0 instead 
	if (key == GLFW_KEY_0) {
		if (mods == GLFW_MOD_SHIFT) {
			cubeBLK->rotate(5, true);
		}
		else {
			cubeBLK->rotate(-5, true);
		}
	}

	//Texture 
	if (key == GLFW_KEY_X) {
		texture = !texture;
	}

	//Shadow
	if (key == GLFW_KEY_B) {
		shadowKey = !shadowKey;
	}
	//Run cycle
	if (key == GLFW_KEY_R) {
		gallop != gallop;
	}
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	camera->setProjectionMatrix(glm::perspective(45.f, ((float)width / (float)height), 0.1f, 200.f));
	glViewport(0, 0, width, height);
	windowWidth = width;
	windowHeight = height;
}

int init()
{
	/*
	float lerp(float start, float end, float currentTime, float totalTime)
	{
		float x = start;
		float y = end;

		float r = currrentTime / totalTime;
		return (x * (1 - r)) + y * r;
	}

	float startRotation = 0;
	float endRotation = 20;
	float currentTime;

	
	currentTime += dt;
	float currentRotation = lerp(startRotation, endRotation, currentTime, 3);
	setNeckRotationY(currentRotation);

	if (currentTime >= 3)
	{
		currentTime = 0;
		float temp = startRotation;
		startRotation = endRotation;
		endRotation = startRotation;
	}
	*/


	glfwInit();

	window = glfwCreateWindow(windowWidth, windowHeight, "Horse", nullptr, nullptr);
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

	GLuint colorProgramId = loadShaders("Shader_color.glsl", "Fragment_color.glsl");
	GLuint textureProgramId = loadShaders("Shader_texture.glsl", "Fragment_texture.glsl");
	GLuint depthProgramId = loadShaders("Shader_depth.glsl", "Fragment_depth.glsl");

	//--WORLD-- (contains the grid and axes)
	world = new World();

	for (int i = 0; i < 4; i++) {
		Cube* worldCube = new Cube(world, Colors::green, "grass");
		worldCube->setScale(glm::vec3(50, 0.01, 50));
		switch (i) {
		case 0: worldCube->setPosition(glm::vec3(25, 0, 25)); break;
		case 1: worldCube->setPosition(glm::vec3(25, 0, -25)); break;
		case 2: worldCube->setPosition(glm::vec3(-25, 0, 25)); break;
		case 3: worldCube->setPosition(glm::vec3(-25, 0, -25)); break;
		}
		world->addEntity(worldCube);
	}

	//--HORSE--
	//F:Front, B:Back, L:Left, R:Right, U:Upper, W:Lower, K:knee, J:joint
	//Body Joint
	bodyJoint = new Cube(world, Colors::red, "horse");
	bodyJoint->setPosition(glm::vec3(0, 7.5, 0));
	bodyJoint->rotate(90, false);
	//Body
	cubeBody = new Cube(bodyJoint, Colors::grey6, "horse");
	cubeBody->setScale(glm::vec3(4, 4, 8));

	//--FRONT LEFT--
	//upper joint
	cubeFLJ = new Cube(bodyJoint, Colors::red, "horse");
	cubeFLJ->setPosition(glm::vec3(1, -2, 3));
	//upper leg
	cubeFLU = new Cube(cubeFLJ, Colors::grey5, "horse");
	cubeFLU->setPosition(glm::vec3(0, -1.5, 0));
	cubeFLU->setScale(glm::vec3(1.5, 3, 1.5));
	//lower knee
	cubeFLK = new Cube(cubeFLJ, Colors::red, "horse");
	cubeFLK->setPosition(glm::vec3(0, -2.5, 0));
	//lower leg
	cubeFLW = new Cube(cubeFLK, Colors::grey4, "horse");
	cubeFLW->setPosition(glm::vec3(0, -1.5, 0));
	cubeFLW->setScale(glm::vec3(1.5, 3, 1.5));

	//--FRONT RIGHT--
	//upper joint
	cubeFRJ = new Cube(bodyJoint, Colors::red, "horse");
	cubeFRJ->setPosition(glm::vec3(-1, -2, 3));
	//upper leg
	cubeFRU = new Cube(cubeFRJ, Colors::red, "horse");
	cubeFRU->setPosition(glm::vec3(0, -1.5, 0));
	cubeFRU->setScale(glm::vec3(1.5, 3, 1.5));
	//lower knee
	cubeFRK = new Cube(cubeFRJ, Colors::red, "horse");
	cubeFRK->setPosition(glm::vec3(0, -2.5, 0));
	//lower leg
	cubeFRW = new Cube(cubeFRK, Colors::grey4, "horse");
	cubeFRW->setPosition(glm::vec3(0, -1.5, 0));
	cubeFRW->setScale(glm::vec3(1.5, 3, 1.5));
	
	//--BACK LEFT--
	//upper joint
	cubeBLJ = new Cube(bodyJoint, Colors::red, "horse");
	cubeBLJ->setPosition(glm::vec3(1, -2, -3));
	//upper leg
	cubeBLU = new Cube(cubeBLJ, Colors::grey5, "horse");
	cubeBLU->setPosition(glm::vec3(0, -1.5, 0));
	cubeBLU->setScale(glm::vec3(1.5, 3, 1.5));
	//lower knee
	cubeBLK = new Cube(cubeBLJ, Colors::red, "horse");
	cubeBLK->setPosition(glm::vec3(0, -2.5, 0));
	//lower leg
	cubeBLW = new Cube(cubeBLK, Colors::grey4, "horse");
	cubeBLW->setPosition(glm::vec3(0, -1.5, 0));
	cubeBLW->setScale(glm::vec3(1.5, 3, 1.5));

	//--BACK RIGHT--
	//upper joint
	cubeBRJ = new Cube(bodyJoint, Colors::red, "horse");
	cubeBRJ->setPosition(glm::vec3(-1, -2, -3));
	//upper leg
	cubeBRU = new Cube(cubeBRJ, Colors::red, "horse");
	cubeBRU->setPosition(glm::vec3(0, -1.5, 0));
	cubeBRU->setScale(glm::vec3(1.5, 3, 1.5));
	//lower knee
	cubeBRK = new Cube(cubeBRJ, Colors::red, "horse");
	cubeBRK->setPosition(glm::vec3(0, -2.5, 0));
	//lower leg
	cubeBRW = new Cube(cubeBRK, Colors::grey4, "horse");
	cubeBRW->setPosition(glm::vec3(0, -1.5, 0));
	cubeBRW->setScale(glm::vec3(1.5, 3, 1.5));

	//--HEAD & NECK
	//Neck Joint
	neckJoint = new Cube(bodyJoint, Colors::red, "horse");
	neckJoint->setPosition(glm::vec3(0, -0.5, 3));
	neckJoint->rotate(-35, true);
	//Neck
	cubeNeck = new Cube(neckJoint, Colors::grey7, "horse");
	cubeNeck->setPosition(glm::vec3(0, 0, 2.5));
	cubeNeck->setScale(glm::vec3(2, 2, 5));
	//Head Joint
	headJoint = new Cube(neckJoint, Colors::red, "horse");
	headJoint->setPosition(glm::vec3(0, 0, 5));
	headJoint->rotate(90, true);
	//Head
	cubeHead = new Cube(headJoint, Colors::grey6, "horse");
	cubeHead->setPosition(glm::vec3(0, 0, 2));
	cubeHead->setScale(glm::vec3(2, 2, 4));

	//light 
	Cube* light = new Cube(world, Colors::white, "light");
	light->setPosition(glm::vec3(0, 20, 0));

	// Add all cubes to the world to be drawn and so they are relative to the world
	//Front left upper
	world->addEntity(cubeFLU);
	//Front right upper
	world->addEntity(cubeFRU);
	//Back left upper
	world->addEntity(cubeBLU);
	//Back right upper
	world->addEntity(cubeBRU);
	//Front left lower
	world->addEntity(cubeFLW);
	//Front right lower
	world->addEntity(cubeFRW);
	//Back left lower
	world->addEntity(cubeBLW);
	//Back right lower
	world->addEntity(cubeBRW);

	//Head and Neck
	world->addEntity(cubeHead);
	world->addEntity(cubeNeck);
	world->addEntity(cubeBody);

	world->addEntity(light);

	//window background color
	glClearColor(0, .4, .4, 0);

	//--SHADOW--
	//Depth map
	GLuint depthMapFBO;
	GLuint depthMapTextureId;
	glGenFramebuffers(1, &depthMapFBO);
	//use 2D texture for framebuffer's depth buffer
	glGenTextures(1, &depthMapTextureId);
	glBindTexture(GL_TEXTURE_2D, depthMapTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTextureId, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glm::vec3 lightPos (0.0f, 20.0f, 0.0f);

	while (!glfwWindowShouldClose(window)) {
		
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		GLuint currentProgram = texture ? textureProgramId : colorProgramId;
		glUseProgram(currentProgram);
		glUniform1i(glGetUniformLocation(currentProgram, "shadowMap"), 0);
		glUniform1i(glGetUniformLocation(currentProgram, "ourTexture"), 1);

		//render
		glClearColor(0, .4, .4, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Get uniform locations
		GLuint depthLightSpaceMatrix_addr = glGetUniformLocation(depthProgramId, "lightSpaceMatrix");
		GLuint depth_mm_addr = glGetUniformLocation(depthProgramId, "m_m");
		GLuint mm_addr = glGetUniformLocation(currentProgram, "m_m");
		GLuint vm_addr = glGetUniformLocation(currentProgram, "v_m");
		GLuint pm_addr = glGetUniformLocation(currentProgram, "p_m");
		GLuint lp_addr = glGetUniformLocation(currentProgram, "l_p");
		GLuint lightSpaceMatrix_addr = glGetUniformLocation(currentProgram, "lightSpaceMatrix");
		GLuint cp_addr = glGetUniformLocation(currentProgram, "c_p");
		GLuint color_addr = glGetUniformLocation(currentProgram, "color");

		// Calculate light matrices
		float near_plane = 1.0;
		float far_plane = 7.5;
		//glm::mat4 lightProjectionMatrix = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
		glm::mat4 lightProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightViewMatrix = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec4 lightPosition4 = glm::vec4(lightPos, 1.0);
		glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

		//--GALLOP--
		//headJoint->rotateDelta(20, true, 0.01);
	
		neckJoint->rotateDelta(20, true, deltaTime);		

		// 1st render to bind depth map to texture
		glUseProgram(depthProgramId);
		glUniformMatrix4fv(depthLightSpaceMatrix_addr, 1, false, glm::value_ptr(lightSpaceMatrix));
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, depthMapTextureId);
		world->draw(drawMode, depth_mm_addr, color_addr, false);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//reset
		glViewport(0, 0, windowWidth, windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// 2nd render using generated depth/shadow map
		glViewport(0, 0, windowWidth, windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(currentProgram);

		lightPosition4 = lightPosition4 * world->getModelMatrix();
		glUniform3fv(lp_addr, 1, glm::value_ptr(glm::vec3(lightPosition4.x, lightPosition4.y, lightPosition4.z)));
		glUniform3fv(cp_addr, 1, glm::value_ptr(camera->getCameraPosition()));
		glUniformMatrix4fv(lightSpaceMatrix_addr, 1, false, glm::value_ptr(lightSpaceMatrix));
		glUniformMatrix4fv(vm_addr, 1, false, glm::value_ptr(camera->getViewMatrix()));
		glUniformMatrix4fv(pm_addr, 1, false, glm::value_ptr(camera->getProjectionMatrix()));
		//glUniform1i(glGetUniformLocation(currentProgram, "shadowMap"), 0);
		//glUniform1i(glGetUniformLocation(currentProgram, "ourTexture"), 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTextureId);

		world->draw(drawMode, mm_addr, color_addr, false);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}