#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
// #define LOCK_FRAMERATE
#ifdef LOCK_FRAMERATE
#include <thread>
#endif

#include "utils.h"
#include "Scene/Scene.h"

#include "Shader.h"
#include "Material.h"
#include "Camera.h"

#include "Line.h"
#include "TM.h"
#include "Light.h"

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

	auto blackEmissive = TexCube(glm::vec3(0.0f));

	auto emeraldDiffuse = TexCube(glm::vec3(0.0215f, 0.745f, 0.0215f));
	auto emeraldSpecular = TexCube(glm::vec3(0.633f, 0.727811f, 0.633f));
	auto jadeDiffuse = TexCube(glm::vec3(0.135f, 0.2225f, 0.1575f));
	auto jadeSpecular = TexCube(glm::vec3(0.316228f, 0.316228f, 0.316228f));
	auto obsidianDiffuse = TexCube(glm::vec3(0.05375f, 0.05f, 0.06625f));
	auto obsidianSpecular = TexCube(glm::vec3(0.332741f, 0.328634f, 0.346435f));
	auto pearlDiffuse = TexCube(glm::vec3(0.25f, 0.20725f, 0.20725f));
	auto pearlSpecular = TexCube(glm::vec3(0.296648f, 0.296648f, 0.296648f));
	auto rubyDiffuse = TexCube(glm::vec3(0.1745f, 0.01175f, 0.01175f));
	auto rubySpecular = TexCube(glm::vec3(0.727811f, 0.626959f, 0.626959f));
	auto turquoiseDiffuse = TexCube(glm::vec3(0.1f, 0.18725f, 0.1745f));
	auto turquoiseSpecular = TexCube(glm::vec3(0.297254f, 0.30829f, 0.306678f));
	auto brassDiffuse = TexCube(glm::vec3(0.329412f, 0.223529f, 0.027451f));
	auto brassSpecular = TexCube(glm::vec3(0.992157f, 0.941176f, 0.807843f));
	auto bronzeDiffuse = TexCube(glm::vec3(0.2125f, 0.1275f, 0.054f));
	auto bronzeSpecular = TexCube(glm::vec3(0.393548f, 0.271906f, 0.166721f));
	auto chromeDiffuse = TexCube(glm::vec3(0.25f, 0.25f, 0.25f));
	auto chromeSpecular = TexCube(glm::vec3(0.774597f, 0.774597f, 0.774597f));
	auto copperDiffuse = TexCube(glm::vec3(0.19125f, 0.0735f, 0.0225f));
	auto copperSpecular = TexCube(glm::vec3(0.25677f, 0.137622f, 0.086014f));
	auto goldDiffuse = TexCube(glm::vec3(0.24725f, 0.1995f, 0.0745f));
	auto goldSpecular = TexCube(glm::vec3(0.628281f, 0.555802f, 0.366065f));
	auto silverDiffuse = TexCube(glm::vec3(0.19225f, 0.19225f, 0.19225f));
	auto silverSpecular = TexCube(glm::vec3(0.508273f, 0.508273f, 0.508273f));

	auto emerald = Material(std::make_shared<TexCube>(emeraldDiffuse), std::make_shared<TexCube>(emeraldSpecular), std::make_shared<TexCube>(blackEmissive), 0.6f);
	auto jade = Material(std::make_shared<TexCube>(jadeDiffuse), std::make_shared<TexCube>(jadeSpecular), std::make_shared<TexCube>(blackEmissive), 0.1f);
	auto obsidian = Material(std::make_shared<TexCube>(obsidianDiffuse), std::make_shared<TexCube>(obsidianSpecular), std::make_shared<TexCube>(blackEmissive), 0.3f);
	auto pearl = Material(std::make_shared<TexCube>(pearlDiffuse), std::make_shared<TexCube>(pearlSpecular), std::make_shared<TexCube>(blackEmissive), 0.088f);
	auto ruby = Material(std::make_shared<TexCube>(rubyDiffuse), std::make_shared<TexCube>(rubySpecular), std::make_shared<TexCube>(blackEmissive), 0.6f);
	auto turquoise = Material(std::make_shared<TexCube>(turquoiseDiffuse), std::make_shared<TexCube>(turquoiseSpecular), std::make_shared<TexCube>(blackEmissive), 0.1f);
	auto brass = Material(std::make_shared<TexCube>(brassDiffuse), std::make_shared<TexCube>(brassSpecular), std::make_shared<TexCube>(blackEmissive), 0.21794872f);
	auto bronze = Material(std::make_shared<TexCube>(bronzeDiffuse), std::make_shared<TexCube>(bronzeSpecular), std::make_shared<TexCube>(blackEmissive), 0.2f);
	auto chrome = Material(std::make_shared<TexCube>(chromeDiffuse), std::make_shared<TexCube>(chromeSpecular), std::make_shared<TexCube>(blackEmissive), 0.6f);
	auto copper = Material(std::make_shared<TexCube>(copperDiffuse), std::make_shared<TexCube>(copperSpecular), std::make_shared<TexCube>(blackEmissive), 0.1f);
	auto gold = Material(std::make_shared<TexCube>(goldDiffuse), std::make_shared<TexCube>(goldSpecular), std::make_shared<TexCube>(blackEmissive), 0.4f);
	auto silver = Material(std::make_shared<TexCube>(silverDiffuse), std::make_shared<TexCube>(silverSpecular), std::make_shared<TexCube>(blackEmissive), 0.4f);

	auto containerDiffuse = TexCube(".\\textures\\container2.png");
	auto containerSpecular = TexCube(".\\textures\\container2_specular.png");
	auto containerEmissive = blackEmissive;
	Material containerCube(std::make_shared<TexCube>(containerDiffuse), std::make_shared<TexCube>(containerSpecular), std::make_shared<TexCube>(containerEmissive), 0.5f);

	// Set up vertex data
	// ------------------
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	Scene scene;
	auto pointLightComponents = std::vector<std::shared_ptr<PointLight>>();

	// Point Light
	// ------------------
	auto pointLight = scene.m_Registry.create();

	auto& pointLightComponent = scene.m_Registry.emplace<PointLight>(pointLight, 100.0f);
	// pointLight->SetAsAACube(glm::vec3(1.2f, 1.0f, 2.0f));
	pointLightComponent.m_KA = 0.2f;
	pointLightComponent.m_KD = 0.5f;
	pointLightComponent.m_KS = 1.0f;
	pointLightComponent.m_Index = 0;
	pointLightComponents.emplace_back(std::make_shared<PointLight>(pointLightComponent));

	auto& pointLightMeshComponent = scene.m_Registry.emplace<TriangleMesh>(pointLight);
	pointLightMeshComponent.SetAsAACube();

	// Directional Light

	auto directionalLight = scene.m_Registry.create();
	auto& directionalLightComponent = scene.m_Registry.emplace<DirectionalLight>(directionalLight);
	directionalLightComponent.m_KA = 0.2f;
	directionalLightComponent.m_KD = 0.5f;
	directionalLightComponent.m_KS = 1.0f;
	directionalLightComponent.m_Direction = glm::normalize(glm::vec3(1.0, -1.0, 1.0));

	auto flashlight = scene.m_Registry.create();
	auto& spotLightComponent = scene.m_Registry.emplace<SpotLight>(flashlight, 12.5f);
	spotLightComponent.m_KA = 0.2f;
	spotLightComponent.m_KD = 0.5f;
	spotLightComponent.m_KS = 1.0f;

	// auto cube = new TriangleMesh(&containerCube, flashlight, &camera);
	// cube->SetAsAACube();
	// cube->m_DrawingMode = LIT_BY_SPOTLIGHT;
	// entities.emplace_back(cube);

	std::vector<TriangleMesh&> cubeMeshComponents;

	for (int i = 0; i < 10; i++)
	{
		auto cube = scene.m_Registry.create();
		auto& triangleMeshComponent = scene.m_Registry.emplace<TriangleMesh>(cube, std::make_shared<Material>(containerCube), pointLightComponents, directionalLightComponent, spotLightComponent, std::make_shared<Camera>(camera));
		triangleMeshComponent.SetAsAACube();
		triangleMeshComponent.m_DrawingMode = LIT;
		cubeMeshComponents.emplace_back(triangleMeshComponent);
	}

	auto origin = glm::vec3(0.0f, 0.0f, 0.0f);

	auto xAxis = scene.m_Registry.create();
	auto yAxis = scene.m_Registry.create();
	auto zAxis = scene.m_Registry.create();

	auto& xAxisLineComponent = scene.m_Registry.emplace<Line>(xAxis, origin, glm::vec3(10.0f, 0.0f, 0.0f));
	auto& yAxisLineComponent = scene.m_Registry.emplace<Line>(yAxis, origin, glm::vec3(0.0f, 10.0f, 0.0f));
	auto& zAxisLineComponent = scene.m_Registry.emplace<Line>(zAxis, origin, glm::vec3(0.0f, 0.0f, 10.0f));

	xAxisLineComponent.SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
	yAxisLineComponent.SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
	zAxisLineComponent.SetColor(glm::vec3(0.0f, 0.0f, 1.0f));

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
		//std::cout << "FPS: " << frameCount / currentFrame << '\r';

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
			xAxisLineComponent.SetMVP(model, view, proj);
			yAxisLineComponent.SetMVP(model, view, proj);
			zAxisLineComponent.SetMVP(model, view, proj);

			xAxisLineComponent.Draw();
			yAxisLineComponent.Draw();
			zAxisLineComponent.Draw();
		}

		// Render objects
		for (int i = 0; i < 10; i++)
		{
			model = glm::translate(identity, cubePositions[i]);
			float angle = 20.0f * static_cast<float>(i);
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			cubeMeshComponents.at(i).SetMVP(model, view, proj);
		}

		model = glm::scale(glm::translate(identity, pointLight->m_Pos), glm::vec3(0.2f));
		pointLightMeshComponent.SetMVP(model, view, proj);

		spotLightComponent.m_Pos = camera.m_Position;
		spotLightComponent.m_Direction = camera.m_Front;

		// TODO: Draw stuff

		if (renderNormals)
		{
			//TODO: Render Normals
		}

		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		frameCount++;
#ifdef LOCK_FRAMERATE
		std::this_thread::sleep_for(std::chrono::nanoseconds((int)(frameCount / currentFrame / (float)DESIRED_FRAME_RATE * 1e7)));
#endif
		if (firstRun)
		{
			std::cout << "First run!" << std::endl;
			firstRun = false;
		}
	}

	for (Entity* entity : entities)
	{
		delete entity;
	}

	glfwTerminate();
	return 0;
}
