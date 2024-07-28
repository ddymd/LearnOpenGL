// Output the vertex position to the fragment shader using the out keyword and set the frag-
// ment’s color equal to this vertex position (see how even the vertex position values are
// interpolated across the triangle). Once you managed to do this; try to answer the following
// question: why is the bottom-left side of our triangle black? Solution: /src/1.getting_
// started/3.6.shaders_exercise3/shaders_exercise3.cpp.

#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "spdlog/spdlog.h"

#include "shader.h"
#include "config.h"

float mvertices[] = {
    // positions // colors
    0.5f, -0.5f, 0.0f, //1.0f, 0.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, //0.0f, 1.0f, 0.0f, // bottom left
    0.0f, 0.5f, 0.0f, //0.0f, 0.0f, 1.0f // top
};

void glfwErrorCallback(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR::CALLBACK {}: {}", ec, emsg);
}

void glfwFramebuffersizeCallback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char** argv) {
    // init glfw
    if (!glfwInit()) {
        SPDLOG_CRITICAL("glfw initialize failed");
        return -1;
    }
    // create window and make context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // for apple
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Shaders-Exercises3", NULL, NULL);
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

    // shader program
    Shader shaderProgram(TOP_SRC_DIR"Shaders/shaderpos.vs", TOP_SRC_DIR"Shaders/shaderpos.fs");
    shaderProgram.use();

    // VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mvertices), mvertices, GL_STATIC_DRAW);

    // attributes
    // GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f,0.3f,0.3f,0.02f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

