#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
// #define LOCK_FRAMERATE
#ifdef LOCK_FRAMERATE
#include <thread>
#endif

#include "utils.h"

#include "shader.h"
#include "material.h"
#include "camera.h"

#include "line.h"
#include "tm.h"
#include "light.h"

constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;

#ifdef LOCK_FRAMERATE
constexpr unsigned int DESIRED_FRAME_RATE = 60;
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
	{
		glfwSetWindowShouldClose(window, true);
	}

	renderAxis = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;

	renderNormals = glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS;

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

void MouseCallback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	auto xPos = static_cast<float>(xPosIn);
	auto yPos = static_cast<float>(yPosIn);

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos; // reversed since y-coordinates go from bottom to top

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

	Tex2D blackEmissive(glm::vec3(0.0f));

	Tex2D emeraldDiffuse(   glm::vec3(0.0215f,   0.745f,    0.0215f  ));
	Tex2D emeraldSpecular(  glm::vec3(0.633f,    0.727811f, 0.633f   ));
	Tex2D jadeDiffuse(      glm::vec3(0.135f,    0.2225f,   0.1575f  ));
	Tex2D jadeSpecular(     glm::vec3(0.316228f, 0.316228f, 0.316228f));
	Tex2D obsidianDiffuse(  glm::vec3(0.05375f,  0.05f,     0.06625f ));
	Tex2D obsidianSpecular( glm::vec3(0.332741f, 0.328634f, 0.346435f));
	Tex2D pearlDiffuse(     glm::vec3(0.25f,     0.20725f,  0.20725f ));
	Tex2D pearlSpecular(    glm::vec3(0.296648f, 0.296648f, 0.296648f));
	Tex2D rubyDiffuse(      glm::vec3(0.1745f,   0.01175f,  0.01175f ));
	Tex2D rubySpecular(     glm::vec3(0.727811f, 0.626959f, 0.626959f));
	Tex2D turquoiseDiffuse( glm::vec3(0.1f,      0.18725f,  0.1745f  ));
	Tex2D turquoiseSpecular(glm::vec3(0.297254f, 0.30829f,  0.306678f));
	Tex2D brassDiffuse(     glm::vec3(0.329412f, 0.223529f, 0.027451f));
	Tex2D brassSpecular(    glm::vec3(0.992157f, 0.941176f, 0.807843f));
	Tex2D bronzeDiffuse(    glm::vec3(0.2125f,   0.1275f,   0.054f   ));
	Tex2D bronzeSpecular(   glm::vec3(0.393548f, 0.271906f, 0.166721f));
	Tex2D chromeDiffuse(    glm::vec3(0.25f,     0.25f,     0.25f    ));
	Tex2D chromeSpecular(   glm::vec3(0.774597f, 0.774597f, 0.774597f));
	Tex2D copperDiffuse(    glm::vec3(0.19125f,  0.0735f,   0.0225f  ));
	Tex2D copperSpecular(   glm::vec3(0.25677f,  0.137622f, 0.086014f));
	Tex2D goldDiffuse(      glm::vec3(0.24725f,  0.1995f,   0.0745f  ));
	Tex2D goldSpecular(     glm::vec3(0.628281f, 0.555802f, 0.366065f));
	Tex2D silverDiffuse(    glm::vec3(0.19225f,  0.19225f,  0.19225f ));
	Tex2D silverSpecular(   glm::vec3(0.508273f, 0.508273f, 0.508273f));

	std::unordered_map<Name, Material> materials = {
		{EMERALD,   Material(&emeraldDiffuse, &emeraldSpecular, &blackEmissive, 0.6f)},
		{JADE,      Material(&jadeDiffuse, &jadeSpecular, &blackEmissive,  0.1f)},
		{OBSIDIAN,  Material(&obsidianDiffuse, &obsidianSpecular, &blackEmissive,  0.3f)},
		{PEARL,     Material(&pearlDiffuse, &pearlSpecular, &blackEmissive,  0.088f)},
		{RUBY,      Material(&rubyDiffuse, &rubySpecular, &blackEmissive,  0.6f)},
		{TURQUOISE, Material(&turquoiseDiffuse, &turquoiseSpecular, &blackEmissive,  0.1f)},
		{BRASS,     Material(&brassDiffuse, &brassSpecular, &blackEmissive,  0.21794872f)},
		{BRONZE,    Material(&bronzeDiffuse, &bronzeSpecular, &blackEmissive,  0.2f)},
		{CHROME,    Material(&chromeDiffuse, &chromeSpecular, &blackEmissive,  0.6f)},
		{COPPER,    Material(&copperDiffuse, &copperSpecular, &blackEmissive,  0.1f)},
		{GOLD,      Material(&goldDiffuse, &goldSpecular, &blackEmissive,  0.4f)},
		{SILVER,    Material(&silverDiffuse, &silverSpecular, &blackEmissive,  0.4f)}
	};

	TexCube containerDiffuse(".\\textures\\container2.png");
	TexCube containerSpecular(".\\textures\\container2_specular.png");
	TexCube containerEmissive(".\\textures\\matrix.jpg");
	Material containerCube(&containerDiffuse, &containerSpecular, &containerEmissive, 0.5f);

	// Set up vertex data
	// ------------------
	Light light;
	light.SetAsAACube(glm::vec3(1.2f, 1.0f, 2.0f));
	light.m_KA = 0.2f;
	light.m_KD = 0.5f;
	light.m_KS = 1.0f;

	TriangleMesh cube(&containerCube, &light, &camera);
	cube.SetAsAACube();
	cube.m_DrawingMode = LIT;

	auto origin = glm::vec3(0.0f, 0.0f, 0.0f);

	Line xAxis(origin, glm::vec3(10.0f, 0.0f, 0.0f));
	Line yAxis(origin, glm::vec3(0.0f, 10.0f, 0.0f));
	Line zAxis(origin, glm::vec3(0.0f, 0.0f, 10.0f));

	glm::mat4 model, view, proj;
	auto identity = glm::mat4(1.0f);

	float frameCount = 0.0f;
	float currentFrame = 0.0f;
	bool firstRun = true;

	CheckForErrors("ERROR::SOURCE::MAIN: ");

	// Render Loop
	std::cout << "Starting render loop" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// std::cout << "FPS: " << frameCount / currentFrame << '\r';

		// Input
		ProcessInput(window);

		// Set matrices
		model = identity;
		view = camera.GetViewMatrix(); 
		proj = glm::perspective(glm::radians(camera.m_Zoom),
			static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);

		//light.m_Pos = glm::vec3(1.0f + sin(currentFrame), light.m_Pos.y, 1.0f + sin(currentFrame));
		
		// Render
		// ------
		// Set Background
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // RGBA
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (renderAxis)
		{
			xAxis.SetMVP(model, view, proj);
			yAxis.SetMVP(model, view, proj);
			zAxis.SetMVP(model, view, proj);

			xAxis.SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
			yAxis.SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
			zAxis.SetColor(glm::vec3(0.0f, 0.0f, 1.0f));

			xAxis.Draw();
			yAxis.Draw();
			zAxis.Draw();
		}

		// Render objects
		cube.SetMVP(model, view, proj);

		model = glm::scale(glm::translate(identity, light.m_Pos), glm::vec3(0.2f));
		light.SetMVP(model, view, proj);

		cube.Draw();
		light.Draw();

		if (renderNormals)
		{
			cube.DrawNormals(glm::vec3(1.0f, 0.0f, 1.0f));
			light.DrawNormals(glm::vec3(1.0f, 0.0f, 1.0f));
		}

		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		frameCount++;
#ifdef LOCK_FRAMERATE
		std::this_thread::sleep_for(std::chrono::nanoseconds((int) (frameCount / currentFrame / (float) DESIRED_FRAME_RATE * 1e7)));
#endif
		if (firstRun)
		{
			std::cout << "First run!" << std::endl;
			firstRun = false;
		}
	}

	glfwTerminate();
	return 0;
}
