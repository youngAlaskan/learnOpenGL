#pragma once

#include <glad\glad.h>
#include <glm\gtc\type_ptr.hpp>

#include <string>
#include <sstream>
#include <vector>

#include "..\Scene\Components\TransformComponent.h"
#include "UniformBuffer.h"
#include "..\Scene\SceneData.h"

class Shader
{
public:
    Shader() { CreateProgram(); }

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);

    void CreateProgram() { m_ID = glCreateProgram(); }
    void CreateProgram(unsigned int vertexShader, unsigned int fragmentShader);
    void CreateProgram(unsigned int vertexShader, unsigned int geometryShader, unsigned int fragmentShader);

    // use/activate the shader
    void Use() const { glUseProgram(m_ID); }

    void OnStart()
    {
        
    }

    // utility uniform functions
    void SetBool(const std::string& name, const bool value) const
        { glUniform1i(glGetUniformLocation(m_ID, name.c_str()), static_cast<int>(value)); }
    void SetInt(const std::string& name, const int value) const
        { glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value); }
    void SetFloat(const std::string& name, const float value) const
        { glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value); }
    void SetVec2(const std::string& name, glm::vec2 v) const
        { glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(v)); }
    void SetVec3(const std::string& name, glm::vec3 v) const
        { glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(v)); }
    void SetVec4(const std::string& name, glm::vec4 v) const
        { glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(v)); }
    void SetMat2(const std::string& name, glm::mat2 m) const
        { glUniformMatrix2fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(m)); }
    void SetMat3(const std::string& name, glm::mat3 m) const
        { glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(m)); }
    void SetMat4(const std::string& name, glm::mat4 m) const
        { glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(m)); }

    // Utility uniform object functions
    void SetTransform(const TransformComponent& transform) const;
    void SetUniformBuffer(const std::shared_ptr<UniformBuffer>& uniformBuffer, const std::string& name) const;
    void SetPointLights(const std::vector<PointLight>& pointLights) const;
    void SetDirectionalLight(const DirectionalLight& directionalLight) const;
    void SetSpotLight(const SpotLight& spotLight) const;
    void SetCameraPosition(const glm::vec3& position) const
        { SetVec3("viewPos", position); }

    void SetSceneData(const SceneData& sceneData) const;

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
    static void CheckCompileErrors(unsigned int shader, const std::string& type);

    // Helper functions for Constructor
    static std::string ReadFile(const std::string& filepath);
    static unsigned int CreateShader(const char* shaderCode, GLenum shaderType, const char* shaderName);
};

inline std::shared_ptr<Shader> isolatedShader;
inline std::shared_ptr<Shader> litObjectShader;
inline std::shared_ptr<Shader> mirrorShader;
inline std::shared_ptr<Shader> refractorShader;
inline std::shared_ptr<Shader> lineShader;
inline std::shared_ptr<Shader> skyboxShader;
inline std::shared_ptr<Shader> screenShader;

inline std::vector shaders = {
    isolatedShader,
    litObjectShader,
    mirrorShader,
    refractorShader,
    lineShader,
    skyboxShader,
    screenShader
};
