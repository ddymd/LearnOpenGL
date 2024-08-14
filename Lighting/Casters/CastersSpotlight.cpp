#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "common.hpp"
#include "config.h"

#define SRC_VSHADER TOP_SRC_DIR"Lighting/Casters/spotlight_shader.vs"
#define SRC_FSHADER TOP_SRC_DIR"Lighting/Casters/spotlight_shader.fs"

Camera mcam(glm::vec3(0.f, 0.f, 3.f));
float mshininess = 32.f;
glm::vec3 lambient(0.2f);
glm::vec3 ldiffuse(0.5f);
glm::vec3 lspecular(1.f);
glm::vec3 lposition(1.2f, 1.f, 2.f);

int main(int argc, char const *argv[])
{
    if (!glfwInit()) {
        SPDLOG_CRITICAL("GLFW Init Failed");
        return -1;
    }
    SetCamera(&mcam);
    glfwSetErrorCallback(GLFWErrorCB);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for apple
#endif // __APPLE__
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Casters Spotlight", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("GLFW Create Window Failed");
        return -2;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("GLAD Load Proc Failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }
    glfwSetFramebufferSizeCallback(window, GLFWFrameResizeCB);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, GLFWCursorPosCB);
    glfwSetScrollCallback(window, GLFWScrollCB);
    glEnable(GL_DEPTH_TEST);

    float lfm = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cfm = glfwGetTime();
        ProcessInputs(window, cfm - lfm);
        lfm = cfm;
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
