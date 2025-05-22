#pragma once
#include "Material.h"
#include <vector>
#include <glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::shared_ptr< Material > material);
    ~Mesh();
    void Draw(Shader& shader) const;
private:
    unsigned int VAO, VBO, EBO;
    unsigned int m_IndexCount;
    std::shared_ptr<Material> m_material;
};
