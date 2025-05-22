#include "Light.h"

void  Light::addDrirectionalLight(const DirectionalLight& light)
{
    directionalLights.push_back(light);
}

void Light::addPointLight(const PointLight& light)
{
    pointLights.push_back(light);
}

void Light::addSpotLight(const SpotLight& light)
{
    spotLights.push_back(light);
}

void Light::uploadToShader(Shader &shader)
{
    shader.Bind();

    for(size_t i = 0; i < directionalLights.size(); i++)
    {
        shader.SetUniformVec3("directionalLights[" + std::to_string(i) + "].direction", directionalLights[i].direction);
        shader.SetUniformVec3("directionalLights[" + std::to_string(i) + "].ambient", directionalLights[i].ambient);
        shader.SetUniformVec3("directionalLights[" + std::to_string(i) + "].diffuse", directionalLights[i].diffuse);
        shader.SetUniformVec3("directionalLights[" + std::to_string(i) + "].specular", directionalLights[i].specular);
    }
    shader.SetUniform1i("directionalLights.size", directionalLights.size());

    for(size_t i = 0; i < pointLights.size(); i++)
    {
        shader.SetUniformVec3("pointLights[" + std::to_string(i) + "].position", pointLights[i].position);
        shader.SetUniformVec3("pointLights[" + std::to_string(i) + "].ambient", pointLights[i].ambient);
        shader.SetUniformVec3("pointLights[" + std::to_string(i) + "].diffuse",pointLights[i].diffuse);
        shader.SetUniform1f("pointLights[" + std::to_string(i) + "].constant", pointLights[i].constant);
        shader.SetUniform1f("pointLights[" + std::to_string(i) + "].linear", pointLights[i].linear);
        shader.SetUniform1f("pointLights[" + std::to_string(i) + "].quadratic", pointLights[i].quadratic);
        shader.SetUniformVec3("pointLights[" + std::to_string(i) + "].specular", pointLights[i].specular);
    }
    shader.SetUniform1i("pointLights.size", pointLights.size());

    
}