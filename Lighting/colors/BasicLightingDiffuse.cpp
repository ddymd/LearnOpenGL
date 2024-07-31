#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "config.h"
#include "shader.h"
#include "camera.h"

#define SRC_OBJ_VERTEX TOP_SRC_DIR"Lighting/colors/obj_shader.vs"
#define SRC_OBJ_FRAGMENT TOP_SRC_DIR"Lighting/colors/obj_shader.fs"

#define SRC_LIT_VERTEX TOP_SRC_DIR"Lighting/colors/lit_shader.vs"
#define SRC_LIT_FRAGMENT TOP_SRC_DIR"Lighting/colors/lit_shader.fs"

Camera mcam(glm::vec3(0.f, 0.f, 5.f));
void glfwErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR_CB({}): {}", ec, emsg);
}

void glfwResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow* window, float delta) {
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

bool bFirstCursor = true;
float lastX, lastY;
void glfwCursorPosCB(GLFWwindow* window, double xpos, double ypos) {
    if (bFirstCursor) {
        lastX = xpos;
        lastY = ypos;
        bFirstCursor = false;
    }

    float xoffset = xpos - lastX;
    lastX = xpos;
    float yoffset = ypos - lastY;
    lastY = ypos;

    mcam.ProcessMouseMovement(xoffset, yoffset);
}

void glfwScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
    mcam.ProcessMouseScroll(yoffset);
}

int main(int argc, char** argv) {
    // init glfw
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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for apple
#endif // __APPLE__

    GLFWwindow* window = glfwCreateWindow(800, 600, "Lighting Diffuse", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("GLFW Create Window Failed");
        return -2;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("GLAD Load GL Loader Failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }
    glEnable(GL_DEPTH_TEST);

    // set cursor position callback
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, glfwCursorPosCB);

    // set scroll callback
    glfwSetScrollCallback(window, glfwScrollCB);

    // VAO, VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norvertices), norvertices, GL_STATIC_DRAW);
    // attrib - vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // attrib - normal vector
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // attrib - coord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightsVAO;
    glGenVertexArrays(1, &lightsVAO);
    glBindVertexArray(lightsVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // attrib - vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glm::vec3 litpos(1.2f, 1.f, 2.f);

    // shader program
    Shader objsp(SRC_OBJ_VERTEX, SRC_OBJ_FRAGMENT);
    objsp.use();
    objsp.setVec3("objcolor", 1.0f, 0.5f, 0.31f);
    objsp.setVec3("litcolor", 1.0f, 1.0f, 1.0f);
    objsp.setVec3("litpos", litpos.x, litpos.y, litpos.z);
    glm::mat4 mmodel(1.f);
    objsp.setMat4("model", mmodel);

    Shader litsp(SRC_LIT_VERTEX, SRC_LIT_FRAGMENT);
    litsp.use();
    litsp.setVec3("litcolor", 1.0f, 1.0f, 1.0f);
    mmodel = glm::mat4(1.f);
    mmodel = glm::translate(mmodel, litpos);
    mmodel = glm::scale(mmodel, glm::vec3(0.2f));
    litsp.setMat4("model", mmodel);

    float lastFrame = 0;

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        processInput(window, currentFrame-lastFrame);
        lastFrame = currentFrame;
        glm::mat4 mview = mcam.GetViewMatrix();
        glm::mat4 mproj = glm::perspective(glm::radians(mcam.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);

        objsp.use();
        objsp.setVec3("viewpos", mcam.Position.x, mcam.Position.y, mcam.Position.z);
        objsp.setMat4("view", mview);
        objsp.setMat4("proj", mproj);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        litsp.use();
        litsp.setMat4("view", mview);
        litsp.setMat4("proj", mproj);
        glBindVertexArray(lightsVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

