#include "Shader.h"

#include <fstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    const std::string vertexCode = ReadFile(std::string(".\\shaders\\vertexShaders\\") + std::string(vertexPath));
    const std::string fragmentCode = ReadFile(std::string(".\\shaders\\fragmentShaders\\") + std::string(fragmentPath));

    const unsigned int vertex = CreateShader(vertexCode.c_str(), GL_VERTEX_SHADER, "VERTEX");
    const unsigned int fragment = CreateShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER, "FRAGMENT");

    CreateProgram(vertex, fragment);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
{
    const std::string vertexCode = ReadFile(std::string(".\\shaders\\vertexShaders\\") + std::string(vertexPath));
    const std::string geometryCode = ReadFile(std::string(".\\shaders\\geometryShaders\\") + std::string(geometryPath));
    const std::string fragmentCode = ReadFile(std::string(".\\shaders\\fragmentShaders\\") + std::string(fragmentPath));

    const unsigned int vertex = CreateShader(vertexCode.c_str(), GL_VERTEX_SHADER, "VERTEX");
    const unsigned int geometry = CreateShader(geometryCode.c_str(), GL_GEOMETRY_SHADER, "GEOMETRY");
    const unsigned int fragment = CreateShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER, "FRAGMENT");

    CreateProgram(vertex, geometry, fragment);

    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);
}

void Shader::CreateProgram(const unsigned int vertexShader, const unsigned int fragmentShader)
{
    CreateProgram();
    glAttachShader(m_ID, vertexShader);
    glAttachShader(m_ID, fragmentShader);
    glLinkProgram(m_ID);
    CheckCompileErrors(m_ID, "PROGRAM");
}

void Shader::CreateProgram(const unsigned int vertexShader, const unsigned int geometryShader, const unsigned int fragmentShader)
{
    CreateProgram(vertexShader, fragmentShader);
    glAttachShader(m_ID, geometryShader);
    glLinkProgram(m_ID);
    CheckCompileErrors(m_ID, "PROGRAM");
}

void Shader::SetBool(const std::string& name, const bool value) const
{
    GLint location = glGetUniformLocation(m_ID, name.c_str());
    assert(location != -1);
    glUniform1i(location, static_cast<int>(value));
}

void Shader::SetInt(const std::string& name, const int value) const
{
    GLint location = glGetUniformLocation(m_ID, name.c_str());
    assert(location != -1);
    glUniform1i(location, value);
}

void Shader::SetFloat(const std::string& name, const float value) const
{
    GLint location = glGetUniformLocation(m_ID, name.c_str());
    assert(location != -1);
    glUniform1f(location, value);
}

void Shader::SetVec2(const std::string& name, glm::vec2 v) const
{
    GLint location = glGetUniformLocation(m_ID, name.c_str());
    assert(location != -1);
    glUniform2fv(location, 1, glm::value_ptr(v));
}

void Shader::SetVec3(const std::string& name, glm::vec3 v) const
{
    GLint location = glGetUniformLocation(m_ID, name.c_str());
    assert(location != -1);
    glUniform3fv(location, 1, glm::value_ptr(v));
}

void Shader::SetVec4(const std::string& name, glm::vec4 v) const
{
    GLint location = glGetUniformLocation(m_ID, name.c_str());
    assert(location != -1);
    glUniform4fv(location, 1, glm::value_ptr(v));
}

