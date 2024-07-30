#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "config.h"
#include "shader.h"
#include "camera.h"

#define SRC_VERTEX_SHADER TOP_SRC_DIR"Lighting/colors/shader.vs"
#define SRC_FRAGMENT_SHADER TOP_SRC_DIR"Lighting/colors/shader.fs"
#define SRC_FRAGMENT_SHADER_LIGHT TOP_SRC_DIR"Lighting/colors/shader_light.fs"

Camera camera({0.f, 0.f, 3.f});
float lastX = SCR_WIDTH / 2.f;
float lastY = SCR_HEIGHT / 2.f;
bool bFirstMouse = true;

// timing
float deltaTime = 0.f;
float lastFrame = 0.f;

void glfwErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR_CB({}): {}", ec, emsg);
}

void glfwResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(ECameraMovement::FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(ECameraMovement::BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(ECameraMovement::LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(ECameraMovement::RIGHT, deltaTime);
    }
}

void cursorPosCB(GLFWwindow* window, double xpos, double ypos) {
    if (bFirstMouse) {
        lastX = xpos;
        lastY = ypos;
        bFirstMouse = false;
        return;
    }

    camera.ProcessMouseMovement(xpos-lastX, ypos-lastY);
    lastX = xpos;
    lastY = ypos;
}

void scrollCB(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

int main(int argc, char** argv) {
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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// for apple
#endif	// __APPLE__

    GLFWwindow* window = glfwCreateWindow(800, 600, "Lighting Colors", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("glfw create window failed");
        return -2;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("Load proc Failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }
    glfwSetFramebufferSizeCallback(window, glfwResizeCB);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursorPosCB);

    glfwSetScrollCallback(window, scrollCB);

    glEnable(GL_DEPTH_TEST);

    // VAO, VBO;
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader sp(SRC_VERTEX_SHADER, SRC_FRAGMENT_SHADER);
    sp.use();
    sp.setVec3("objColor", 1.0f, 0.5f, 0.31f);
    sp.setVec3("litColor", 1.0f, 1.0f, 1.0f);

    glm::mat4 mmodel(1.f);
    sp.setMat4("model", mmodel);

    Shader sp_light(SRC_VERTEX_SHADER, SRC_FRAGMENT_SHADER_LIGHT);
    sp_light.use();
    mmodel = glm::mat4(1.f);
    mmodel = glm::translate(mmodel, glm::vec3(1.2f, 1.f, 2.f));
    mmodel = glm::scale(mmodel, glm::vec3(0.2f));
    sp_light.setMat4("model", mmodel);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mview = camera.GetViewMatrix();
        glm::mat4 mproj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);

        sp.use();
        sp.setMat4("view", mview);
        sp.setMat4("proj", mproj);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        sp_light.use();
        sp_light.setMat4("view", mview);
        sp_light.setMat4("proj", mproj);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
