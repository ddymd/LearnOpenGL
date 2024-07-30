#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "config.h"
#include "shader.h"

#define SRC_VERTEX_SHADER TOP_SRC_DIR"Lighting/colors/shader.vs"
#define SRC_FRAGMENT_SHADER TOP_SRC_DIR"Lighting/colors/shader.fs"

void glfwErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR_CB({}): {}", ec, emsg);
}

void glfwResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char** argv) {
    if (!glfwInit()) {
        SPDLOG_CRITICAL("GLFW Init Failed");
        return -1;
    }

    // set error callback
    glfwSetErrorCallback(glfwErrorCB);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// for apple
#endif	// __APPLE__

    GLFWwindow* window = glfwCreateWindow(800, 600, "Lighting Colors", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("glfw create window failed");
        return -2;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("Load proc Failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }
    glfwSetFramebufferSizeCallback(window, glfwResizeCB);

    // VAO, VBO;
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader sp(SRC_VERTEX_SHADER, SRC_FRAGMENT_SHADER);
    sp.use();
    sp.setVec3("objColor", 1.0f, 0.5f, 0.31f);
    sp.setVec3("litColor", 1.0f, 1.0f, 1.0f);

    glm::mat4 mmodel(1.f);
    mmodel = glm::translate(mmodel, glm::vec3(0.f, 0.f, 0.f));
    mmodel = glm::rotate(mmodel, glm::radians(30.f), {1.f, 0.3f, 0.3f});
    sp.setMat4("model", mmodel);

    glm::mat4 mview(1.f);
    mview = glm::translate(mview, glm::vec3(0.f, 0.f, -3.f));
    sp.setMat4("view", mview);

    glm::mat4 mproj(1.f);
    mproj = glm::perspective(glm::radians(45.f), 4.f/3.f, 0.1f, 100.f);
    sp.setMat4("proj", mproj);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
