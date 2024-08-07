#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "config.h"
#include "camera.h"
#include "shader.h"

#define SRC_VSHADER_OBJ TOP_SRC_DIR"Colors/Materials/obj_shader.vs"
#define SRC_FSHADER_OBJ TOP_SRC_DIR"Colors/Materials/obj_shader.fs"
#define SRC_VSHADER_LIT TOP_SRC_DIR"Colors/Materials/lit_shader.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Colors/Materials/lit_shader.fs"

glm::vec3 litPos(1.2f, 1.f, 2.f);
glm::vec3 litColor(1.0f, 1.0f, 1.0f);
glm::vec3 objColor(1.0f, 0.5f, 0.31f);

Camera mcam(glm::vec3(0.f, 0.f, 5.f));

void GLFWErrorCB(int ec, const char* emsg);
void GLFWResizeCB(GLFWwindow* window, int w, int h);
void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos);
void GLFWCursorScrollCB(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInputs(GLFWwindow* window, float delta);

int main(int argc, char** argv) {
    if (!glfwInit()) {
        SPDLOG_CRITICAL("GLFW Init Failed");
        return -1;
    }
    glfwSetErrorCallback(GLFWErrorCB);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for apple
#endif // __APPLE__
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Materials", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("GLFW Create Window Failed");
        return -2;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("GLAD Load Proc Addr Failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }
    glfwSetFramebufferSizeCallback(window, GLFWResizeCB);
    glEnable(GL_DEPTH_TEST);

    float lframe = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

/**
 * @brief GLFWErrorCB
 * @param ec
 * @param emsg
 */
void GLFWErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW ERROR CB({}): {}", ec, emsg);
}

void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos) {

}

void GLFWCursorScrollCB(GLFWwindow* window, double xoffset, double yoffset) {

}

void ProcessInputs(GLFWwindow* window, float delta) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}
