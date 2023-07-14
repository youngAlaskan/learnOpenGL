#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#define LOCK_FRAMERATE
#ifdef LOCK_FRAMERATE
#include <thread>
#endif

#include "utils.h"

#include "Shader.h"
#include "Material.h"
#include "Camera.h"

#include "Line.h"
#include "TM.h"
#include "Light.h"

#include "Model.h"

constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;

#ifdef LOCK_FRAMERATE
constexpr unsigned int DESIRED_FRAME_RATE = 6;
#endif

bool renderFilled = true;
bool renderAxis = false;
bool renderNormals = false;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

Camera camera(glm::vec3(1.0f, 1.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

enum Name {
	EMERALD, JADE, OBSIDIAN, PEARL, RUBY,
	TURQUOISE, BRASS, BRONZE, CHROME, COPPER,
	GOLD, SILVER
};

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	renderAxis = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;

	renderNormals = glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS;

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

void MouseCallback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	const auto xPos = static_cast<float>(xPosIn);
	const auto yPos = static_cast<float>(yPosIn);

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	const float xOffset = xPos - lastX;
	const float yOffset = lastY - yPos; // reversed since y-coordinates go from bottom to top

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yOffset));
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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	// Register window resizing callback
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	// Register mouse callback
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, MouseCallback);

	// Register scroll callback
	glfwSetScrollCallback(window, ScrollCallback);

	// Initialize GLAD
	// ---------------
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// glEnable(GL_DEBUG_OUTPUT);
	// glDebugMessageCallback(MessageCallback, 0);

	glEnable(GL_DEPTH_TEST);

	auto start = glfwGetTime();
	auto backpack = Model("C:\\Dev\\LearnOpenGL\\assets\\backpack.obj");
	auto end = glfwGetTime();
	std::cout << "Model load time: " << end - start << " seconds" << std::endl;

	auto origin = glm::vec3(0.0f, 0.0f, 0.0f);

	Line xAxis(origin, glm::vec3(10.0f,  0.0f,  0.0f));
	Line yAxis(origin, glm::vec3( 0.0f, 10.0f,  0.0f));
	Line zAxis(origin, glm::vec3( 0.0f,  0.0f, 10.0f));

	xAxis.SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
	yAxis.SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
	zAxis.SetColor(glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 model, view, proj;
	auto identity = glm::mat4(1.0f);

	float currentFrame = 0.0f;

	constexpr int frameHistoryCount = 10;
	auto frameRateHistory = std::vector<int>(frameHistoryCount, 0);

	// Render Loop
	std::cout << "Starting render loop" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		// Calculate Frame Rate
		currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		frameRateHistory.pop_back();
		frameRateHistory.insert(frameRateHistory.begin(), static_cast<int>(1.0f / deltaTime));
		int averageFrameRate = 0;
		for (int rate : frameRateHistory)
			averageFrameRate += rate;
		averageFrameRate /= static_cast<int>(frameRateHistory.size());
		glfwSetWindowTitle(window, ("LearnOpenGL FPS: " + std::to_string(averageFrameRate)).c_str());

		// Input
		ProcessInput(window);

		// Set matrices
		model = identity;
		view = camera.GetViewMatrix();
		proj = glm::perspective(glm::radians(camera.m_Zoom),
			static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);

		// Render
		// -------
		// Set Background
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // RGBA
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (renderAxis)
		{
			xAxis.SetMVP(model, view, proj);
			yAxis.SetMVP(model, view, proj);
			zAxis.SetMVP(model, view, proj);

			xAxis.Draw();
			yAxis.Draw();
			zAxis.Draw();
		}

		backpack.Draw();

		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		
#ifdef LOCK_FRAMERATE
		std::this_thread::sleep_for(std::chrono::nanoseconds(static_cast<int>(static_cast<float>(averageFrameRate) / static_cast<float>(DESIRED_FRAME_RATE) * 1e7f)));
#endif
	}

	glfwTerminate();
	return 0;
}
