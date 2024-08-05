// Right now the light source is a boring static light source that doesn’t move. Try to move
// the light source around the scene over time using either sin or cos. Watching the lighting
// change over time gives you a good understanding of Phong’s lighting model. Solution:
// /src/2.lighting/2.3.basic_lighting_exercise1/.

#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "config.h"
#include "shader.h"
#include "camera.h"

#define SRC_VSHADER_OJB TOP_SRC_DIR"Lighting/Colors/Exercises/obj_shader1.vs"
#define SRC_FSHADER_OJB TOP_SRC_DIR"Lighting/Colors/Exercises/obj_shader1.fs"
#define SRC_VSHADER_LIT TOP_SRC_DIR"Lighting/Colors/Exercises/lit_shader1.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Lighting/Colors/Exercises/lit_shader1.fs"

glm::vec3 litPos(1.2f, 1.f, 2.f);
glm::vec3 litColor(1.0f, 1.0f, 1.0f);
glm::vec3 objColor(1.0f, 0.5f, 0.31f);
Camera viewCam(glm::vec3(0.f, 0.f, 5.f));

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
        viewCam.ProcessKeyboard(ECameraMovement::FORWARD, delta);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        viewCam.ProcessKeyboard(ECameraMovement::BACKWARD, delta);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        viewCam.ProcessKeyboard(ECameraMovement::LEFT, delta);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        viewCam.ProcessKeyboard(ECameraMovement::RIGHT, delta);
    }
}

bool bIsFirstCursor = true;
float lastxpos, lastypos;
void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos) {
    if (bIsFirstCursor) {
        bIsFirstCursor = false;
    } else {
        viewCam.ProcessMouseMovement(xpos-lastxpos, ypos-lastypos);
    }
    lastxpos = xpos;
    lastypos = ypos;
}

void GLFWCursorScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
    viewCam.ProcessMouseScroll(yoffset);
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "Lighting Colors Exercises 1", NULL, NULL);
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
    glfwSetScrollCallback(window, GLFWCursorScrollCB);
    glEnable(GL_DEPTH_TEST);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norvertices), norvertices, GL_STATIC_DRAW);
    // vertex array
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal array
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // coord array
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int VAO_LIT;
    glGenVertexArrays(1, &VAO_LIT);
    glBindVertexArray(VAO_LIT);
    glGenBuffers(1, &VBO);
    // vertex array
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // object program shader
    Shader OBJ_SP(SRC_VSHADER_OJB, SRC_FSHADER_OJB);
    OBJ_SP.use();
    OBJ_SP.setVec3("objColor", objColor);
    OBJ_SP.setVec3("litColor", litColor);
    OBJ_SP.setMat4("model", glm::mat4(1.f));

    // ligth program shader
    Shader LIT_SP(SRC_VSHADER_LIT, SRC_FSHADER_LIT);
    LIT_SP.use();
    LIT_SP.setVec3("litColor", litColor);
    glm::mat4 mlit(1.f);
    mlit = glm::translate(mlit, litPos);
    mlit = glm::scale(mlit, glm::vec3(0.2f));
    LIT_SP.setMat4("model", mlit);

    float lastFrame = 0;
    float radius = 3.f;
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currFrame = glfwGetTime();
        ProcessInput(window, currFrame-lastFrame);
        lastFrame = currFrame;

        litPos.x = sin(glfwGetTime()) * radius;
        litPos.z = cos(glfwGetTime()) * radius;

        glm::mat4 mview = viewCam.GetViewMatrix();
        glm::mat4 mproj = glm::perspective(glm::radians(viewCam.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);

        OBJ_SP.use();
        OBJ_SP.setMat4("view", mview);
        OBJ_SP.setMat4("proj", mproj);
        OBJ_SP.setVec3("litPos", litPos);
        OBJ_SP.setVec3("viewPos", viewCam.Position);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        LIT_SP.use();
        mlit = glm::mat4(1.f);
        mlit = glm::translate(mlit, litPos);
        mlit = glm::scale(mlit, glm::vec3(0.2f));
        LIT_SP.setMat4("model", mlit);
        LIT_SP.setMat4("view", mview);
        LIT_SP.setMat4("proj", mproj);
        glBindVertexArray(VAO_LIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
