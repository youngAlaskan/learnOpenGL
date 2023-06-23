#pragma once

#include <glad/glad.h>
#include <iostream>
#include <unordered_map>

std::unordered_map<GLenum, std::string> errorMessages {
	{ GL_INVALID_ENUM, "INVALID_ENUM, 0x0500" },
	{ GL_INVALID_VALUE, "INVALID_VALUE, 0x0501" },
	{ GL_INVALID_OPERATION, "INVALID_OPERATION, 0x0502" },
	{ GL_STACK_OVERFLOW, "STACK_OVERFLOW, 0x0503" },
	{ GL_STACK_UNDERFLOW, "STACK_UNDERFLOW, 0x0504" },
	{ GL_OUT_OF_MEMORY, "OUT_OF_MEMORY, 0x0505" },
	{ GL_INVALID_FRAMEBUFFER_OPERATION, "INVALID_FRAMEBUFFER_OPERATION, 0x0506" },
	{ GL_CONTEXT_LOST, "CONTEXT_LOST, 0x0507"}
};

void checkForErrors(const char* message)
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR)
	{
		std::cout << message << errorMessages[error] << std::endl;
	}
}

// Not using currently
void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::cerr
		<< "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
		<< " type = 0x" << type
		<< ", severity = 0x" << severity
		<< ", message = " << message << std::endl;
}
