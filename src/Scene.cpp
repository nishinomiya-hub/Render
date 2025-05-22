// Scene.cpp
#include "Scene.h"

void Scene::AddModel(const std::string& path, glm::vec3 position) {
    auto node = std::make_shared<SceneNode>();
    node->model = std::make_shared<Model>(path);
    node->Position = position;
    nodes.push_back(node);
}

std::shared_ptr<SceneNode> Scene::GetNode(size_t index)
{
    if (index >= 0 && index < nodes.size()) {
        return nodes[index];
    }
    return nullptr;
}

void Scene::Render(const Camera& camera, Shader& shader) {
    shader.Bind();
    shader.SetUniformMat4("view", camera.GetViewMatrix());
    shader.SetUniformMat4("projection", camera.GetProjectionMatrix());

    for (const auto& node : nodes) {
        shader.SetUniformMat4("model", node->GetModelMatrix());
        node->model->Draw(shader);
    }
}
