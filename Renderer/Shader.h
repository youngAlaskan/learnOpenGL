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

    // Set uniform boolean
    void SetBool(const std::string& name, const bool value) const;
    // Set uniform int
    void SetInt(const std::string& name, const int value) const;
    // Set uniform float
    void SetFloat(const std::string& name, const float value) const;
    // Set uniform vec2
    void SetVec2(const std::string& name, glm::vec2 v) const;
    // Set uniform vec3
    void SetVec3(const std::string& name, glm::vec3 v) const;
    // Set uniform vec4
    void SetVec4(const std::string& name, glm::vec4 v) const;
    // Set uniform mat2
    void SetMat2(const std::string& name, glm::mat2 m) const;
    // Set uniform mat3
    void SetMat3(const std::string& name, glm::mat3 m) const;
    // Set uniform mat4
    void SetMat4(const std::string& name, glm::mat4 m) const;

    // Set uniform transform
    void SetTransform(const TransformComponent& transform) const;
    // Set uniform buffer
    void SetUniformBuffer(const std::shared_ptr<UniformBuffer>& uniformBuffer, const std::string& name) const;
    // Set uniform point lights
    void SetPointLights(const std::vector<PointLight>& pointLights) const;
    // Set uniform directional lights
    void SetDirectionalLight(const DirectionalLight& directionalLight) const;
    // Set uniform spot light
    void SetSpotLight(const SpotLight& spotLight) const;
    // Set unfiform view position, "viewPos"
    void SetCameraPosition(const glm::vec3& position) const
        { SetVec3("viewPos", position); }
    // Set uniform scene data
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

inline std::shared_ptr<Shader> g_IsolatedShader;
inline std::shared_ptr<Shader> g_LitObjectShader;
inline std::shared_ptr<Shader> g_MirrorShader;
inline std::shared_ptr<Shader> g_RefractorShader;
inline std::shared_ptr<Shader> g_LineShader;
inline std::shared_ptr<Shader> g_SkyboxShader;
inline std::shared_ptr<Shader> g_ScreenShader;

inline std::vector shaders = {
    g_IsolatedShader,
    g_LitObjectShader,
    g_MirrorShader,
    g_RefractorShader,
    g_LineShader,
    g_SkyboxShader,
    g_ScreenShader
};
