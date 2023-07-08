#pragma once

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>

class Shader
{
public:
    Shader() = default;
    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void Use() const { glUseProgram(m_ID); }
    // utility uniform functions
    void SetBool(const std::string& name, const bool value) const { glUniform1i(glGetUniformLocation(m_ID, name.c_str()), static_cast<int>(value)); }
    void SetInt(const std::string& name, const int value) const { glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value); }
    void SetFloat(const std::string& name, const float value) const { glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value); }
    void SetVec2(const std::string& name, glm::vec2 v) const { glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(v)); }
    void SetVec3(const std::string& name, glm::vec3 v) const { glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(v)); }
    void SetVec4(const std::string& name, glm::vec4 v) const { glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(v)); }
    void SetMat2(const std::string& name, glm::mat2 m) const { glUniformMatrix2fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(m)); }
    void SetMat3(const std::string& name, glm::mat3 m) const { glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(m)); }
    void SetMat4(const std::string& name, glm::mat4 m) const { glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(m)); }
    bool operator==(const Shader& other) const { return m_ID == other.m_ID; }

public:
    // the program m_ID
    unsigned int m_ID;

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    static void CheckCompileErrors(const unsigned int shader, const std::string& type);
};
