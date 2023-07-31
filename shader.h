#pragma once

#include <glad\glad.h>
#include <glm\gtc\type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "TransformComponent.h"
#include "UniformBuffer.h"
#include "Material.h"
#include "Light.h"
#include "Camera.h"

class Shader
{
public:
    Shader()
    {
        CreateProgram();
    }

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        const std::string vertexCode = ReadFile(std::string(".\\shaders\\vertexShaders\\") + std::string(vertexPath));
        const std::string fragmentCode = ReadFile(std::string(".\\shaders\\fragmentShaders\\") + std::string(fragmentPath));

        const unsigned int vertex = CreateShader(vertexCode.c_str(), GL_VERTEX_SHADER, "VERTEX");
        const unsigned int fragment = CreateShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER, "FRAGMENT");

        CreateProgram(vertex, fragment);

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
    {
        const std::string vertexCode   = ReadFile(std::string(".\\shaders\\vertexShaders\\")   + std::string(vertexPath));
        const std::string geometryCode = ReadFile(std::string(".\\shaders\\geometryShaders\\") + std::string(geometryPath));
        const std::string fragmentCode = ReadFile(std::string(".\\shaders\\fragmentShaders\\") + std::string(fragmentPath));

        const unsigned int vertex   = CreateShader(vertexCode.c_str(),   GL_VERTEX_SHADER,   "VERTEX");
        const unsigned int geometry = CreateShader(geometryCode.c_str(), GL_GEOMETRY_SHADER, "GEOMETRY");
        const unsigned int fragment = CreateShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER, "FRAGMENT");
        
        CreateProgram(vertex, geometry, fragment);
        
        glDeleteShader(vertex);
        glDeleteShader(geometry);
        glDeleteShader(fragment);
    }

    void CreateProgram()
    {
        m_ID = glCreateProgram();
    }

    void CreateProgram(const unsigned int vertexShader, const unsigned int fragmentShader)
    {
        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertexShader);
        glAttachShader(m_ID, fragmentShader);
        glLinkProgram(m_ID);
        CheckCompileErrors(m_ID, "PROGRAM");
    }

    void CreateProgram(const unsigned int vertexShader, const unsigned int geometryShader, const unsigned int fragmentShader)
    {
        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertexShader);
        glAttachShader(m_ID, geometryShader);
        glAttachShader(m_ID, fragmentShader);
        glLinkProgram(m_ID);
        CheckCompileErrors(m_ID, "PROGRAM");
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

    void SetTransform(const TransformComponent& transform) const
    {
        SetMat4("model", transform.GetTransform());
        SetMat4("modelInv", glm::inverse(transform.GetTransform()));
    }

    void SetUniformBuffer(const std::shared_ptr<UniformBuffer>& uniformBuffer, const std::string& name) const
    {
        const unsigned int uniformBlockIndex = glGetUniformBlockIndex(m_ID, name.c_str());
        if (uniformBlockIndex != GL_INVALID_INDEX)
            glUniformBlockBinding(m_ID, uniformBlockIndex, uniformBuffer->m_Index);
    }

    void SetMaterial(const std::shared_ptr<Material>& material) const
    {
        for (int i = 0; i < static_cast<int>(material->m_Textures.size()); i++)
            material->m_Textures[i]->Use(i);

        SetInt("material.diffuseEnd",  material->m_DiffuseEnd);
        SetInt("material.specularEnd", material->m_SpecularEnd);
        SetInt("material.emissiveEnd", material->m_EmissiveEnd);
        SetFloat("material.shininess", material->m_Shininess);
    }

    void SetPointLights(const std::vector<std::shared_ptr<PointLight>>& pointLights) const
    {
        for (const auto& pointLight : pointLights)
        {
            const std::string prefix = "pointLights[" + std::to_string(pointLight->m_Index) + "].";
            SetVec4(prefix + "position", pointLight->m_Pos);
            SetVec4(prefix + "color",    pointLight->m_Color);

            SetFloat(prefix + "kA", pointLight->m_KA);
            SetFloat(prefix + "kD", pointLight->m_KD);
            SetFloat(prefix + "kS", pointLight->m_KS);

            SetFloat(prefix + "constant",  pointLight->m_Constant);
            SetFloat(prefix + "linear",    pointLight->m_Linear);
            SetFloat(prefix + "quadratic", pointLight->m_Quadratic);
        }
    }

    void SetDirectionalLight(const std::shared_ptr<DirectionalLight>& directionalLight) const
    {
        SetVec3("dirLight.direction", directionalLight->m_Direction);
        SetVec4("dirLight.color", directionalLight->m_Color);

        SetFloat("dirLight.kA", directionalLight->m_KA);
        SetFloat("dirLight.kD", directionalLight->m_KD);
        SetFloat("dirLight.kS", directionalLight->m_KS);
    }

    void SetSpotLight(const std::shared_ptr<SpotLight>& spotLight) const
    {
        SetVec4("spotLight.position",  spotLight->m_Pos);
        SetVec3("spotLight.direction", spotLight->m_Direction);
        SetVec4("spotLight.color",     spotLight->m_Color);

        SetFloat("spotLight.kA", spotLight->m_KA);
        SetFloat("spotLight.kD", spotLight->m_KD);
        SetFloat("spotLight.kS", spotLight->m_KS);

        SetFloat("spotLight.constant",  spotLight->m_Constant);
        SetFloat("spotLight.linear",    spotLight->m_Linear);
        SetFloat("spotLight.quadratic", spotLight->m_Quadratic);

        SetFloat("spotLight.innerCutOff", spotLight->m_InnerCutOff);
        SetFloat("spotLight.outerCutOff", spotLight->m_OuterCutOff);
    }

    void SetCamera(const std::shared_ptr<Camera>& camera) const
    {
        SetVec3("viewPos", camera->m_Position);
    }

    bool operator==(const Shader& other) const
    {
        return m_ID == other.m_ID;
    }

    ~Shader()
    {
        if (!m_ID)
            glDeleteProgram(m_ID);
    }

public:
    // the program m_ID
    unsigned int m_ID = 0;

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

    //static std::string ReadFile(const std::string& filepath)
    //{
    //    std::ifstream fileStream;
    //    fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    //
    //    try
    //    {
    //        
    //        fileStream.open(filepath);
    //        std::stringstream vShaderStream;
    //        
    //        vShaderStream << fileStream.rdbuf();
    //        
    //        fileStream.close();
    //        
    //        return vShaderStream.str();
    //    }
    //    catch (std::ifstream::failure& e)
    //    {
    //        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    //    }
    //
    //    return {};
    //}

    static std::string ReadFile(const std::string& filepath)
    {
        try
        {
            std::ifstream fileStream(filepath);
            fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

            return { std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>() };
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }

        return {};
    }

    static unsigned int CreateShader(const char* shaderCode, const GLenum shaderType, const char* shaderName)
    {
        const unsigned int shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderCode, nullptr);
        glCompileShader(shader);
        CheckCompileErrors(shader, shaderName);
        return shader;
    }
};
