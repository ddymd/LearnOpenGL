// Specify a horizontal offset via a uniform and move the triangle to the right side of the screen
// in the vertex shader using this offset value. Solution /src/1.getting_started/3.5.
// shaders_exercise2/shaders_exercise2.cpp.

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

void glfwErrorCallback(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR({}): {}", ec, emsg);
}

void glfwWindowResize(GLFWwindow* window, int w, int h) {
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

    glfwSetErrorCallback(glfwErrorCallback);

    // create window and context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__
    GLFWwindow *window = glfwCreateWindow(800, 600, "Shaders-Exercises2", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("create glfw window failed");
        return -2;
    }
    // make context
    glfwMakeContextCurrent(window);

    // init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("glad load proc failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }

    glfwSetFramebufferSizeCallback(window, glfwWindowResize);

    Shader ourShader(TOP_SRC_DIR"Shaders/uniformright.vs", TOP_SRC_DIR"Shaders/shader.fs");
    ourShader.use();
    ourShader.setFloat("moveRight", 0.5);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

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
