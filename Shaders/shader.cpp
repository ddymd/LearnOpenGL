#include "shader.h"
#include "glad/glad.h"

#include <fstream>
#include <sstream>

#include "spdlog/spdlog.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::ifstream vShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    vShaderFile.open(vertexPath);
    std::stringstream vShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    vShaderFile.close();
    // convert stream to string
    std::string vertexCode = vShaderStream.str();


    std::ifstream fShaderFile;
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    fShaderFile.open(fragmentPath);
    std::stringstream fShaderStream;
    fShaderStream << fShaderFile.rdbuf();
    fShaderFile.close();
    // convert stream to string
    std::string fragmentCode = fShaderStream.str();

    const char* vShaderCode = vertexCode.c_str();

    // 2. compile shaders
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // error checking
    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::VERTEX::SHADER::COMPILE_FAILED {}", infoLog);
    }

    const char* fShaderCode = fragmentCode.c_str();
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::FRAGMENT::SHADER::COMPILE_FAILED {}", infoLog);
    }

    // shader program
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::PROGRAM::LINK_FAILED {}", infoLog);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(id);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
