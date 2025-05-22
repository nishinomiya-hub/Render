#pragma once
#include <vector>
#include <memory>
#include "SceneNode.h"
#include "Camera.h"
#include "Shader.h"

class Scene {
public:
    void AddModel(const std::string& path, glm::vec3 position);
    std::shared_ptr<SceneNode> GetNode(size_t index);
    void Render(const Camera& camera, Shader& shader);

private:
    std::vector<std::shared_ptr<SceneNode>> nodes;
};