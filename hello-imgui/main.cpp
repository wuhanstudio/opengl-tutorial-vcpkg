//-----------------------------------------------------------------------------
// Textures_2.cpp by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// - Add another texture
// - fragment shader blending using GLSL mix()
//-----------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fmt/core.h>

#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Mesh.h"
#include "Skybox.h"

enum LightType
{
	POINT_LIGHT,
	SPOT_LIGHT
};

LightType lightType = SPOT_LIGHT;

const char* glsl_version = "#version 150";

// // Set to true to enable fullscreen
bool FULLSCREEN = false;

GLFWwindow* gWindow = NULL;
const char* APP_TITLE = "Introduction to Modern OpenGL - Hello ImGUI";

// Window dimensions
int gWindowWidth = 800;
int gWindowHeight = 600;

// Fullscreen dimensions
int gWindowWidthFull = 1920;
int gWindowHeightFull = 1200;

bool gWireframe = false;
bool gFlashlightOn = true;

// Camera orientation
double lastMouseX, lastMouseY;

double lastTime = glfwGetTime();
double deltaTime = 0.0;
bool draging = false;

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

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(gWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Load meshes and textures
	ShaderProgram spotLightShader;
	ShaderProgram pointLightShader;
	pointLightShader.loadShaders("shaders/lighting_phong_materials.vert", "shaders/lighting_phong_materials.frag");
	spotLightShader.loadShaders("shaders/lighting_spot.vert", "shaders/lighting_spot.frag");

	// Light shader
	ShaderProgram lightShader;
	lightShader.loadShaders("shaders/bulb.vert", "shaders/bulb.frag");

	// Shadow shader
	ShaderProgram shadowShader;
	shadowShader.loadShaders("shaders/shadow.vert", "shaders/shadow.frag");

	// Load meshes and textures
	const int numModels = 6;
	Mesh mesh[numModels];
	Texture2D texture[numModels];

	mesh[0].loadOBJ("models/crate.obj");
	mesh[1].loadOBJ("models/woodcrate.obj");
	mesh[2].loadOBJ("models/robot.obj");
	mesh[3].loadOBJ("models/floor.obj");
	mesh[4].loadOBJ("models/bowling_pin.obj");
	mesh[5].loadOBJ("models/bunny.obj");

	texture[0].loadTexture("textures/crate.jpg", true);
	texture[1].loadTexture("textures/woodcrate_diffuse.jpg", true);
	texture[2].loadTexture("textures/robot_diffuse.jpg", true);
	texture[3].loadTexture("textures/tile_floor.jpg", true);
	texture[4].loadTexture("textures/AMF.tga", true);
	texture[5].loadTexture("textures/bunny_diffuse.jpg", true);

	Mesh lightMesh;
	lightMesh.loadOBJ("models/light.obj");

	// Model positions
	glm::vec3 modelPos[] = {
		glm::vec3(-3.5f, 0.0f, 0.0f),	// crate1
		glm::vec3(3.5f, 0.0f, 0.0f),	// crate2
		glm::vec3(0.0f, 0.0f, -2.0f),	// robot
		glm::vec3(0.0f, 0.0f, 0.0f),	// floor
		glm::vec3(0.0f, 0.0f, 2.0f),	// pin
		glm::vec3(-2.0f, 0.0f, 2.0f)	// bunny
	};

	// Model scale
	glm::vec3 modelScale[] = {
		glm::vec3(1.0f, 1.0f, 1.0f),	// crate1
		glm::vec3(1.0f, 1.0f, 1.0f),	// crate2
		glm::vec3(1.0f, 1.0f, 1.0f),	// robot
		glm::vec3(10.0f, 1.0f, 10.0f),	// floor
		glm::vec3(0.1f, 0.1f, 0.1f),	// pin
		glm::vec3(0.7f, 0.7f, 0.7f)		// bunny
	};


	// Shadow
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Skybox
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

	lastTime = glfwGetTime();
	float angle = 0.0f;

	// Rendering loop
	while (!glfwWindowShouldClose(gWindow))
	{
		// Vsync - comment this out if you want to disable vertical sync
		glfwSwapInterval(0);

		showFPS(gWindow);

		double currentTime = glfwGetTime();
		double deltaLightTime = currentTime - lastTime;

		// Poll for and process events
		glfwPollEvents();

		if (glfwGetWindowAttrib(gWindow, GLFW_ICONIFIED) != 0)
		{
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			ImGui::Begin("Hello, ImGUI!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("Choose the light type.");               // Display some text (you can use a format strings too)
            ImGui::RadioButton("Point Light", reinterpret_cast<int*>(&lightType), 0); ImGui::SameLine();
            ImGui::RadioButton("Spot Light", reinterpret_cast<int*>(&lightType), 1);

			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			//static float f = 0.0f;
			//static int counter = 0;

			//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(gWindow, &display_w, &display_h);

		io.MouseDrawCursor = true;
		if (draging && (!io.WantCaptureMouse) ) {
			glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			io.MouseDrawCursor = false;
			update(deltaTime);
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1.0), view(1.0), projection(1.0);

		// Create the View matrix
		view = fpsCamera.getViewMatrix();

		// Create the projection matrix
		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f);

		// update the view (camera) position
		glm::vec3 viewPos;
		viewPos.x = fpsCamera.getPosition().x;
		viewPos.y = fpsCamera.getPosition().y;
		viewPos.z = fpsCamera.getPosition().z;

		glm::vec3 lightPos(0.0f, 5.0f, 10.0f);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

		// 1. render depth of scene to texture (from light's perspective)
		float near_plane = 1.0f, far_plane = 30.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		if (LightType::POINT_LIGHT == lightType)
		{
			// Pint Light
			lightPos = glm::vec3(0.0f, 5.0f, 10.0f);

			// Move the light
			angle += (float)deltaLightTime * 50.0f;
			lightPos.x = 8.0f * sinf(glm::radians(angle));  // slide back and forth
			lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

		}
		else if (LightType::SPOT_LIGHT == lightType)
		{
			// Spot Light
			lightPos = fpsCamera.getPosition();
			lightView = fpsCamera.getViewMatrix();
		}

		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		// Render the scene to the depth buffer (shadow map)
		shadowShader.use();
		shadowShader.setUniform("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Render the scene
		for (int i = 0; i < numModels; i++)
		{
			model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]);
			shadowShader.setUniform("model", model);

			texture[i].bind(0);		// set the texture before drawing.
			mesh[i].draw();
			texture[i].unbind(0);
		}

		if (LightType::POINT_LIGHT == lightType)
		{
			// Render the light bulb geometry
			model = glm::translate(glm::mat4(1.0), lightPos);
			lightShader.use();
			lightShader.setUniform("lightColor", lightColor);
			lightShader.setUniform("model", model);
			lightShader.setUniform("view", view);
			lightShader.setUniform("projection", projection);
			lightMesh.draw();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		if (FULLSCREEN)
			glViewport(0, 0, gWindowWidthFull, gWindowHeightFull);
		else
			glViewport(0, 0, gWindowWidth, gWindowHeight);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render the scene
		if (LightType::POINT_LIGHT == lightType)
		{
			pointLightShader.use();

			// Point light
			pointLightShader.setUniform("view", view);
			pointLightShader.setUniform("projection", projection);
			pointLightShader.setUniform("viewPos", viewPos);

			pointLightShader.setUniform("light.position", lightPos);
			pointLightShader.setUniform("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			pointLightShader.setUniform("light.diffuse", lightColor);
			pointLightShader.setUniform("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		
			pointLightShader.setUniform("lightSpaceMatrix", lightSpaceMatrix);

		}
		else if (LightType::SPOT_LIGHT == lightType)
		{
			spotLightShader.use();

			// Spot light
			glm::vec3 spotlightPos = fpsCamera.getPosition();
			spotlightPos.y -= 0.5f;

			spotLightShader.setUniform("view", view);
			spotLightShader.setUniform("projection", projection);
			spotLightShader.setUniform("viewPos", viewPos);

			spotLightShader.setUniform("spotLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			spotLightShader.setUniform("spotLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
			spotLightShader.setUniform("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
			spotLightShader.setUniform("spotLight.position", spotlightPos);
			spotLightShader.setUniform("spotLight.direction", fpsCamera.getLook());
			spotLightShader.setUniform("spotLight.cosInnerCone", glm::cos(glm::radians(15.0f)));
			spotLightShader.setUniform("spotLight.cosOuterCone", glm::cos(glm::radians(20.0f)));
			spotLightShader.setUniform("spotLight.constant", 1.0f);
			spotLightShader.setUniform("spotLight.linear", 0.07f);
			spotLightShader.setUniform("spotLight.exponent", 0.017f);
			spotLightShader.setUniform("spotLight.on", gFlashlightOn);

			spotLightShader.setUniform("lightSpaceMatrix", lightSpaceMatrix);
		}


		// Render the scene
		for (int i = 0; i < numModels; i++)
		{
			model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]);
			
			if (LightType::POINT_LIGHT == lightType)
			{
				pointLightShader.setUniform("model", model);

				// Set material properties
				pointLightShader.setUniform("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
				pointLightShader.setUniformSampler("material.diffuseMap", 0);
				pointLightShader.setUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
				pointLightShader.setUniform("material.shininess", 32.0f);
			}
			else if (LightType::SPOT_LIGHT == lightType)
			{
				spotLightShader.setUniform("model", model);

				// Set material properties
				spotLightShader.setUniform("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
				spotLightShader.setUniformSampler("material.diffuseMap", 0);
				spotLightShader.setUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
				spotLightShader.setUniform("material.shininess", 32.0f);
			}
			texture[i].bind(0);		// set the texture before drawing.

			// Render the shadow
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, depthMap);

			pointLightShader.setUniform("shadowMap", 2);
			spotLightShader.setUniform("shadowMap", 2);

			mesh[i].draw();			// Render the OBJ mesh
			texture[i].unbind(0);
		}

		if (LightType::POINT_LIGHT == lightType)
		{
			// Render the light bulb geometry
			model = glm::translate(glm::mat4(1.0), lightPos);
			lightShader.use();
			lightShader.setUniform("lightColor", lightColor);
			lightShader.setUniform("model", model);
			lightShader.setUniform("view", view);
			lightShader.setUniform("projection", projection);
			lightMesh.draw();
		}

		skybox.render(skyboxShader, view, projection);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap front and back buffers
		glfwSwapBuffers(gWindow);

		lastTime = currentTime;
	}

	texture[0].destroy();
	texture[1].destroy();
	texture[2].destroy();
	texture[3].destroy();
	texture[4].destroy();
	texture[5].destroy();

	mesh[0].destroy();
	mesh[1].destroy();
	mesh[2].destroy();
	mesh[3].destroy();
	mesh[4].destroy();
	mesh[5].destroy();
	lightMesh.destroy();

	pointLightShader.destroy();
	spotLightShader.destroy();
	shadowShader.destroy();
	skyboxShader.destroy();
	
	skybox.destroy();

	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	return 0;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (GLFW_PRESS == action)
		{
			draging = true;
			// Get the current mouse cursor position delta
			glfwGetCursorPos(gWindow, &lastMouseX, &lastMouseY);
		}
		else if (GLFW_RELEASE == action)
			draging = false;
	}

	lastTime = currentTime;
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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3

	// Create a window
	if (FULLSCREEN)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		gWindowWidthFull = mode->width;
		gWindowHeightFull = mode->height;
		gWindow = glfwCreateWindow(gWindowWidthFull, gWindowHeightFull, APP_TITLE, glfwGetPrimaryMonitor(), NULL);
	}
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
	glfwSetMouseButtonCallback(gWindow, mouse_button_callback);
	glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);
	glfwSetScrollCallback(gWindow, glfw_onMouseScroll);

	// Hides and grabs cursor, unlimited movement
	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	if (FULLSCREEN)
		glfwSetCursorPos(gWindow, gWindowWidthFull / 2.0, gWindowHeightFull / 2.0);
	else
		glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	return true;
}

//-----------------------------------------------------------------------------
// Is called whenever a key is pressed/released via GLFW
//-----------------------------------------------------------------------------
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

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		gFlashlightOn = !gFlashlightOn;
	}
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	gWindowWidth = width;
	gWindowHeight = height;
	glViewport(0, 0, gWindowWidth, gWindowHeight);
}

