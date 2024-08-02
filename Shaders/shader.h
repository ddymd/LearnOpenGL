#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm/matrix.hpp>

class Shader {
public:
    /// the shader program id
    unsigned int id;

    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setVec3(const std::string& name, float v1, float v2, float v3) const;
    void setVec3(const std::string& name, const glm::vec3& v3) const;
};

#endif // SHADER_H
