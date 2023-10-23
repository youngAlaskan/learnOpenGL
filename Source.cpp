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

#include "utils.h"

// #include "Renderbuffer.h"
// #include "Framebuffer.h"

#include "Scene\Scene.h"
#include "Scene\Model.h"

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

	g_IsolatedShader = std::make_shared<Shader>("positionNormalTex.vert", "uniformColor.frag");
	g_LitObjectShader = std::make_shared<Shader>("positionNormalTex.vert", "objectLitByVariousLights.frag");
	g_MirrorShader = std::make_shared<Shader>("positionNormalTex.vert", "skyboxMirror.frag");
	g_RefractorShader = std::make_shared<Shader>("positionNormalTex.vert", "skyboxRefractor.frag");
	g_LineShader = std::make_shared<Shader>("position.vert", "uniformColor.frag");
	g_SkyboxShader = std::make_shared<Shader>("skybox.vert", "skybox.frag");
	g_ScreenShader = std::make_shared<Shader>("screen.vert", "texture2D.frag");

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
	auto renderer = Renderer();
	auto scene = std::make_shared<Scene>(std::weak_ptr(std::make_shared<Renderer>(renderer)));

	scene->m_SceneData.PointLights = std::make_shared<std::vector<PointLight>>();
	scene->m_SceneData.PointLights->emplace_back(glm::vec4(2.0f, 1.0f, 3.0f, 1.0f), 100.0f, 0.2f, 0.5f, 0.99f);

	scene->m_SceneData.Sun = std::make_shared<DirectionalLight>(glm::normalize(glm::vec3(1.0, -1.0, 1.0)), 0.2f, 0.5f, 0.99f);

	scene->m_SceneData.Flashlight = std::make_shared<SpotLight>(12.5f, 0.2f, 0.5f, 0.99f);

	std::vector<std::string> faces
	{
		".\\textures\\skybox\\right.jpg",
		".\\textures\\skybox\\left.jpg",
		".\\textures\\skybox\\top.jpg",
		".\\textures\\skybox\\bottom.jpg",
		".\\textures\\skybox\\front.jpg",
		".\\textures\\skybox\\back.jpg"
	};

	scene->m_SceneData.Skybox = std::make_shared<Skybox>(std::weak_ptr<Shader>(g_SkyboxShader), std::make_shared<TexCube>(faces));

	auto entity = scene->CreateEntity();
	scene->AddComponent<CubeComponent>(entity);

	return std::make_pair(scene, renderer);
}

int main()
{
	GLFWwindow* window = Init();
	if (!window)
		return -1;

	g_LitObjectShader->Use();
	for (int i = 0; i < 16; i++)
		g_LitObjectShader->SetInt("textures[" + std::to_string(i) + "]", i);

	g_MirrorShader->Use();
	g_MirrorShader->SetInt("skybox", 0);

	g_RefractorShader->Use();
	g_RefractorShader->SetInt("skybox", 0);

	g_SkyboxShader->Use();
	g_SkyboxShader->SetInt("skybox", 0);

	auto [scene, renderer] = SandboxScene();

	const auto uniformMatrixBuffer = std::make_shared<UniformBuffer>();
	uniformMatrixBuffer->SetData(2 * sizeof(glm::mat4), nullptr);
	uniformMatrixBuffer->BindDataRange(0, 0, 2 * sizeof(glm::mat4));
	// SetUniformBuffer(uniformMatrixBuffer, "Matrices");

	// Render Loop
	std::cout << "Starting render loop" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		UpdateFrameRate(window);
		ProcessInput(window);

		// Set uniform view and projection matrices
		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 proj = glm::perspective(glm::radians(camera->m_Zoom),
			static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
		uniformMatrixBuffer->SetSubData(0, sizeof(proj), glm::value_ptr(proj));
		uniformMatrixBuffer->SetSubData(sizeof(proj), sizeof(view), glm::value_ptr(view));

		// Update flashlight position to match camera's
		scene->m_SceneData.Flashlight->Update(glm::vec4(camera->m_Position, 1.0f), camera->m_Front);

		// Render
		scene->OnUpdate();

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
