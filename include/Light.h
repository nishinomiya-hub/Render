#pragma once
#include <glm.hpp>
#include "Shader.h"
#include <vector>

enum class LightTypes
{
	DIRECTIONAL,
	POINT,
	SPOT
};

struct BaseLight
{
    LightTypes type;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    /* data */
};

struct DirectionalLight : public BaseLight
{
    glm::vec3 direction;
    /* data */
};

struct PointLight : public BaseLight
{
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;
    /* data */
};

struct SpotLight : public PointLight
{
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    /* data */
};

class Light
{
public:

    void addDrirectionalLight(const DirectionalLight& light);
    void addPointLight(const PointLight& light);
    void addSpotLight(const SpotLight& light);

    void uploadToShader(Shader &shader);

private:
    std::vector<DirectionalLight> directionalLights;
    std::vector<PointLight> pointLights;
    std::vector<SpotLight> spotLights;
};
