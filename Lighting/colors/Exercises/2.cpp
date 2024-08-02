// Play around with different ambient, diffuse and specular strengths and see how they impact
// the result. Also experiment with the shininess factor. Try to comprehend why certain values
// have a certain visual output.

#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "config.h"
#include "shader.h"
#include "camera.h"

#define SRC_VSHADER_OBJ TOP_SRC_DIR"Lighting/colors/Exercises/obj_shader1.vs"
#define SRC_FSHADER_OBJ TOP_SRC_DIR"Lighting/colors/Exercises/obj_shader1.fs"

#define SRC_VSHADER_LIT TOP_SRC_DIR"Lighting/colors/Exercises/lit_shader1.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Lighting/colors/Exercises/lit_shader1.fs"

Camera mcamera(glm::vec3(0.f, 0.f, 5.f));

void GLFWErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR_CB({}): {}", ec, emsg);
}

void GLFWFrameResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

void ProcessInput(GLFWwindow* window, float delta) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        mcamera.ProcessKeyboard(ECameraMovement::FORWARD, delta);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        mcamera.ProcessKeyboard(ECameraMovement::BACKWARD, delta);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        mcamera.ProcessKeyboard(ECameraMovement::LEFT, delta);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        mcamera.ProcessKeyboard(ECameraMovement::RIGHT, delta);
    }
}

bool bIsFirstCursor = true;
float xlastpos, ylastpos;
void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos) {
    if (bIsFirstCursor) {
        bIsFirstCursor = true;
    } else {
        mcamera.ProcessMouseMovement(xpos - xlastpos, ypos - ylastpos);
    }
    xlastpos = xpos;
    ylastpos = ypos;
}

void GLFWScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
    mcamera.ProcessMouseScroll(yoffset);
}

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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Lighting colors exerciese 2", NULL, NULL);
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
    glEnable(GL_DEPTH_TEST);

    float lastFrame = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        ProcessInput(window, currentFrame-lastFrame);
        lastFrame = currentFrame;

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
