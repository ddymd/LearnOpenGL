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

#define SRC_VSHADER_OBJ TOP_SRC_DIR"Lighting/Colors/Exercises/obj_shader2.vs"
#define SRC_FSHADER_OBJ TOP_SRC_DIR"Lighting/Colors/Exercises/obj_shader2.fs"

#define SRC_VSHADER_LIT TOP_SRC_DIR"Lighting/Colors/Exercises/lit_shader1.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Lighting/Colors/Exercises/lit_shader1.fs"

glm::vec3 litPos(1.2f, 1.f, 2.f);
glm::vec3 litColor(1.0f, 1.0f, 1.0f);
glm::vec3 objColor(1.0f, 0.5f, 0.31f);
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
        bIsFirstCursor = false;
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
    glfwSetFramebufferSizeCallback(window, GLFWFrameResizeCB);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, GLFWCursorPosCB);
    glfwSetScrollCallback(window, GLFWScrollCB);
    glEnable(GL_DEPTH_TEST);

    unsigned int VAO[2], VBO;
    glGenVertexArrays(2, VAO);
    glBindVertexArray(VAO[0]);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norvertices), norvertices, GL_STATIC_DRAW);
    // vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal vectors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader obj_sp(SRC_VSHADER_OBJ, SRC_FSHADER_OBJ);
    obj_sp.use();
    obj_sp.setVec3("objColor", objColor);
    obj_sp.setVec3("litColor", litColor);
    obj_sp.setVec3("litPos", litPos);
    obj_sp.setMat4("model", glm::mat4(1.f));

    Shader lit_sp(SRC_VSHADER_LIT, SRC_FSHADER_LIT);
    lit_sp.use();
    lit_sp.setVec3("litColor", litColor);

    glm::mat4 lmodel(1.f);
    lmodel = glm::translate(lmodel, litPos);
    lmodel = glm::scale(lmodel, glm::vec3(0.3f));
    lit_sp.setMat4("model", lmodel);

    float lastFrame = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        ProcessInput(window, currentFrame-lastFrame);
        lastFrame = currentFrame;

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mview = mcamera.GetViewMatrix();
        glm::mat4 mproj = glm::perspective(glm::radians(mcamera.Zoom), 800.f/600.f, 0.1f, 100.f);

        // draw object
        obj_sp.use();
        obj_sp.setMat4("view", mview);
        obj_sp.setMat4("proj", mproj);
        obj_sp.setVec3("viewPos", mcamera.Position);

        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw light
        lit_sp.use();
        lit_sp.setMat4("view", mview);
        lit_sp.setMat4("proj", mproj);

        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
