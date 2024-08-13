#ifndef SHADER_HPP
#define SHADER_HPP

#include <glm/matrix.hpp>

class Shader {
public:
    /// the shader program id
    unsigned int id;

    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    void setBool(const char* name, bool value) const;
    void setInt(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setMat4(const char* name, const glm::mat4& value) const;
    void setVec3(const char* name, float v1, float v2, float v3) const;
    void setVec3(const char* name, const glm::vec3& v3) const;
};

#endif // SHADER_HPP
