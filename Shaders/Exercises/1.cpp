// Adjust the vertex shader so that the triangle is upside down. Solution: /src/1.getting_
//  started/3.4.shaders_exercise1/shaders_exercise1.cpp.

#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "spdlog/spdlog.h"

#include "shader.h"

float vertices[] = {
    // positions // colors
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
    0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f // top
};

void errorCallback(int error_code, const char* info) {
    SPDLOG_ERROR("glfw error callback({}): {}", error_code, info);
}

void resizeCallback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char** argv) {
    Shader unsidedown("", "");
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, );
    return 0;
}

