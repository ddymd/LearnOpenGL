#include <spdlog/spdlog.h>
#include "model.hpp"

Model::Model(const char* path) {
    loadModel(path);
}

void Model::Draw(Shader* shader) {
    for (unsigned int i = 0; i < meshes.size(); ++i) {
        meshes[i].Draw(shader);
    }
}

void Model::loadModel(const char* path) {
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        SPDLOG_CRITICAL("ERROR::ASSIMP::{}", import.GetErrorString());
        return;
    }
    directory = std::string(path).substr(0, std::string(path).find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    for (int i = 0; i < mesh->mNumVertices; ++i) {
        float coordx = 0.f, coordy = 0.f;
        if (mesh->mTextureCoords[0]) {
            coordx = mesh->mTextureCoords[0][i].x;
            coordy = mesh->mTextureCoords[0][i].y;
        }
        vertices.push_back({
                            {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].y},
                            {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
                            {coordx, coordy}
        });
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[i]);
        }
    }

    if (mesh->mMaterialIndex > 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffusemaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffusemaps.begin(), diffusemaps.end());
        std::vector<Texture> specularmaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularmaps.begin(), specularmaps.end());
    }
    return {vertices, indices, textures};
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    // for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
    //     aiString str;
    //     mat->GetTexture(type, i, &str);
    //     bool skip = false;
    //     for (unsigned int j = 0; j < textures_loaded.size();)

    //     Texture texture;
    //     textures.push_back({TextureFromFile(str.C_Str(),directory), typeName, str.C_Str()});
    // }
    return textures;
}
