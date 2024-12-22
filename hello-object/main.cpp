//-----------------------------------------------------------------------------
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//-----------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fmt/core.h>

#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Mesh.h"
#include "Skybox.h"

// Set to true to enable fullscreen
bool FULLSCREEN = false;

GLFWwindow* gWindow = NULL;
const char* APP_TITLE = "Introduction to Modern OpenGL - Hello Object";

// Window dimensions
const int gWindowWidth = 800;
const int gWindowHeight = 600;

// Fullscreen dimensions
const int gWindowWidthFull = 1920;
const int gWindowHeightFull = 1200;

bool gWireframe = false;

FPSCamera fpsCamera(glm::vec3(0.0f, 5.0f, 10.0f), -180, -10);
const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 5.0; // units per second
const float MOUSE_SENSITIVITY = 0.1f;

// Function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);
void update(double elapsedTime);
void showFPS(GLFWwindow* window);
bool initOpenGL();

//-----------------------------------------------------------------------------
// Main Application Entry Point
//-----------------------------------------------------------------------------
int main()
{
	if (!initOpenGL())
	{
		fmt::println("GLFW initialization failed");
		return -1;
	}

	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("shaders/basic.vert", "shaders/basic.frag");

	// Load meshes and textures
	const int numModels = 4;
	Mesh mesh[numModels];
	Texture2D texture[numModels];

	mesh[0].loadOBJ("models/crate.obj");
	mesh[1].loadOBJ("models/woodcrate.obj");
	mesh[2].loadOBJ("models/robot.obj");
	mesh[3].loadOBJ("models/floor.obj");

	texture[0].loadTexture("textures/crate.jpg", true);
	texture[1].loadTexture("textures/woodcrate_diffuse.jpg", true);
	texture[2].loadTexture("textures/robot_diffuse.jpg", true);
	texture[3].loadTexture("textures/tile_floor.jpg", true);

	// Model positions
	glm::vec3 modelPos[] = {
		glm::vec3(-2.5f, 1.0f, 0.0f),	// crate1
		glm::vec3(2.5f, 1.0f, 0.0f),	// crate2
		glm::vec3(0.0f, 0.0f, -2.0f),	// robot
		glm::vec3(0.0f, 0.0f, 0.0f)		// floor
	};

	// Model scale
	glm::vec3 modelScale[] = {
		glm::vec3(1.0f, 1.0f, 1.0f),	// crate1
		glm::vec3(1.0f, 1.0f, 1.0f),	// crate2
		glm::vec3(1.0f, 1.0f, 1.0f),	// robot
		glm::vec3(10.0f, 1.0f, 10.0f)	// floor
	};

	ShaderProgram skyboxShader;
	skyboxShader.loadShaders("shaders/skybox.vert", "shaders/skybox.frag");

	skyboxShader.use();
	skyboxShader.setUniform("skybox", 0);

	Skybox skybox({
		"textures/skybox/right.jpg",
		"textures/skybox/left.jpg",
		"textures/skybox/top.jpg",
		"textures/skybox/bottom.jpg",
		"textures/skybox/front.jpg",
		"textures/skybox/back.jpg"
		});

	double lastTime = glfwGetTime();

	// Rendering loop
	while (!glfwWindowShouldClose(gWindow))
	{
		// Vsync - comment this out if you want to disable vertical sync
		glfwSwapInterval(0);

		showFPS(gWindow);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		update(deltaTime);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1.0), view(1.0), projection(1.0);

		// Create the View matrix
		view = fpsCamera.getViewMatrix();

		// Create the projection matrix
		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f);

		// Render the scene
		// Must be called BEFORE setting uniforms because setting uniforms is done
		// on the currently active shader program.
		shaderProgram.use();

		// Pass the matrices to the shader
		shaderProgram.setUniform("model", model);
		shaderProgram.setUniform("view", view);
		shaderProgram.setUniform("projection", projection);

		// Render the scene
		for (int i = 0; i < numModels; i++)
		{
			model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]);
			shaderProgram.setUniform("model", model);

			texture[i].bind(0);		// set the texture before drawing.  Our simple OBJ mesh loader does not do materials yet.
			mesh[i].draw();			// Render the OBJ mesh
			texture[i].unbind(0);
		}

		// Render the skybox
		skybox.render(skyboxShader, view, projection);

		// Swap front and back buffers
		glfwSwapBuffers(gWindow);
		glfwPollEvents();

		lastTime = currentTime;
	}

	texture[0].destroy();
	texture[1].destroy();
	texture[2].destroy();
	texture[3].destroy();

	mesh[0].destroy();
	mesh[1].destroy();
	mesh[2].destroy();
	mesh[3].destroy();

	shaderProgram.destroy();
	skyboxShader.destroy();

	glfwTerminate();

	return 0;
}

//-----------------------------------------------------------------------------
// Initialize GLFW and OpenGL
//-----------------------------------------------------------------------------
bool initOpenGL()
{
	// Intialize GLFW 
	if (!glfwInit())
	{
		fmt::println("GLFW initialization failed");
		return false;
	}

	// Set the OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3

	// Create a window
	if (FULLSCREEN)
		gWindow = glfwCreateWindow(gWindowWidthFull, gWindowHeightFull, APP_TITLE, glfwGetPrimaryMonitor(), NULL);
	else
		gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);

	if (gWindow == NULL)
	{
		fmt::println("Failed to create GLFW window");
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(gWindow);

	gladLoadGL();

	// Set the required callback functions
	glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);
	glfwSetScrollCallback(gWindow, glfw_onMouseScroll);

	// Hides and grabs cursor, unlimited movement
	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

	if (FULLSCREEN)
		glViewport(0, 0, gWindowWidthFull, gWindowHeightFull);
	else
		glViewport(0, 0, gWindowWidth, gWindowHeight);

	glEnable(GL_DEPTH_TEST);

	return true;
}

// Is called whenever a key is pressed/released via GLFW
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

// Is called when the window is resized
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Called by GLFW when the mouse wheel is rotated
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
{
	double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;

	fov = glm::clamp(fov, 1.0, 120.0);

	fpsCamera.setFOV((float)fov);
}

// Update the camera every frame
void update(double elapsedTime)
{
	// Camera orientation
	double mouseX, mouseY;

	// Get the current mouse cursor position delta
	glfwGetCursorPos(gWindow, &mouseX, &mouseY);

	// Rotate the camera the difference in mouse distance from the center screen.  Multiply this delta by a speed scaler
	fpsCamera.rotate((float)(gWindowWidth / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(gWindowHeight / 2.0 - mouseY) * MOUSE_SENSITIVITY);

	// Clamp mouse cursor to center of screen
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	// Camera FPS movement

	// Forward/backward
	if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook());
	else if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getLook());

	// Strafe left/right
	if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getRight());
	else if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getRight());

	// Up/down
	if (glfwGetKey(gWindow, GLFW_KEY_Q) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getUp());
	else if (glfwGetKey(gWindow, GLFW_KEY_E) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getUp());
}

void showFPS(GLFWwindow* window) {
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime();

	elapsedSeconds = currentSeconds - previousSeconds;

	if (elapsedSeconds > 0.25) {
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		char title[80];
		std::snprintf(title, sizeof(title), "Hello Object @ fps: %.2f, ms/frame: %.2f", fps, msPerFrame);
		glfwSetWindowTitle(window, title);

		frameCount = 0;
	}

	frameCount++;
}
