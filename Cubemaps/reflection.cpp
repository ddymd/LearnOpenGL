#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "common.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "cubemapsconf.hpp"

#define SRC_VSHADER CUBEMAPS_SRC_DIR"refshader.vs"
#define SRC_FSHADER CUBEMAPS_SRC_DIR"refshader.fs"
#define SRC_VSHADER_SKY CUBEMAPS_SRC_DIR"skyshader1.vs"
#define SRC_FSHADER_SKY CUBEMAPS_SRC_DIR"skyshader1.fs"

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
    GLFWwindow* window = glfwCreateWindow(800, 600, "CubeMaps Reflection", NULL, NULL);
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

    unsigned int VAO[2], VBO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    // cube
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norvertices), norvertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    // skybox
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // textures
    unsigned int texcontainer = LoadTexture(TEXTURE_CONTAINER);
    unsigned int texsky = LoadCubeTexture(texskybox);
    glActiveTexture(GL_TEXTURE0);

    // shader program
    Shader refsp(SRC_VSHADER, SRC_FSHADER);
    refsp.use();
    refsp.setMat4("model", glm::mat4(1.f));
    refsp.setInt("skybox", 0);
    Shader skysp(SRC_VSHADER_SKY, SRC_FSHADER_SKY);
    skysp.use();
    skysp.setInt("texcube", 0);

    glEnable(GL_DEPTH_TEST);
    float lframe = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);

        glm::mat4 view = mcam.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(mcam.Zoom), 4.f/3.f, 0.1f, 100.f);
        refsp.use();
        refsp.setMat4("view", view);
        refsp.setMat4("proj", proj);
        refsp.setVec3("cameraPos", mcam.Position);
        // glBindTexture(GL_TEXTURE_2D, texcontainer);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texsky);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //////////////////////////////////////// skybox /////////////////////////////////////////////
        glDepthFunc(GL_LEQUAL);
        skysp.use();
        skysp.setMat4("view", glm::mat4(glm::mat3(view)));
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
