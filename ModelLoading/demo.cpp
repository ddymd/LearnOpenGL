#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "common.hpp"
#include "config.h"

#define SRC_VSHADER TOP_SRC_DIR"ModelLoading/shader.vs"
#define SRC_FSHADER TOP_SRC_DIR"ModelLoading/shader.fs"

Camera mcam(glm::vec3(0.f, 0.f, 3.f));

int main(int argc, char* argv[]) {
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Model Loading", NULL, NULL);
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

    Model loadmodel(MODEL_BACKPACK_DIR"/backpack.obj");

    Shader sp(SRC_VSHADER, SRC_FSHADER);
    sp.use();
    sp.setMat4("model", glm::mat4(1.f));
    sp.setFloat("material.shininess", 32.f);
    sp.setVec3("dirlight.direction", litdirection);
    sp.setVec3("dirlight.ambient", styleFactory.DLColors[0]);
    sp.setVec3("dirlight.diffuse", styleFactory.DLColors[1]);
    sp.setVec3("dirlight.specular", styleFactory.DLColors[2]);

    sp.setVec3("spotlight.ambient", styleFactory.SLColors[0]);
    sp.setVec3("spotlight.diffuse", styleFactory.SLColors[1]);
    sp.setVec3("spotlight.specular", styleFactory.SLColors[2]);
    sp.setFloat("spotlight.constant", styleFactory.SLAttenuations[0]);
    sp.setFloat("spotlight.linear", styleFactory.SLAttenuations[1]);
    sp.setFloat("spotlight.quadratic", styleFactory.SLAttenuations[2]);
    sp.setFloat("spotlight.innercutoff", styleFactory.SLIntensities[0]);
    sp.setFloat("spotlight.outercutoff", styleFactory.SLIntensities[1]);

    // glm::mat4 model(1.f);
    // model = glm::translate(model, glm::vec3(0.f, 0.f, -3.f));
    // model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
    // sp.setMat4("model", model);

    float lframe = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sp.use();
        sp.setMat4("view", mcam.GetViewMatrix());
        glm::mat4 proj(1.f);
        proj = glm::perspective(mcam.Zoom, (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);
        sp.setMat4("proj", proj);
        sp.setVec3("camPos", mcam.Position);
        sp.setVec3("spotlight.direction", mcam.Front);
        sp.setVec3("spotlight.position", mcam.Position);
        loadmodel.Draw(&sp);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
