#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "common.hpp"
#include "advconf.hpp"

Camera mcam(glm::vec3(0.f, 0.f, 3.f));

#define SRC_VSHADER ADVOGL_SRC_DIR"shader.vs"
#define SRC_FSHADER1 ADVOGL_SRC_DIR"stencil_shader1.fs"
#define SRC_FSHADER2 ADVOGL_SRC_DIR"stencil_shader2.fs"

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(GLFWErrorCB);
    if (!glfwInit()) {
        SPDLOG_CRITICAL("GLFW Init Failed");
        return -1;
    }
    SetCamera(&mcam);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for apple
#endif // __APPLE__
    GLFWwindow* window = glfwCreateWindow(800, 600, "Advanced OpenGL Stencil Testing", NULL, NULL);
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

    // textures
    unsigned int texture0 = LoadTexture(TEXTURE_MARBLE);
    unsigned int texture1 = LoadTexture(TEXTURE_METAL);
    glActiveTexture(GL_TEXTURE0);

    Shader sp1(SRC_VSHADER, SRC_FSHADER1);
    sp1.use();
    sp1.setFloat("texture0", 0);
    Shader sp2(SRC_VSHADER, SRC_FSHADER2);
    sp2.use();
    sp2.setFloat("texture0", 0);

    unsigned int VAO[2], VBO[2];
    glGenVertexArrays(2, VAO);
    glBindVertexArray(VAO[0]);
    glGenBuffers(2, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_STENCIL_TEST);
    float lframe = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 model(1.f);
        glm::mat4 view = mcam.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(mcam.Zoom), 4.f/3.f, 0.1f, 100.f);

        sp1.use();
        sp1.setMat4("model", model);
        sp1.setMat4("view", view);
        sp1.setMat4("proj", proj);
        glBindTexture(GL_TEXTURE_2D, texture0);
        // draw plane
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
