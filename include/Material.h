#pragma once

#include <memory>
#include "Texture.h"
#include "Shader.h"

class Material {
public:
    glm::vec3 diffuseColor = glm::vec3(1.0f);
    glm::vec3 specularColor = glm::vec3(0.5f);
    float shininess = 32.0f;

    std::shared_ptr<Texture> diffuseMap;
    std::shared_ptr<Texture> specularMap;
    std::shared_ptr<Texture> normalMap;
    std::shared_ptr<Texture> opacityMap;
    std::shared_ptr<Texture> emissiveMap;

    Material() = default;
    void Apply(Shader& shader) const;
};
