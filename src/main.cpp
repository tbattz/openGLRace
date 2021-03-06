/*
 * main.cpp
 *
 *  Created on: 1Jan.,2017
 *      Author: bcub3d-desktop
 *
 *  Based on tutorials from http://www.learnopengl.com/#!Introduction
 */

// Standard Includes
#include <string>
#include <unistd.h>
#include <memory>

// GLEW (OpenGL Extension Wrangler Library)
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

// openGL Includes
#include "../src/shader.h"
#include "../src/camera.h"
#include "../src/model.h"
#include "../src/fonts.h"
#include "../src/light.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL Image loading library
#include <SOIL.h>

// Free Type Library
#include <ft2build.h>
#include FT_FREETYPE_H

using std::vector;

// Screen Properties
GLuint screenWidth = 1920, screenHeight = 1080;

// Camera View Setup
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Define Fonts based on OS
#ifdef _WIN32
	#define FONTPATH "C:/Windows/Fonts/Arial.ttf"
#elif __linux__
	#define FONTPATH "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf"
#endif

// X Position
GLfloat xval = 0.0f;

// Functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

int main(int argc, char* argv[]) {
	// Parse Command Line Arguments
	bool wireFrameOn = false;
	bool fpsOn = false;
	int opt;
	while((opt = getopt(argc, argv, "wf")) != -1) {
		switch(opt) {
		case 'w': wireFrameOn = true; break;
		case 'f': fpsOn = true; break;
		}
	}

	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE); // Set core profile
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth,screenHeight,"openGLRace",nullptr,nullptr);
	glfwMakeContextCurrent(window);

	// Setup Callbacks for user input
	glfwSetKeyCallback(window,key_callback);
	glfwSetCursorPosCallback(window,mouse_callback);
	glfwSetScrollCallback(window,scroll_callback);

	// Disable Cursor
	//glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

	// Initialise GLEW - setup OpenGL pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Set viewport size
	glViewport(0,0,screenWidth,screenHeight); // Origin is bottom left

	// Wireframe command line argument
	if(wireFrameOn) {
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	}

	// Test for objects in front of each other
	glEnable(GL_DEPTH_TEST);

	// Setup and compile shaders
	Shader lightingShader("../Shaders/multiple_lighting.vs","../Shaders/multiple_lighting.frag");

	// Load Models
	Model ourModel("../Models/wheel/wheelTest.obj");
	Model ground("../Models/wheel/ground.obj");

	// Load Lights
	DirectionalLight myDirLight({-0.2f,-1.0f,-0.3f}, {0.5f,0.5f,0.5f}, {0.4f,0.4f,0.4f}, {0.5f,0.5f,0.5f}, &lightingShader,0);
	//DirectionalLight myDirLight2({0.2f,-1.0f,0.3f}, {0.5f,0.5f,0.5f}, {0.4f,0.4f,0.4f}, {0.5f,0.5f,0.5f}, &lightingShader,1);
	//PointLight myPointLight({0.5f,1.0f,2.0f}, {0.5f,0.5f,0.5f}, {0.4f,0.4f,0.4f}, {0.5f,0.5f,0.5f}, 1.0f, 0.09f, 0.32f, &lightingShader,0);
	//PointLight myPointLight2({-0.5f,1.0f,-2.0f}, {0.5f,0.5f,0.5f}, {0.4f,0.4f,0.4f}, {0.5f,0.5f,0.5f}, 1.0f, 0.09f, 0.32f, &lightingShader,1);
	//SpotLight spotLight({0.5f,1.0f,2.0f}, {-0.5f,-1.0f,-2.0f}, {1.0f,1.0f,1.0f}, {0.4f,0.4f,0.4f}, {0.5f,0.5f,0.5f}, 1.0f, 0.09f, 0.32f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)), &lightingShader, 0);

	// Set number of lights
	glUniform1i(glGetUniformLocation(lightingShader.Program,"numLights.nDirLight"),1);
	glUniform1i(glGetUniformLocation(lightingShader.Program,"numLights.nPointLight"),0);
	glUniform1i(glGetUniformLocation(lightingShader.Program,"numLights.nSpotLight"),0);

	// Load Font
	GLFont* arialFontPt;
	Shader textShader = setupFontShader("../Shaders/font.vs", "../Shaders/font.frag",screenWidth,screenHeight);
	Shader* textShaderPt = &textShader;
	if(fpsOn) {
		//arialFontPt = new GLFont("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf");
		arialFontPt = new GLFont(FONTPATH);
		textShaderPt = new Shader(setupFontShader("../Shaders/font.vs", "../Shaders/font.frag",screenWidth,screenHeight));
	}

	// Game Loop
	while(!glfwWindowShouldClose(window)) {
		// Set Frame Time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check Events
		glfwPollEvents();
		do_movement();

		// Clear the colour buffer
		glClearColor(0.64f, 0.64f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.Use();

		// Update View Position Uniform
		GLint viewPosLoc              = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc,              camera.Position.x, camera.Position.y, camera.Position.z);

		// Transformation Matrices
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight,0.1f,100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program,"view"),1,GL_FALSE,glm::value_ptr(view));

		// Draw ground
		glm::mat4 model2;
		//model2 = glm::translate(model2, glm::vec3(0.0f,-1.75f, 0.0f)); // Translate down
		//model2 = glm::scale(model2, glm::vec3(0.2f, 0.2f, 0.2f)); // Scale to screen
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program,"model"),1,GL_FALSE,glm::value_ptr(model2));
		ground.Draw(lightingShader);

		// Draw Model
		glm::mat4 model;
		//model = glm::translate(model, glm::vec3(0.0f,-1.75f, xval)); // Translate down
		//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); // Scale to screen
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program,"model"),1,GL_FALSE,glm::value_ptr(model));
		ourModel.Draw(lightingShader);

		// Print FPS
		if(fpsOn) {
			std::stringstream ss;
			ss << int(1.0f/deltaTime) << " fps";
			arialFontPt->RenderText(textShaderPt,ss.str(),screenWidth-150.0f,screenHeight-50.0f,1.0f,glm::vec3(0.0f, 1.0f, 0.0f));
			//std::cout << (1.0f/deltaTime) << "fps" << "\r";
		}

		// Swap buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

// Smoothing key input callback
void key_callback(GLFWwindow* window,int key,int scancode, int action, int mode) {
	// ESC to close window
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window,GL_TRUE);
	}
	// Set key states
	if(action == GLFW_PRESS) {
		keys[key] = true;
	} else if (action == GLFW_RELEASE) {
		keys[key] = false;
	}
}

// Callback to position the camera
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// Reset mouse to center of window
	if(firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// Offset for smoother movement
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset,yoffset);
}

// Scrolling zoom callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		camera.ProcessMouseScroll(yoffset);
}

// Move camera using wasd keys
void do_movement() {
	// Camera Controls
	if(keys[GLFW_KEY_W]) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if(keys[GLFW_KEY_S]) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if(keys[GLFW_KEY_A]) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if(keys[GLFW_KEY_D]) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if(keys[GLFW_KEY_UP]) {
		xval += 0.05f;
	}
	if(keys[GLFW_KEY_DOWN]) {
		xval -= 0.05f;
	}
}
