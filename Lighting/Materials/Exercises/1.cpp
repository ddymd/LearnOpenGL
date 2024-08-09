// Can you make it so that changing the light color changes the color of the light’s cube object?

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "camera.h"
#include "config.h"

#define SRC_VSHADER_OBJ TOP_SRC_DIR"Lighting/Materials/Exercises/obj_shader1.vs"
#define SRC_FSHADER_OBJ TOP_SRC_DIR"Lighting/Materials/Exercises/obj_shader1.fs"

#define SRC_VSHADER_LIT TOP_SRC_DIR"Lighting/Materials/Exercises/lit_shader1.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Lighting/Materials/Exercises/lit_shader1.fs"

Camera mcam(glm::vec3(0.f, 0.f, 5.f));

void GLFWErrorCB(int ec, const char* emsg);
void ProcessInputs(GLFWwindow* window, float delta);
void GLFWResizeCB(GLFWwindow* window, int w, int h);
void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos);
void GLFWScrollCB(GLFWwindow* window, double xoffset, double yoffset);

// glm::vec3 objColor(1.0f, 0.5f, 0.31f);

glm::vec3 mambient(1.0f, 0.5f, 0.31f);
glm::vec3 mdiffuse(1.0f, 0.5f, 0.31f);
glm::vec3 mspecular(0.5f, 0.5f, 0.5f);
float mshininess(32.0f);

glm::vec3 lambient(0.2f, 0.2f, 0.2f);
glm::vec3 ldiffuse(0.5, 0.5f, 0.5f);
glm::vec3 lspecular(1.f, 1.f, 1.f);
glm::vec3 lposition(1.2f, 1.f, 2.f);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Materials Exercise1", NULL, NULL);
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
    glfwSetFramebufferSizeCallback(window, GLFWResizeCB);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, GLFWCursorPosCB);
    glfwSetScrollCallback(window, GLFWScrollCB);

    unsigned int VAO[2], VBO;
    // for object
    glGenVertexArrays(2, VAO);
    glBindVertexArray(VAO[0]);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norvertices), norvertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // for light
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader objsp(SRC_VSHADER_OBJ, SRC_FSHADER_OBJ);
    objsp.use();
    objsp.setMat4("model", glm::mat4(1.f));
    objsp.setVec3("material.ambient", mambient);
    objsp.setVec3("material.diffuse", mdiffuse);
    objsp.setVec3("material.specular", mspecular);
    objsp.setFloat("material.shininess", mshininess);

    objsp.setVec3("light.ambient", lambient);
    objsp.setVec3("light.diffuse", ldiffuse);
    objsp.setVec3("light.specular", lspecular);
    objsp.setVec3("light.position", lposition);

    Shader litsp(SRC_VSHADER_LIT, SRC_FSHADER_LIT);
    litsp.use();
    glm::mat4 lmodel(1.f);
    lmodel = glm::translate(lmodel, lposition);
    lmodel = glm::scale(lmodel, glm::vec3(0.3f));
    litsp.setMat4("model", lmodel);
    litsp.setVec3("light.ambient", lambient);
    litsp.setVec3("light.diffuse", ldiffuse);
    litsp.setVec3("light.specular", lspecular);

    float lframe = 0.f;

    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mview = mcam.GetViewMatrix();
        glm::mat4 mproj = glm::perspective(glm::radians(mcam.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);

        glm::vec3 diffuseColor = glm::vec3(sin(cframe * 2.0f), sin(cframe * 0.7f), sin(cframe * 1.3f)) * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        objsp.use();
        objsp.setMat4("view", mview);
        objsp.setMat4("proj", mproj);
        objsp.setVec3("light.ambient", ambientColor);
        objsp.setVec3("light.diffuse", diffuseColor);

        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        litsp.use();
        litsp.setMat4("view", mview);
        litsp.setMat4("proj", mproj);
        litsp.setVec3("light.ambient", ambientColor);
        litsp.setVec3("light.diffuse", diffuseColor);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

///////////////////////////////////////////////////////////////////////////
void GLFWErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW ERROR CALLBACK({}): {}", ec, emsg);
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

float lxpos, lypos;
bool bIsFirstCursor = true;
void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos) {
    if (bIsFirstCursor) {
        bIsFirstCursor = false;
    } else {
        mcam.ProcessMouseMovement(xpos-lxpos, ypos-lypos);
    }
    lxpos = xpos;
    lypos = ypos;
}

void GLFWScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
    mcam.ProcessMouseScroll(yoffset);
}


