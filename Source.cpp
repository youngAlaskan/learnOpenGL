#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <iostream>
#include <vector>
// #define LOCK_FRAMERATE
#ifdef LOCK_FRAMERATE
#include <thread>
#endif

#define STB_IMAGE_IMPLEMENTATION

#include "utils.h"

// #include "Renderbuffer.h"
// #include "Framebuffer.h"

#include "Model.h"
// TM
//   - Renderable
//       - TransformComponent
//   - Vertex
//   - Material
//       - Texture
//   - Scene
//       - Light
//       - Camera
//       - Texture

#include "Renderer.h"
// Line
//   - Renderable
//       - TransformComponent
// TM
//   - Renderable
//       - TransformComponent
//   - Vertex
//   - Material
//       - Texture
//   - Scene
//       - Light
//       - Camera
//       - Texture
// Shader
//   - TransformComponent
//   - Material
//       - Texture
//   - Light
//   - Camera

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

auto camera = std::make_shared<Camera>(glm::vec4(1.0f, 1.0f, 3.0f, 1.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

std::vector<int> frameRateHistory(10, 0);

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
		camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		camera->ProcessKeyboard(TOGGLE_FLY, deltaTime);
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

	camera->ProcessMouseMovement(xOffset, yOffset);
}

void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera->ProcessMouseScroll(static_cast<float>(yOffset));
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
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, nullptr);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

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

std::pair<std::shared_ptr<Scene>, Renderer> SandboxScene()
{
	auto scene = std::make_shared<Scene>();
	auto renderer = Renderer(scene);

	scene->Camera = camera;

	std::vector<std::string> faces
	{
		".\\textures\\skybox\\right.jpg",
			".\\textures\\skybox\\left.jpg",
			".\\textures\\skybox\\top.jpg",
			".\\textures\\skybox\\bottom.jpg",
			".\\textures\\skybox\\front.jpg",
			".\\textures\\skybox\\back.jpg"
	};

	auto skybox = std::make_shared<CubemapMesh>(faces);
	scene->Skybox = skybox->m_Texture;
	renderer.m_Meshes.emplace_back(skybox);

	auto pointLight = std::make_shared<PointLight>(glm::vec4(2.0f, 1.0f, 3.0f, 1.0f), 100.0f, 0.2f, 0.5f, 0.99f);
	scene->PointLights.emplace_back(pointLight);

	std::cout << PointLight::GetCount() << std::endl;

	auto pointLightMesh = std::make_shared<TriangleMesh>();
	pointLightMesh->SetAsAACube();
	pointLightMesh->m_TransformComponent.Translation = pointLight->m_Pos;
	pointLightMesh->m_TransformComponent.Scale = glm::vec3(0.2f);
	renderer.m_Meshes.emplace_back(pointLightMesh);

	scene->DirectionalLight = std::make_shared<DirectionalLight>(glm::normalize(glm::vec3(1.0, -1.0, 1.0)), 0.2f, 0.5f, 0.99f);

	scene->SpotLight = std::make_shared<SpotLight>(12.5f, 0.2f, 0.5f, 0.99f);

	auto containerDiffuse = std::make_shared<Tex2D>(".\\textures\\container2.png", "diffuse");
	auto containerSpecular = std::make_shared<Tex2D>(".\\textures\\container2_specular.png", "specular");
	auto blackEmissive = std::make_shared<Tex2D>(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), "emissive");
	auto containerTextures = std::vector<std::shared_ptr<Tex2D>>{ containerDiffuse, containerSpecular, blackEmissive };
	auto containerMaterial = std::make_shared<Material>(containerTextures, 0.5f);

	auto containerCube = std::make_shared<TriangleMesh>(containerMaterial, DrawingMode::LIT_OBJECT);
	containerCube->SetAsAACube();
	containerCube->m_TransformComponent.Translation = glm::vec3(-2.0f, 0.0f, 2.0f);
	renderer.m_Meshes.emplace_back(containerCube);

	auto start = glfwGetTime();
	auto backpack = Model("C:\\Dev\\LearnOpenGL\\assets\\backpack.obj");
	auto end = glfwGetTime();
	std::cout << "Model load time: " << end - start << " seconds" << std::endl;

	for (auto& mesh : backpack.m_Meshes)
		renderer.m_Meshes.emplace_back(mesh);

	auto floor = std::make_shared<TriangleMesh>(containerMaterial, DrawingMode::MIRROR);
	floor->SetAsAAPlane();
	floor->m_TransformComponent.Translation = glm::vec3(0.0f, -2.0f, 0.0f);
	floor->m_TransformComponent.Rotation = glm::vec3(glm::radians(270.0f), 0.0f, 0.0f);
	floor->m_TransformComponent.Scale = glm::vec3(10.0f, 10.0f, 10.0f);
	renderer.m_Meshes.emplace_back(floor);

	auto grassDiffuse = std::make_shared<Tex2D>(".\\textures\\grass.png", "diffuse");
	auto grassMaterial = std::make_shared<Material>(grassDiffuse, 0.2f);

	auto square = std::make_shared<TriangleMesh>(grassMaterial, DrawingMode::LIT_OBJECT);
	square->SetAsAAPlane();
	square->m_TransformComponent.Translation = glm::vec3(0.5f, 0.0f, 1.5f);
	renderer.m_Meshes.emplace_back(square);

	auto windowDiffuse = std::make_shared<Tex2D>(".\\textures\\blending_transparent_window.png", "diffuse");
	auto windowMaterial = std::make_shared<Material>(windowDiffuse, 0.2f);

	auto windowMesh = std::make_shared<TriangleMesh>(windowMaterial, DrawingMode::LIT_OBJECT);
	windowMesh->SetAsAAPlane();
	windowMesh->m_TransformComponent.Translation = glm::vec3(0.4f, 0.0f, 2.0f);
	renderer.m_Meshes.emplace_back(windowMesh);

	return std::make_pair(scene, renderer);
}

int main()
{
	GLFWwindow* window = Init();
	if (!window)
		return -1;

	auto [scene, renderer] = SandboxScene();

	const auto uniformMatrixBuffer = std::make_shared<UniformBuffer>();
	uniformMatrixBuffer->SetData(2 * sizeof(glm::mat4), nullptr);
	uniformMatrixBuffer->BindDataRange(0, 0, 2 * sizeof(glm::mat4));
	renderer.SetUniformBuffer(uniformMatrixBuffer, "Matrices");

	// Render Loop
	std::cout << "Starting render loop" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		UpdateFrameRate(window);
		ProcessInput(window);

		// Set matrices
		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 proj = glm::perspective(glm::radians(camera->m_Zoom),
			static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
		uniformMatrixBuffer->SetSubData(0, sizeof(proj), glm::value_ptr(proj));
		uniformMatrixBuffer->SetSubData(sizeof(proj), sizeof(view), glm::value_ptr(view));

		if (scene->SpotLight)
			scene->SpotLight->Update(glm::vec4(camera->m_Position, 1.0f), camera->m_Front);

		// Render
		// -------
		renderer.Render(renderAxis, renderNormals);

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
