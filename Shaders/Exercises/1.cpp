// Adjust the vertex shader so that the triangle is upside down. Solution: /src/1.getting_
//  started/3.4.shaders_exercise1/shaders_exercise1.cpp.

#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "spdlog/spdlog.h"

#include "shader.h"
#include "config.h"

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
    if (!glfwInit()) {
        SPDLOG_CRITICAL("glfw initialize failed");
        return -1;
    }

    glfwSetErrorCallback(errorCallback);

    // create window and context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__

    GLFWwindow *window = glfwCreateWindow(800, 600, "Shaders-Exercises1", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("create glfw window failed");
        return -2;
    }
    glfwMakeContextCurrent(window);

    // glad initialization
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("load glad proc failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }

    glfwSetFramebufferSizeCallback(window, resizeCallback);

    Shader upsidedown(TOP_SRC_DIR"Shaders/upsidedown.vs", TOP_SRC_DIR"Shaders/shader.fs");
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // GLenum target, GLsizeiptr size, const void *data, GLenum usage
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    upsidedown.use();
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

