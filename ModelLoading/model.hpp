#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.hpp"

class Model {
public:
    Model(const char* path);
    void Draw(Shader* shader);

private:
    void loadModel(const char* path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

private:
    // model data
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;
};

#endif // __MODEL_HPP__
