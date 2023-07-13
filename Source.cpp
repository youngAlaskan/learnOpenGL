#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
//#define LOCK_FRAMERATE
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

	auto blackEmissive = std::make_shared<TexCube>(glm::vec3(0.0f));
	blackEmissive->SetTagEmissive();

	// TODO: Add tags to the below items
	// auto emeraldDiffuse = std::make_shared<TexCube>(glm::vec3(0.0215f, 0.745f, 0.0215f));
	// auto emeraldSpecular = std::make_shared<TexCube>(glm::vec3(0.633f, 0.727811f, 0.633f));
	// auto emeraldTextures = std::vector<std::shared_ptr<Texture>>{emeraldDiffuse, emeraldSpecular};
	// auto jadeDiffuse = std::make_shared<TexCube>(glm::vec3(0.135f, 0.2225f, 0.1575f));
	// auto jadeSpecular = std::make_shared<TexCube>(glm::vec3(0.316228f, 0.316228f, 0.316228f));
	// auto jadeTextures = std::vector<std::shared_ptr<Texture>>{ jadeDiffuse, jadeSpecular, blackEmissive };
	// auto obsidianDiffuse = std::make_shared<TexCube>(glm::vec3(0.05375f, 0.05f, 0.06625f));
	// auto obsidianSpecular = std::make_shared<TexCube>(glm::vec3(0.332741f, 0.328634f, 0.346435f));
	// auto obsidianTextures = std::vector<std::shared_ptr<Texture>>{ obsidianDiffuse, obsidianSpecular, blackEmissive };
	// auto pearlDiffuse = std::make_shared<TexCube>(glm::vec3(0.25f, 0.20725f, 0.20725f));
	// auto pearlSpecular = std::make_shared<TexCube>(glm::vec3(0.296648f, 0.296648f, 0.296648f));
	// auto pearlTextures = std::vector<std::shared_ptr<Texture>>{ pearlDiffuse, pearlSpecular, blackEmissive };
	// auto rubyDiffuse = std::make_shared<TexCube>(glm::vec3(0.1745f, 0.01175f, 0.01175f));
	// auto rubySpecular = std::make_shared<TexCube>(glm::vec3(0.727811f, 0.626959f, 0.626959f));
	// auto rubyTextures = std::vector<std::shared_ptr<Texture>>{ rubyDiffuse, rubySpecular, blackEmissive };
	// auto turquoiseDiffuse = std::make_shared<TexCube>(glm::vec3(0.1f, 0.18725f, 0.1745f));
	// auto turquoiseSpecular = std::make_shared<TexCube>(glm::vec3(0.297254f, 0.30829f, 0.306678f));
	// auto turquoiseTextures = std::vector<std::shared_ptr<Texture>>{ turquoiseDiffuse, turquoiseSpecular, blackEmissive };
	// auto brassDiffuse = std::make_shared<TexCube>(glm::vec3(0.329412f, 0.223529f, 0.027451f));
	// auto brassSpecular = std::make_shared<TexCube>(glm::vec3(0.992157f, 0.941176f, 0.807843f));
	// auto brassTextures = std::vector<std::shared_ptr<Texture>>{ brassDiffuse, brassSpecular, blackEmissive };
	// auto bronzeDiffuse = std::make_shared<TexCube>(glm::vec3(0.2125f, 0.1275f, 0.054f));
	// auto bronzeSpecular = std::make_shared<TexCube>(glm::vec3(0.393548f, 0.271906f, 0.166721f));
	// auto bronzeTextures = std::vector<std::shared_ptr<Texture>>{ bronzeDiffuse, bronzeSpecular, blackEmissive };
	// auto chromeDiffuse = std::make_shared<TexCube>(glm::vec3(0.25f, 0.25f, 0.25f));
	// auto chromeSpecular = std::make_shared<TexCube>(glm::vec3(0.774597f, 0.774597f, 0.774597f));
	// auto chromeTextures = std::vector<std::shared_ptr<Texture>>{ chromeDiffuse, chromeSpecular, blackEmissive };
	// auto copperDiffuse = std::make_shared<TexCube>(glm::vec3(0.19125f, 0.0735f, 0.0225f));
	// auto copperSpecular = std::make_shared<TexCube>(glm::vec3(0.25677f, 0.137622f, 0.086014f));
	// auto copperTextures = std::vector<std::shared_ptr<Texture>>{ copperDiffuse, copperSpecular, blackEmissive };
	// auto goldDiffuse = std::make_shared<TexCube>(glm::vec3(0.24725f, 0.1995f, 0.0745f));
	// auto goldSpecular = std::make_shared<TexCube>(glm::vec3(0.628281f, 0.555802f, 0.366065f));
	// auto goldTextures = std::vector<std::shared_ptr<Texture>>{ goldDiffuse, goldSpecular, blackEmissive };
	// auto silverDiffuse = std::make_shared<TexCube>(glm::vec3(0.19225f, 0.19225f, 0.19225f));
	// auto silverSpecular = std::make_shared<TexCube>(glm::vec3(0.508273f, 0.508273f, 0.508273f));
	// auto silverTextures = std::vector<std::shared_ptr<Texture>>{ silverDiffuse, silverSpecular, blackEmissive };
	// 
	// auto emerald = Material(emeraldTextures, 0.6f);
	// auto jade = Material(jadeTextures, 0.1f);
	// auto obsidian = Material(obsidianTextures, 0.3f);
	// auto pearl = Material(pearlTextures, 0.088f);
	// auto ruby = Material(rubyTextures, 0.6f);
	// auto turquoise = Material(turquoiseTextures, 0.1f);
	// auto brass = Material(brassTextures, 0.21794872f);
	// auto bronze = Material(bronzeTextures, 0.2f);
	// auto chrome = Material(chromeTextures, 0.6f);
	// auto copper = Material(copperTextures, 0.1f);
	// auto gold = Material(goldTextures, 0.4f);
	// auto silver = Material(silverTextures, 0.4f);

	auto containerDiffuse = std::make_shared<TexCube>(".\\textures\\container2.png");
	containerDiffuse->SetTagDiffuse();
	auto containerSpecular = std::make_shared<TexCube>(".\\textures\\container2_specular.png");
	containerSpecular->SetTagSpecular();
	auto containerTextures = std::vector<std::shared_ptr<Texture>>{ containerDiffuse, containerSpecular, blackEmissive };
	auto containerCube = Material(containerTextures, 0.5f);

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

	std::vector<std::shared_ptr<TriangleMesh>> meshes;
	std::vector<std::shared_ptr<PointLight>> pointLights;

	auto pointLight = std::make_shared<PointLight>(100.0f);
	pointLight->m_KA = 0.2f;
	pointLight->m_KD = 0.5f;
	pointLight->m_KS = 1.0f;
	pointLight->m_Index = 0;
	pointLight->m_Pos = glm::vec3(1.2f, 1.0f, 2.0f);
	pointLights.emplace_back(pointLight);

	auto pointLightMesh = std::make_shared<TriangleMesh>();
	pointLightMesh->SetAsAACube(pointLight->m_Color);
	meshes.emplace_back(pointLightMesh);

	auto directionalLight = std::make_shared<DirectionalLight>();
	directionalLight->m_KA = 0.2f;
	directionalLight->m_KD = 0.5f;
	directionalLight->m_KS = 1.0f;
	directionalLight->m_Direction = glm::normalize(glm::vec3(1.0, -1.0, 1.0));

	auto flashlight = std::make_shared<SpotLight>(12.5f);
	flashlight->m_KA = 0.2f;
	flashlight->m_KD = 0.5f;
	flashlight->m_KS = 1.0f;

	// auto cube = new TriangleMesh(&containerCube, flashlight, &camera);
	// cube->SetAsAACube();
	// cube->m_DrawingMode = LIT_BY_SPOTLIGHT;
	// entities.emplace_back(cube);

	for (int i = 0; i < 10; i++)
	{
		auto cube = std::make_shared<TriangleMesh>(std::make_shared<Material>(containerCube), pointLights, directionalLight, flashlight, std::make_shared<Camera>(camera));
		cube->SetAsAACube();
		cube->m_DrawingMode = LIT_CUBE;
		meshes.emplace_back(cube);
	}

	auto origin = glm::vec3(0.0f, 0.0f, 0.0f);

	Line xAxis(origin, glm::vec3(10.0f, 0.0f, 0.0f));
	Line yAxis(origin, glm::vec3(0.0f, 10.0f, 0.0f));
	Line zAxis(origin, glm::vec3(0.0f, 0.0f, 10.0f));

	xAxis.SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
	yAxis.SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
	zAxis.SetColor(glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 model, view, proj;
	auto identity = glm::mat4(1.0f);

	float currentFrame = 0.0f;

	CheckForErrors("ERROR::SOURCE::MAIN: ");

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

		pointLight->m_Pos = glm::vec3(1.0f + sin(currentFrame), pointLight->m_Pos.y, 1.0f + sin(currentFrame));

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

		// Render objects
		for (int i = 1; i < 11; i++)
		{
			model = glm::translate(identity, cubePositions[i - 1]);
			float angle = 20.0f * static_cast<float>(i - 1);
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			meshes.at(i)->SetMVP(model, view, proj);
		}
		//cube->SetMVP(model, view, proj);

		model = glm::scale(glm::translate(identity, pointLight->m_Pos), glm::vec3(0.2f));
		pointLightMesh->SetMVP(model, view, proj);
		
		flashlight->m_Pos = camera.m_Position;
		flashlight->m_Direction = camera.m_Front;

		for (const auto& mesh : meshes)
		{
			mesh->Draw();
			if (renderNormals)
				mesh->DrawNormals(glm::vec3(1.0f, 0.0f, 1.0f));
		}

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
