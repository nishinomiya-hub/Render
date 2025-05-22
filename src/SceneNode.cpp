#include "SceneNode.h"

glm::mat4 SceneNode::GetModelMatrix() const {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), Position);
    glm::mat4 rotation = glm::toMat4(Rotation);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), Scale);
    return translation * rotation * scale;
}