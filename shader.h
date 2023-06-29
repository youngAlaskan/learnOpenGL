#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    Shader() {}

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(std::string(".\\shaders\\vertexShaders\\") + std::string(vertexPath));
            fShaderFile.open(std::string(".\\shaders\\fragmentShaders\\") + std::string(fragmentPath));
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        CheckCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        CheckCompileErrors(fragment, "FRAGMENT");
        // shader Program
        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertex);
        glAttachShader(m_ID, fragment);
        glLinkProgram(m_ID);
        CheckCompileErrors(m_ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // use/activate the shader
    void Use() const
    {
        glUseProgram(m_ID);
    }

    // utility uniform functions
    void SetBool(const std::string& name, const bool value) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), static_cast<int>(value));
    }
    void SetInt(const std::string& name, const int value) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
    }
    void SetFloat(const std::string& name, const float value) const
    {
        glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
    }
    void SetVec2(const std::string& name, glm::vec2 v) const
    {
        glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(v));
    }
    void SetVec3(const std::string& name, glm::vec3 v) const
    {
        glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(v));
    }
    void SetVec4(const std::string& name, glm::vec4 v) const
    {
        glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(v));
    }
    void SetMat2(const std::string& name, glm::mat2 m) const
    {
        glUniformMatrix2fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(m));
    }
    void SetMat3(const std::string& name, glm::mat3 m) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(m));
    }
    void SetMat4(const std::string& name, glm::mat4 m) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(m));
    }

    bool operator==(const Shader& other) const
    {
        return m_ID == other.m_ID;
    }

public:
    // the program m_ID
    unsigned int m_ID;

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    static void CheckCompileErrors(const unsigned int shader, const std::string& type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
