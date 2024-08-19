#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <string>
#include <vector>
#include <glm/glm.hpp>

class Shader;
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader* shader);

private:
    void setupMesh();

private:
    unsigned int VAO, VBO, EBO;
};

#endif // __MESH_HPP__

