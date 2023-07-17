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

#include "Renderbuffer.h"
#include "Framebuffer.h"

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Camera.h"

#include "Line.h"
#include "TM.h"
#include "Light.h"

#include "Model.h"

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

Camera camera(glm::vec4(1.0f, 1.0f, 3.0f, 1.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

std::vector<int> frameRateHistory(10, 0);

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

GLFWwindow* Init()
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
		return nullptr;
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
		return nullptr;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, nullptr);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);

	return window;
}

void UpdateFrameRate(GLFWwindow* window)
{
	// Calculate Frame Rate
	const auto currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	frameRateHistory.pop_back();
	frameRateHistory.insert(frameRateHistory.begin(), static_cast<int>(1.0f / deltaTime));
	int averageFrameRate = 0;
	for (const int rate : frameRateHistory)
		averageFrameRate += rate;
	averageFrameRate /= static_cast<int>(frameRateHistory.size());
	glfwSetWindowTitle(window, ("LearnOpenGL FPS: " + std::to_string(averageFrameRate)).c_str());
}

int main()
{
	GLFWwindow* window = Init();
	if (!window)
		return -1;

	std::vector<std::shared_ptr<TriangleMesh>> triangleMeshes;

	std::vector<std::shared_ptr<PointLight>> pointLights;
	auto pointLight = std::make_shared<PointLight>(glm::vec4(2.0f, 1.0f, 3.0f, 1.0f), 100.0f, 0.2f, 0.5f, 0.99f);
	pointLights.emplace_back(pointLight);

	std::cout << PointLight::GetCount() << std::endl;

	auto pointLightMesh = std::make_shared<TriangleMesh>();
	pointLightMesh->SetAsAACube(pointLight->m_Color);
	pointLightMesh->SetModel(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(pointLight->m_Pos)), glm::vec3(0.2f)));
	triangleMeshes.emplace_back(pointLightMesh);

	auto directionalLight = std::make_shared<DirectionalLight>(glm::normalize(glm::vec3(1.0, -1.0, 1.0)), 0.2f, 0.5f, 0.99f);

	auto flashlight = std::make_shared<SpotLight>(12.5f, 0.2f, 0.5f, 0.99f);

	auto containerDiffuse = std::make_shared<Tex2D>(".\\textures\\container2.png");
	containerDiffuse->SetTagDiffuse();
	auto containerSpecular = std::make_shared<Tex2D>(".\\textures\\container2_specular.png");
	containerSpecular->SetTagSpecular();
	auto blackEmissive = std::make_shared<Tex2D>(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	blackEmissive->SetTagEmissive();
	auto containerTextures = std::vector<std::shared_ptr<Texture>>{ containerDiffuse, containerSpecular, blackEmissive };
	auto containerMaterial = std::make_shared<Material>(containerTextures, 0.5f);

	auto containerCube = std::make_shared<TriangleMesh>(containerMaterial, pointLights, directionalLight, flashlight, std::make_shared<Camera>(camera));
	containerCube->SetAsAACube();
	containerCube->m_DrawingMode = LIT_OBJECT;
	containerCube->SetModel(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 2.0f)));
	triangleMeshes.emplace_back(containerCube);

	auto start = glfwGetTime();
	auto backpack = Model("C:\\Dev\\LearnOpenGL\\assets\\backpack.obj");
	auto end = glfwGetTime();
	std::cout << "Model load time: " << end - start << " seconds" << std::endl;

	for (auto& mesh : backpack.m_Meshes)
	{
		mesh->m_DirectionalLight = directionalLight;
		mesh->m_PointLights = pointLights;
		mesh->m_SpotLight = flashlight;
		mesh->m_Camera = std::make_shared<Camera>(camera);
		mesh->SetModel(glm::mat4(1.0f));
		triangleMeshes.emplace_back(mesh);
	}

	auto floor = std::make_shared<TriangleMesh>(containerMaterial, pointLights, directionalLight, flashlight, std::make_shared<Camera>(camera));
	floor->SetAsAARectangle();
	floor->m_DrawingMode = LIT_OBJECT;
	floor->SetModel(glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f)), glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(10.0f)));
	triangleMeshes.emplace_back(floor);

	auto grassDiffuse = std::make_shared<Tex2D>(".\\textures\\grass.png");
	grassDiffuse->SetTagDiffuse();
	Tex2D::SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	auto grassMaterial = std::make_shared<Material>(std::vector<std::shared_ptr<Texture>>{grassDiffuse}, 0.2f);

	auto square = std::make_shared<TriangleMesh>(grassMaterial, pointLights, directionalLight, flashlight, std::make_shared<Camera>(camera));
	square->SetAsAARectangle();
	square->m_DrawingMode = LIT_OBJECT;
	square->SetModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 1.5f)));
	triangleMeshes.emplace_back(square);

	auto windowDiffuse = std::make_shared<Tex2D>(".\\textures\\blending_transparent_window.png");
	windowDiffuse->SetTagDiffuse();
	auto windowMaterial = std::make_shared<Material>(std::vector<std::shared_ptr<Texture>>{windowDiffuse}, 0.2f);

	auto windowMesh = std::make_shared<TriangleMesh>(windowMaterial, pointLights, directionalLight, flashlight, std::make_shared<Camera>(camera));
	windowMesh->SetAsAARectangle();
	windowMesh->m_DrawingMode = LIT_OBJECT;
	windowMesh->SetModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 2.0f)));
	triangleMeshes.emplace_back(windowMesh);

	auto origin = glm::vec3(0.0f, 0.0f, 0.0f);

	Line xAxis(origin, glm::vec3(10.0f,  0.0f,  0.0f));
	Line yAxis(origin, glm::vec3( 0.0f, 10.0f,  0.0f));
	Line zAxis(origin, glm::vec3( 0.0f,  0.0f, 10.0f));

	xAxis.SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	yAxis.SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	zAxis.SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	glm::mat4 view, proj;
	auto identity = glm::mat4(1.0f);

	auto colorBuffer = TexColorBuffer(SCR_WIDTH, SCR_HEIGHT);
	auto depthAndStencilBuffer = Renderbuffer(SCR_WIDTH, SCR_HEIGHT);
	auto framebuffer = Framebuffer(colorBuffer, depthAndStencilBuffer);

	// Render Loop
	std::cout << "Starting render loop" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		UpdateFrameRate(window);
		ProcessInput(window);

		// Set matrices
		view = camera.GetViewMatrix();
		proj = glm::perspective(glm::radians(camera.m_Zoom),
			static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);

		flashlight->m_Pos = glm::vec4(camera.m_Position, 1.0f);
		flashlight->m_Direction = camera.m_Front;

		// Render
		// -------
		framebuffer.Use();
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (renderAxis)
		{
			xAxis.SetMVP(identity, view, proj);
			yAxis.SetMVP(identity, view, proj);
			zAxis.SetMVP(identity, view, proj);

			xAxis.Draw();
			yAxis.Draw();
			zAxis.Draw();
		}

		for (auto& mesh : triangleMeshes)
		{
			mesh->SetView(view);
			mesh->SetProj(proj);
			mesh->Draw();
			if (renderNormals)
				mesh->DrawNormals();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);



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
