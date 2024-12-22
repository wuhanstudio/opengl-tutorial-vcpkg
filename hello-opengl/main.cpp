#include <iostream>
#include <fmt/core.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Set to true to enable fullscreen
bool FULLSCREEN = false;

GLFWwindow* gWindow = NULL;
const char* APP_TITLE = "Introduction to Modern OpenGL - Hello OpenGL";

// Window dimensions
const int gWindowWidth = 800;
const int gWindowHeight = 600;

// Fullscreen dimensions
const int gWindowWidthFull = 1920;
const int gWindowHeightFull = 1200;

// Press ESC to close the window
static void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

int main() {

	glfwInit();

	// Set the OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window
	if (FULLSCREEN)
		gWindow = glfwCreateWindow(gWindowWidthFull, gWindowHeightFull, APP_TITLE, glfwGetPrimaryMonitor(), NULL);
	else
		gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);

	if (gWindow == NULL) {
		fmt::print("Failed to create a window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(gWindow);

	// Set the callback function for ESC key
	glfwSetKeyCallback(gWindow, glfw_onKey);

	gladLoadGL();

	if (FULLSCREEN)
		glViewport(0, 0, gWindowWidthFull, gWindowHeightFull);
	else
		glViewport(0, 0, gWindowWidth, gWindowHeight);

	while (!glfwWindowShouldClose(gWindow))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glfwSwapBuffers(gWindow);

		glfwPollEvents();
	}

	glfwDestroyWindow(gWindow);
	glfwTerminate();

	return 0;
}
