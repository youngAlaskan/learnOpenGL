#pragma once

#include <glad\glad.h>
#include <glm\gtc\type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "..\Scene\Components\TransformComponent.h"
#include "UniformBuffer.h"
#include "..\Scene\Components\MaterialComponent.h"
#include "..\Scene\Light.h"
#include "..\Scene\Camera.h"

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

    void SetMaterial(const MaterialComponent& material) const
    {
        int activeMaps = 0;

        if (material.m_BaseColorMap)
        {
            SetInt("textures[0]", static_cast<int>(material.m_BaseColorMap->m_ID));
            activeMaps |= 1;
        }
        if (material.m_AlbedoMap)
        {
            SetInt("textures[1]", static_cast<int>(material.m_AlbedoMap->m_ID));
            activeMaps |= 2;
        }
        if (material.m_MetallicMap)
        {
            SetInt("textures[2]", static_cast<int>(material.m_MetallicMap->m_ID));
            activeMaps |= 4;
        }
        if (material.m_RoughnessMap)
        {
            SetInt("textures[3]", static_cast<int>(material.m_RoughnessMap->m_ID));
            activeMaps |= 8;
        }
        if (material.m_AmbientOcclusionMap)
        {
            SetInt("textures[4]", static_cast<int>(material.m_AmbientOcclusionMap->m_ID));
            activeMaps |= 16;
        }
        if (material.m_NormalMap)
        {
            SetInt("textures[5]", static_cast<int>(material.m_NormalMap->m_ID));
            activeMaps |= 32;
        }
        if (material.m_HeightMap)
        {
            SetInt("textures[6]", static_cast<int>(material.m_HeightMap->m_ID));
            activeMaps |= 1;
        }
        if (material.m_EmissionMap)
        {
            SetInt("textures[7]", static_cast<int>(material.m_EmissionMap->m_ID));
            activeMaps |= 64;
        }

        SetInt("material.activeMaps", activeMaps);
        SetFloat("material.shininess", material.m_Shininess);
    }

    void SetPointLights(const std::vector<PointLight>& pointLights) const
    {
        for (const auto& pointLight : pointLights)
        {
            const std::string prefix = "pointLights[" + std::to_string(pointLight.m_Index) + "].";
            SetVec4(prefix + "position", pointLight.m_Pos);
            SetVec4(prefix + "color",    pointLight.m_Color);

            SetFloat(prefix + "kA", pointLight.m_KA);
            SetFloat(prefix + "kD", pointLight.m_KD);
            SetFloat(prefix + "kS", pointLight.m_KS);

            SetFloat(prefix + "constant",  pointLight.m_Constant);
            SetFloat(prefix + "linear",    pointLight.m_Linear);
            SetFloat(prefix + "quadratic", pointLight.m_Quadratic);
        }
    }

    void SetDirectionalLights(const std::vector<DirectionalLight>& directionalLights) const
    {
        for (const auto& directionalLight : directionalLights)
        {
            const std::string prefix = "directionalLights[" + std::to_string(directionalLight.m_Index) + "].";
            SetVec3(prefix + "direction", directionalLight.m_Direction);
            SetVec4(prefix + "color", directionalLight.m_Color);

            SetFloat(prefix + "kA", directionalLight.m_KA);
            SetFloat(prefix + "kD", directionalLight.m_KD);
            SetFloat(prefix + "kS", directionalLight.m_KS);
        }
    }

    void SetSpotLights(const std::vector<SpotLight>& spotLights) const
    {
        for (const auto& spotLight : spotLights)
        {
            const std::string prefix = "spotLights[" + std::to_string(spotLight.m_Index) + "].";
            SetVec4(prefix + "position", spotLight.m_Pos);
            SetVec3(prefix + "direction", spotLight.m_Direction);
            SetVec4(prefix + "color", spotLight.m_Color);

            SetFloat(prefix + "kA", spotLight.m_KA);
            SetFloat(prefix + "kD", spotLight.m_KD);
            SetFloat(prefix + "kS", spotLight.m_KS);

            SetFloat(prefix + "constant", spotLight.m_Constant);
            SetFloat(prefix + "linear", spotLight.m_Linear);
            SetFloat(prefix + "quadratic", spotLight.m_Quadratic);

            SetFloat(prefix + "innerCutOff", spotLight.m_InnerCutOff);
            SetFloat(prefix + "outerCutOff", spotLight.m_OuterCutOff);
        }
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

inline auto isolatedShader = std::make_shared<Shader>("positionNormalTex.vert", "uniformColor.frag");
inline auto litObjectShader = std::make_shared<Shader>("positionNormalTex.vert", "objectLitByVariousLights.frag");
inline auto mirrorShader = std::make_shared<Shader>("positionNormalTex.vert", "skyboxMirror.frag");
inline auto refractorShader = std::make_shared<Shader>("positionNormalTex.vert", "skyboxRefractor.frag");
inline auto lineShader = std::make_shared<Shader>("position.vert", "uniformColor.frag");
inline auto skyboxShader = std::make_shared<Shader>("skybox.vert", "skybox.frag");
inline auto screenShader = std::make_shared<Shader>("screen.vert", "texture2D.frag");

inline std::vector shaders = {
    isolatedShader,
    litObjectShader,
    mirrorShader,
    refractorShader,
    lineShader,
    skyboxShader,
    screenShader
};
