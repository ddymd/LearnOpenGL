// Do Phong shading in view space instead of world space. Solution: /src/2.lighting/
// 2.4.basic_lighting_exercise2/.

#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "config.h"
#include "camera.h"
#include "shader.h"

#define SRC_VSHADER_OBJ TOP_SRC_DIR"Lighting/Colors/Exercises/obj_shader3.vs"
#define SRC_FSHADER_OBJ TOP_SRC_DIR"Lighting/Colors/Exercises/obj_shader3.fs"
#define SRC_VSHADER_LIT TOP_SRC_DIR"Lighting/Colors/Exercises/lit_shader3.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Lighting/Colors/Exercises/lit_shader3.fs"

glm::vec3 litPos(1.2f, 1.f, 2.f);
glm::vec3 litColor(1.0f, 1.0f, 1.0f);
glm::vec3 objColor(1.0f, 0.5f, 0.31f);
Camera mcam(glm::vec3(0.f, 0.f, 5.f));

void GLFWErrorCB(int ec, const char* emsg);
void GLFWResizeCB(GLFWwindow* window, int w, int h);
void ProcessInputs(GLFWwindow* window, float delta);
bool bIsFirstCursor = true;
float xlpos, ylpos;
void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos);
void GLFWCursorScrollCB(GLFWwindow* window, double xoffset, double yoffset);

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
    GLFWwindow* window = glfwCreateWindow(800, 600, "Lighting Colors Exerciese3", NULL, NULL);
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
    glfwSetScrollCallback(window, GLFWCursorScrollCB);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, GLFWCursorPosCB);
    glEnable(GL_DEPTH_TEST);

    unsigned int VAO[2], VBO;
    glGenVertexArrays(2, VAO);
    glBindVertexArray(VAO[0]);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norvertices), norvertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader objsp(SRC_VSHADER_OBJ, SRC_FSHADER_OBJ);
    objsp.use();
    objsp.setVec3("objColor", objColor);
    objsp.setVec3("litColor", litColor);
    objsp.setVec3("litPos", litPos);
    objsp.setMat4("model", glm::mat4(1.f));

    Shader litsp(SRC_VSHADER_LIT, SRC_FSHADER_LIT);
    litsp.use();
    litsp.setVec3("litColor", litColor);
    glm::mat4 lmodel(1.f);
    lmodel = glm::translate(lmodel, litPos);
    lmodel = glm::scale(lmodel, glm::vec3(0.3f));
    litsp.setMat4("model", lmodel);

    float framel = 0.f;
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mview = mcam.GetViewMatrix();
        glm::mat4 mproj = glm::perspective(glm::radians(mcam.Zoom), 800.f/600.f, 0.1f, 100.f);

        float framec = glfwGetTime();
        ProcessInputs(window, framec - framel);
        framel = framec;

        objsp.use();
        objsp.setMat4("view", mview);
        objsp.setMat4("proj", mproj);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        litsp.use();
        litsp.setMat4("view", mview);
        litsp.setMat4("proj", mproj);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void GLFWErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR_CB({}): {}", ec, emsg);
}

void GLFWResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
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

void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos) {
    if (bIsFirstCursor) {
        bIsFirstCursor = false;
    } else {
        mcam.ProcessMouseMovement(xpos-xlpos, ypos-ylpos);
    }
    xlpos = xpos;
    ylpos = ypos;
}

void GLFWCursorScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
    mcam.ProcessMouseScroll(yoffset);
}
