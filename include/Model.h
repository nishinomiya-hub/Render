#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
    explicit Model(const std::string& path);
    void Draw(Shader& shader);
    
private:
    std::vector<std::shared_ptr<Mesh> > meshes;
    std::string directory;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
    std::shared_ptr< Texture> loadTexture(aiMaterial* mat, aiTextureType type);
};