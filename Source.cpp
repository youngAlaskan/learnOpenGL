#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "tm.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool renderFilled = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float mixPercent = 0.2f;

Camera camera(glm::vec3(0.0f, -3.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

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

	// Create Shader(s)
	Shader shaderProgramCube(".\\shaders\\vertexShaders\\vertexShaderSource.vert",
		".\\shaders\\fragmentShaders\\fragmentShaderSource.frag");

	Shader shaderProgramContainer(".\\shaders\\vertexShaders\\vertexShaderSource.vert",
		".\\shaders\\fragmentShaders\\fragmentShader1t2D.frag");

	// Textures
	glEnable(GL_DEPTH_TEST);
	stbi_set_flip_vertically_on_load(true);
	texture tex1, tex2, tex3;
	tex1.setTexCube(".\\textures\\container.jpg", GL_RGB);
	tex2.setTexCube(".\\textures\\awesomeface.png", GL_RGBA);
	tex3.setTex2D(".\\textures\\container.jpg", GL_RGB);

	// Set up vertex data
	// ------------------
	TriangleMesh cube, rectangle;
	cube.SetAsAACube();
	rectangle.SetAsAARectangle(XY, 10.0f);

	// Create Buffers
	uint32_t VAOs[2], VBOs[2], EBOs[2];
	glGenVertexArrays(2, VAOs);
	glGenBuffers(2, VBOs);
	glGenBuffers(2, EBOs);
	// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, cube.m_TrianglesN * 36 * sizeof(float), cube.m_ConnectivityData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.m_TrianglesN * 3 * sizeof(unsigned int), cube.m_Indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Unbinds
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Bind VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAOs[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, rectangle.m_TrianglesN * 36 * sizeof(float), rectangle.m_ConnectivityData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, rectangle.m_TrianglesN * 3 * sizeof(unsigned int), rectangle.m_Indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Unbinds
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glm::mat4 model, view, proj;

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

	// Render Loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float) glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input
		processInput(window);

		view = camera.GetViewMatrix(); 
		proj = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);

		shaderProgramCube.use();
		shaderProgramCube.setMat4("view", glm::value_ptr(view));
		shaderProgramCube.setMat4("proj", glm::value_ptr(proj));

		// Render
		// ------
		// Set Background
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // RGBA
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set shader variables
		shaderProgramCube.setFloat("mixPercent", mixPercent);

		tex1.useCube(GL_TEXTURE0);
		tex2.useCube(GL_TEXTURE1);
		shaderProgramCube.setInt("texture1", 0);
		shaderProgramCube.setInt("texture2", 1);

		for (unsigned int i = 0; i < 10; i++)
		{
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			float angle = i % 3 == 0 ? 25.0f * (float)glfwGetTime() : 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

			shaderProgramCube.setMat4("model", glm::value_ptr(model));

			// Draw Triangles
			glBindVertexArray(VAOs[0]);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		shaderProgramContainer.use();
		shaderProgramContainer.setMat4("view", glm::value_ptr(view));
		shaderProgramContainer.setMat4("proj", glm::value_ptr(proj));

		tex3.use2D(GL_TEXTURE0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.0f, -3.0f));
		model = glm::scale(model, glm::vec3(100.0f, 1.0f, 100.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		shaderProgramContainer.setMat4("model", glm::value_ptr(model));
		shaderProgramContainer.setInt("texture1", 0);

		// Draw Triangles
		glBindVertexArray(VAOs[1]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, VAOs);
	glDeleteBuffers(1, VBOs);
	glDeleteProgram(shaderProgramCube.ID);

	// delete[] vertices;
	// delete[] colors;
	// delete[] texCoords;

	glfwTerminate();
	return 0;
}
