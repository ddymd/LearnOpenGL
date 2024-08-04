// Implement Gouraud shading instead of Phong shading. If you did things right the lighting
// should look a bit off as you can see at: learnopengl.com/img/lighting/
// basic_lighting_exercise3.png (especially the specular highlights) with the cube
// object. Try to reason why it looks so weird. Solution: /src/2.lighting/2.5.basic_
// lighting_exercise3/.

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>

#include "config.h"
#include "camera.h"
#include "shader.h"

Camera mcam(glm::vec3(0.f, 0.f, 5.f));

void GLFWErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR_CB({}): {}", ec, emsg);
}

void ProcessInputs(GLFWwindow* window, float delta) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        mcam.ProcessKeyboard(ECameraMovement::FORWARD, delta);
    }
    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        mcam.ProcessKeyboard(ECameraMovement::BACKWARD, delta);
    }
    
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        mcam.ProcessKeyboard(ECameraMovement::LEFT, delta);
    }
    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        mcam.ProcessKeyboard(ECameraMovement::RIGHT, delta);
    }
}

void GLFWResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

bool isFirstCursor = true;
float lxpos, lypos;
void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos) {
    if (isFirstCursor) {
        isFirstCursor = false;
    } else {
        mcam.ProcessMouseMovement(xpos-lxpos, ypos-lypos);
    }
    lxpos = xpos;
    lypos = ypos;
}

void GLFWScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
    mcam.ProcessMouseScroll(yoffset);
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
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "Lighting colors exercises 4", NULL, NULL);
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
    glfwSetFramebufferSizeCallback(window, GLFWResizeCB);
    glfwSetScrollCallback(window, GLFWScrollCB);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, GLFWCursorPosCB);
    glEnable(GL_DEPTH_TEST);
    
    float lframe = 0.f;
    
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
