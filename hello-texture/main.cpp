#include <iostream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <fmt/core.h>

#include "ShaderProgram.h"
#include "Texture2D.h"

// Set to true to enable fullscreen
bool FULLSCREEN = false;

GLFWwindow* gWindow = NULL;
const char* APP_TITLE = "Introduction to Modern OpenGL - Hello Texture";

// Window dimensions
const int gWindowWidth = 400;
const int gWindowHeight = 300;

// Fullscreen dimensions
const int gWindowWidthFull = 1920;
const int gWindowHeightFull = 1200;

bool gWireframe = false;

const std::string texture1Filename = "textures/crate.jpg";
const std::string texture2Filename = "textures/airplane.png";

// Function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void showFPS(GLFWwindow* window);
bool initOpenGL();

int main()
{
	if (!initOpenGL())
	{
		fmt::println("GLFW initialization failed");
		return -1;
	}

	// 1. Set up an array of vertices for a quad (2 triangls) with an index buffer data
	GLfloat vertices[] = {
		// position			 // texture coords
		-0.5f,  0.5f, 0.0f,	 0.0f, 1.0f,		// Top left
		 0.5f,  0.5f, 0.0f,	 1.0f, 1.0f,		// Top right
		 0.5f, -0.5f, 0.0f,	 1.0f, 0.0f,		// Bottom right
		-0.5f, -0.5f, 0.0f,	 0.0f, 0.0f			// Bottom left 
	};

	GLuint indices[] = {
		0, 1, 2,  // First Triangle
		0, 2, 3   // Second Triangle
	};

	// 2. Set up buffers on the GPU
	GLuint VAO, VBO, IBO;

	glGenVertexArrays(1, &VAO);				// Tell OpenGL to create new Vertex Array Object
	glGenBuffers(1, &VBO);					// Generate an empty vertex buffer on the GPU
	glGenBuffers(1, &IBO);					// Create buffer space on the GPU for the index buffer

	glBindVertexArray(VAO);					// Make it the current one
	glBindBuffer(GL_ARRAY_BUFFER, VBO);		// "bind" or set as the current buffer we are working with

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	// copy the data from CPU to GPU

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0));	// Define a layout for the first vertex buffer "0"
	glEnableVertexAttribArray(0);			// Enable the first attribute or attribute "0"

	// Texture Coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Set up index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);					// unbind to make sure other code doesn't change it

	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("shaders/basic.vert", "shaders/basic.frag");

	Texture2D texture1;
	texture1.loadTexture(texture1Filename);

	Texture2D texture2;
	texture2.loadTexture(texture2Filename);

	// Rendering loop
	while (!glfwWindowShouldClose(gWindow))
	{
		// Vsync - comment this out if you want to enable vertical sync
		glfwSwapInterval(0);

		showFPS(gWindow);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Render the quad
		// Must be called BEFORE setting uniforms because setting uniforms is done
		// on the currently active shader program.
		shaderProgram.use();

		// Wait!  Why didn't we call glUniform1i for the texture in Part 1?  The answer is OpenGL uses the 
		// default texture unit of 0 for the first texture so if you only have one texture sampler in the fragment
		// shader then you do not need to explicitly set the sampler by calling glUniform1i.

		// Ssetting uniform samplers
		texture1.bind(0);
		texture2.bind(1);
		glUniform1i(glGetUniformLocation(shaderProgram.getProgram(), "texSampler1"), 0);
		glUniform1i(glGetUniformLocation(shaderProgram.getProgram(), "texSampler2"), 1);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Swap front and back buffers
		glfwSwapBuffers(gWindow);
		glfwPollEvents();
	}

	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);

	texture1.destroy();
	texture2.destroy();
	shaderProgram.destroy();

	glfwTerminate();

	return 0;
}

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
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

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

	if (FULLSCREEN)
		glViewport(0, 0, gWindowWidthFull, gWindowHeightFull);
	else
		glViewport(0, 0, gWindowWidth, gWindowHeight);

	return true;
}

// Press ESC to close the window
// Press 1 to toggle wireframe mode
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
		std::snprintf(title, sizeof(title), "Hello Texture @ fps: %.2f, ms/frame: %.2f", fps, msPerFrame);
		glfwSetWindowTitle(window, title);

		frameCount = 0;
	}

	frameCount++;
}
