#include "Model.h"

Model::Model(const std::string& path)
{
    loadModel(path);
}

void Model::Draw(Shader &shader)
{
    for (auto& mesh : meshes) {
        mesh->Draw(shader);
    }
}

void Model::loadModel(const std::string& path)
{
    Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        //std::cout << "Mesh " << i << " uses material " << mesh->mMaterialIndex << "\n";
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    // std::cout << "Mesh has " << mesh->mNumVertices << " vertices\n";
    // if (mesh->HasTextureCoords(0)) {
    //     std::cout << "Mesh has texture coordinates\n";
    // } else {
    //     std::cout << "Mesh has NO texture coordinates\n";
    // }
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;

        vertex.position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        vertex.normal = mesh->HasNormals()
            ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
            : glm::vec3(0.0f);

        if (mesh->mTextureCoords[0]) {
            vertex.texCoords = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        } else {
            vertex.texCoords = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    std::shared_ptr<Material> material = std::make_shared<Material>();
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

        // 加载漫反射贴图
        material->diffuseMap = loadTexture(mat, aiTextureType_DIFFUSE);
        // 加载高光贴图
        material->specularMap = loadTexture(mat, aiTextureType_SPECULAR);

        material->normalMap   = loadTexture(mat, aiTextureType_NORMALS);
        material->opacityMap  = loadTexture(mat, aiTextureType_OPACITY);
        material->emissiveMap = loadTexture(mat, aiTextureType_EMISSIVE);

        aiColor3D color;
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
            material->diffuseColor = glm::vec3(color.r, color.g, color.b);
        }
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
            material->specularColor = glm::vec3(color.r, color.g, color.b);
        }
        float shininess;
        if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS, shininess)) {
            material->shininess = shininess;
        }
    }

    return std::make_shared<Mesh>(vertices, indices, material);
}

std::shared_ptr<Texture> Model::loadTexture(aiMaterial *mat, aiTextureType type)
{
    if (mat->GetTextureCount(type) == 0) return nullptr;

    aiString str;
    mat->GetTexture(type, 0, &str);

    std::string texPath = str.C_Str();

    // 判断路径是否为绝对路径
    std::filesystem::path p(texPath);
    if (!p.is_absolute()) {
        texPath = directory + "/" + texPath;
    }

    // 可选：输出调试信息
    //std::cout << "Loading texture from: " << texPath << std::endl;

    return std::make_shared<Texture>(texPath);
}