void Shader::SetMat2(const std::string& name, glm::mat2 m) const
{
    GLint location = glGetUniformLocation(m_ID, name.c_str());
    assert(location != -1);
    glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::SetMat3(const std::string& name, glm::mat3 m) const
{
    GLint location = glGetUniformLocation(m_ID, name.c_str());
    assert(location != -1);
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::SetMat4(const std::string& name, glm::mat4 m) const
{
    GLint location = glGetUniformLocation(m_ID, name.c_str());
    assert(location != -1);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::SetTransform(const TransformComponent& transform) const
{
    SetMat4("model", transform.GetTransform());
    SetMat4("modelInv", glm::inverse(transform.GetTransform()));
}

void Shader::SetUniformBuffer(const std::shared_ptr<UniformBuffer>& uniformBuffer, const std::string& name) const
{
    const unsigned int uniformBlockIndex = glGetUniformBlockIndex(m_ID, name.c_str());
    if (uniformBlockIndex != GL_INVALID_INDEX)
        glUniformBlockBinding(m_ID, uniformBlockIndex, uniformBuffer->m_Index);
}

void Shader::SetPointLights(const std::vector<PointLight>& pointLights) const
{
    for (const auto& pointLight : pointLights)
    {
        const std::string prefix = "pointLights[" + std::to_string(pointLight.m_Index) + "].";
        SetVec4(prefix + "position", pointLight.m_Pos);
        SetVec4(prefix + "color", pointLight.m_Color);

        SetFloat(prefix + "kA", pointLight.m_KA);
        SetFloat(prefix + "kD", pointLight.m_KD);
        SetFloat(prefix + "kS", pointLight.m_KS);

        SetFloat(prefix + "constant", pointLight.m_Constant);
        SetFloat(prefix + "linear", pointLight.m_Linear);
        SetFloat(prefix + "quadratic", pointLight.m_Quadratic);
    }
}

void Shader::SetDirectionalLight(const DirectionalLight& directionalLight) const
{
    SetVec3("dirLight.direction", directionalLight.m_Direction);
    SetVec4("dirLight.color", directionalLight.m_Color);

    SetFloat("dirLight.kA", directionalLight.m_KA);
    SetFloat("dirLight.kD", directionalLight.m_KD);
    SetFloat("dirLight.kS", directionalLight.m_KS);
}

void Shader::SetSpotLight(const SpotLight& spotLight) const
{
    SetVec4("spotLight.position", spotLight.m_Pos);
    SetVec3("spotLight.direction", spotLight.m_Direction);
    SetVec4("spotLight.color", spotLight.m_Color);

    SetFloat("spotLight.kA", spotLight.m_KA);
    SetFloat("spotLight.kD", spotLight.m_KD);
    SetFloat("spotLight.kS", spotLight.m_KS);

    SetFloat("spotLight.constant", spotLight.m_Constant);
    SetFloat("spotLight.linear", spotLight.m_Linear);
    SetFloat("spotLight.quadratic", spotLight.m_Quadratic);

    SetFloat("spotLight.innerCutOff", spotLight.m_InnerCutOff);
    SetFloat("spotLight.outerCutOff", spotLight.m_OuterCutOff);
}

void Shader::SetSceneData(const SceneData& sceneData) const
{
    if (sceneData.PointLights)
        SetPointLights(*sceneData.PointLights);
    if (sceneData.Sun)
        SetDirectionalLight(*sceneData.Sun);
    if (sceneData.Flashlight)
        SetSpotLight(*sceneData.Flashlight);
    if (sceneData.SkyboxTexture)
        SetInt("skybox", sceneData.SkyboxTexture);
    if (sceneData.ViewMatrix && sceneData.ProjectionMatrix)
    {
        const auto uniformMatrixBuffer = std::make_shared<UniformBuffer>();
        uniformMatrixBuffer->SetData(2 * sizeof(glm::mat4), nullptr);
        uniformMatrixBuffer->BindDataRange(0, 0, 2 * sizeof(glm::mat4));
        SetUniformBuffer(uniformMatrixBuffer, "Matrices");
    }
}


void Shader::CheckCompileErrors(const unsigned int shader, const std::string& type)
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

std::string Shader::ReadFile(const std::string& filepath)
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

unsigned int Shader::CreateShader(const char* shaderCode, const GLenum shaderType, const char* shaderName)
{
    const unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);
    CheckCompileErrors(shader, shaderName);
    return shader;
}
