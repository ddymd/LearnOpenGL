#include <spdlog/spdlog.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
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
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); ++j) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

unsigned int Model::TextureFromFile(const char *path, const std::string &directory, bool gamma) {
    std::string filename = fmt::format("{}/{}", directory, path);
    unsigned int textureid;
    glGenTextures(1, &textureid);
    int w, h, c;
    unsigned char* data = stbi_load(filename.c_str(), &w, &h, &c, 0);
    if (data) {
        GLenum format = GL_RGB;
        switch (c) {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default:
            SPDLOG_WARN("unhandled format: {}", c);
            break;
        }
        glBindTexture(GL_TEXTURE_2D, textureid);
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else {
        SPDLOG_ERROR("Texture failed to load at path: {}", path);
    }
    return textureid;
}
