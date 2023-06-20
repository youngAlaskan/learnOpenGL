#pragma once

#include <glad/glad.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <string>
#include <iostream>

class texture
{
public:
	void setTex2D(const char* filepath, GLint format)
	{
        // load and create a texture 
        // -------------------------
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);
        
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        
        unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }

        stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

    void setTexCube(const char* filepath, GLint format) {
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, channelsN;
		unsigned char* data = stbi_load(filepath, &width, &height, &channelsN, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0,
				format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

	void setTexCube(const char** filepaths, GLint format) {
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		unsigned char* data;
		for (unsigned int i = 0; i < 6; i++)
		{
			data = stbi_load(filepaths[i], &width, &height, &nrChannels, 0);
			if (data) {
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
			}
			else
			{
				std::cout << "Failed to load cube map texture" << std::endl;
			}
		}

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void setWrap2D(GLint sWrap, GLint tWrap) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
	}

	void setWrap3D(GLint sWrap, GLint tWrap, GLint rWrap) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, rWrap);
	}

	void use2D(GLint index) {
		glActiveTexture(index);
		glBindTexture(GL_TEXTURE_2D, ID);
	}

	void useCube(GLint index) {
		glActiveTexture(index);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	}

	~texture() {
		glDeleteTextures(1, &ID);
	}

public:
	unsigned int ID;
};
