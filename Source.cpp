#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "utils.h"

#include "shader.h"
#include "material.h"
#include "texture.h"
#include "camera.h"

#include "line.h"
#include "tm.h"
#include "light.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const unsigned int DESIRED_FRAME_RATE = 60;

bool renderFilled = true;
bool renderAxis = false;
bool renderNormals = false;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float mixPercent = 0.2f;
int materialIndex = 0;

Camera camera(glm::vec3(1.0f, 1.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	// TODO: Add delay to prevent flickering
	else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		renderFilled = !renderFilled;

		if (renderFilled)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixPercent += 0.0001f;
		mixPercent = mixPercent > 1.0f ? 1.0f : mixPercent;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixPercent -= 0.0001f;
		mixPercent = mixPercent < 0.0f ? 0.0f : mixPercent;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		materialIndex = materialIndex - 1 < 0 ? 0 : materialIndex - 1;
		std::cout << "Material changed to: " << materialIndex << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		materialIndex = materialIndex + 1 >= (int) materials.size() ? ((int) materials.size() == 0 ? 0 : (int) materials.size() - 1) : materialIndex + 1;
		std::cout << "Material changed to: " << materialIndex << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		renderAxis = !renderAxis;
	}
	else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		renderNormals = !renderNormals;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		camera.ProcessKeyboard(TOGGLE_FLY, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

int main()
{
	// Initialize GLFW to use OpenGL 4.6
	// ---------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// Create Window
	// -------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	// Register window resizing callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Register mouse callback
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	// Register scroll callback
	glfwSetScrollCallback(window, scroll_callback);

	// Initialize GLAD
	// ---------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// glEnable(GL_DEBUG_OUTPUT);
	// glDebugMessageCallback(MessageCallback, 0);

	glEnable(GL_DEPTH_TEST);

	// Set up vertex data
	// ------------------
	TriangleMesh cube;
	cube.SetAsAACube(materials[emerald]);

	Light light;
	light.SetAsAACube();

	light.setPos(glm::vec3(1.2f, 1.0f, 2.0f));
	light.setColor(glm::vec3(1.0f, 1.0f, 1.0f));

	glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);

	Line xAxis(origin, glm::vec3(10.0f, 0.0f, 0.0f));
	Line yAxis(origin, glm::vec3(0.0f, 10.0f, 0.0f));
	Line zAxis(origin, glm::vec3(0.0f, 0.0f, 10.0f));

	glm::mat4 model, view, proj;
	glm::mat4 identity = glm::mat4(1.0f);

	float frameCount = 0;
	bool firstRun = true;

	checkForErrors("ERROR::SOURCE::MAIN: ");

	// Render Loop
	std::cout << "Starting render loop" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float) glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// std::cout << "FPS: " << frameCount / currentFrame << '\r';

		// Input
		processInput(window);

		// Set matricies
		model = identity;
		view = camera.GetViewMatrix(); 
		proj = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);

		//light.m_Pos = glm::vec3(1.0f + sin(currentFrame), light.m_Pos.y, 1.0f + sin(currentFrame));
		
		// Render
		// ------
		// Set Background
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // RGBA
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (renderAxis)
		{
			xAxis.draw(identity, view, proj, glm::vec3(1.0f, 0.0f, 0.0f));
			yAxis.draw(identity, view, proj, glm::vec3(0.0f, 1.0f, 0.0f));
			zAxis.draw(identity, view, proj, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		// Render objects
		cube.setMVP(model, view, proj);
		cube.setMaterial(materials[materialIndex]);

		cube.draw(light, camera.Position);

		//model = glm::scale(glm::translate(identity, light.m_Pos), glm::vec3(0.2f));
		//light.setMVP(model, view, proj);
		
		//light.draw();

		if (renderNormals)
		{
			cube.drawNormals(glm::vec3(1.0f, 0.0f, 1.0f));
			light.drawNormals(glm::vec3(1.0f, 0.0f, 1.0f));
		}

		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		checkForErrors("ERROR::SOURCE::END_OF_LOOP: ");

		frameCount++;
		std::this_thread::sleep_for(std::chrono::nanoseconds((int) (frameCount / currentFrame / (float) DESIRED_FRAME_RATE * 1e7)));
		if (firstRun)
		{
			std::cout << "First run!" << std::endl;
			firstRun = false;
		}
	}

	glfwTerminate();
	return 0;
}
