#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <memory>
#include "Model.h"
#include <gtc/matrix_transform.hpp>
#include <gtx/quaternion.hpp>

class SceneNode {
public:
    glm::vec3 Position = {0, 0, 0};
    glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 Scale = {1, 1, 1};

    std::shared_ptr<Model> model;

    glm::mat4 GetModelMatrix() const;
};