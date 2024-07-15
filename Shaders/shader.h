#ifndef SHADER_H
#define SHADER_H
#include <string>

class Shader {
public:
    /// the shader program id
    unsigned int id;

    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
};

#endif // SHADER_H
