#pragma once
#include <string>
#include <sstream>
#include <unordered_map>
#include <glad/glad.h>
#include <fstream>
#include <iostream>
#include <glm.hpp>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void Bind() const;
    void Unbind() const;
    void setUniformBool(const std::string& name, bool value);
    void SetUniform1i(const std::string& name, int value);
    void SetUniformMat4(const std::string& name, const glm::mat4& matrix);

    void SetUniform1f(const std::string& name, float value);
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    void SetUniformVec3(const std::string& name, const glm::vec3& vector);

private:
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;
    int GetUniformLocation(const std::string& name);
};
