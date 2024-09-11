#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "shader.hpp"
#include "cubemapsconf.hpp"
#include "common.hpp"

#define SRC_VSHADER_SKY CUBEMAPS_SRC_DIR"skyshader.vs"
#define SRC_FSHADER_SKY CUBEMAPS_SRC_DIR"skyshader.fs"

#define SRC_VSHADER CUBEMAPS_SRC_DIR"shader.vs"
#define SRC_FSHADER CUBEMAPS_SRC_DIR"shader.fs"

Camera mcam(glm::vec3(0.f, 0.f, 3.f));

int main(int argc, char** argv) {
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "CubeMaps", NULL, NULL);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, GLFWFrameResizeCB);
    glfwSetCursorPosCallback(window, GLFWCursorPosCB);
    glfwSetScrollCallback(window, GLFWScrollCB);

    unsigned int VAO[2], VBO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    // cube
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // skybox
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // skybox texture
    unsigned int texsky = LoadCubeTexture(texskybox);
    // cube texture
    unsigned int texcube = LoadTexture(TEXTURE_CONTAINER);
    glActiveTexture(GL_TEXTURE0);

    // shader
    Shader sp(SRC_VSHADER, SRC_FSHADER);
    sp.use();
    sp.setInt("texcontainer", 0);

    Shader skysp(SRC_VSHADER_SKY, SRC_FSHADER_SKY);
    skysp.use();
    skysp.setInt("texcube", 0);

    glEnable(GL_DEPTH_TEST);
    float lframe = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = mcam.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(mcam.Zoom), 4.f/3.f, 0.1f, 100.f);

        sp.use();
        sp.setMat4("model", glm::mat4(1.f));
        sp.setMat4("view", view);
        sp.setMat4("proj", proj);
        glBindTexture(GL_TEXTURE_2D, texcube);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw sky
        glDepthFunc(GL_LEQUAL);

        skysp.use();
        skysp.setMat4("view", glm::mat4(glm::mat3(mcam.GetViewMatrix())));
        skysp.setMat4("proj", proj);

        glBindTexture(GL_TEXTURE_CUBE_MAP, texsky);

        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
