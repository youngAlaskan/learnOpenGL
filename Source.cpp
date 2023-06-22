#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"
#include "texture.h"
#include "camera.h"

#include "line.h"
#include "tm.h"
#include "light.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool renderFilled = true;
bool renderAxis = false;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float mixPercent = 0.2f;

Camera camera(glm::vec3(1.0f, 1.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

glm::vec3 cameraPos = glm::vec3(1.0f, 1.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(-1.0f, -1.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
	else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		renderAxis = !renderAxis;
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

	glEnable(GL_DEPTH_TEST);

	// Set up vertex data
	// ------------------
	TriangleMesh cube;
	cube.SetAsAACube();

	Light light;
	light.SetAsAACube();

	glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);

	Line xAxis(origin, glm::vec3(10.0f, 0.0f, 0.0f));
	Line yAxis(origin, glm::vec3(0.0f, 10.0f, 0.0f));
	Line zAxis(origin, glm::vec3(0.0f, 0.0f, 10.0f));

	glm::mat4 model, view, proj;

	// Render Loop
	std::cout << "Starting render loop" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float) glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//std::cout << "Up time: " << currentFrame << 's' << '\r';

		// Input
		processInput(window);

		// Set matricies
		model = glm::mat4(1.0f);
		view = camera.GetViewMatrix(); 
		proj = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);

		// Render
		// ------
		// Set Background
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // RGBA
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (renderAxis)
		{
			xAxis.draw(glm::mat4(1.0f), view, proj, glm::vec3(1.0f, 0.0f, 0.0f));
			yAxis.draw(glm::mat4(1.0f), view, proj, glm::vec3(0.0f, 1.0f, 0.0f));
			zAxis.draw(glm::mat4(1.0f), view, proj, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		// Render objects
		cube.setMVP(model, view, proj);

		// cube.draw(lightPos, camera.Position);

		cube.drawNormals(glm::vec3(1.0f, 0.0f, 1.0f));

		model = glm::scale(glm::translate(glm::mat4(1.0f), lightPos), glm::vec3(0.2f));
		light.setMVP(model, view, proj);
		
		// light.draw();

		light.drawNormals(glm::vec3(1.0f, 0.0f, 1.0f));

		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