//-----------------------------------------------------------------------------
// Called by GLFW when the mouse wheel is rotated
//-----------------------------------------------------------------------------
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
{
	double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;

	fov = glm::clamp(fov, 1.0, 120.0);

	fpsCamera.setFOV((float)fov);
}

//-----------------------------------------------------------------------------
// Update stuff every frame
//-----------------------------------------------------------------------------
void update(double elapsedTime)
{
	// Camera orientation
	double mouseX, mouseY;

	// Get the current mouse cursor position delta
	glfwGetCursorPos(gWindow, &mouseX, &mouseY);

	// Rotate the camera the difference in mouse distance from the center screen.  Multiply this delta by a speed scaler
	fpsCamera.rotate((float)(lastMouseX - mouseX) * MOUSE_SENSITIVITY, (float)(lastMouseY - mouseY) * MOUSE_SENSITIVITY);

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

	glfwGetCursorPos(gWindow, &lastMouseX, &lastMouseY);
}

//-----------------------------------------------------------------------------
// Code computes the average frames per second, and also the average time it takes
// to render one frame.  These stats are appended to the window caption bar.
//-----------------------------------------------------------------------------
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
		std::snprintf(title, sizeof(title), "Hello ImGUI @ fps: %.2f, ms/frame: %.2f", fps, msPerFrame);
		glfwSetWindowTitle(window, title);

		frameCount = 0;
	}

	frameCount++;
}
