#include <map>

#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "common.hpp"
#include "advconf.hpp"

#define SRC_VSHADER ADVOGL_SRC_DIR"bdshader.vs"
#define SRC_FSHADER ADVOGL_SRC_DIR"bdshader.fs"

Camera mcam(glm::vec3(0.f, 0.f, 3.f));

int main(int argc, char* argv[]) {
    // spdlog::cfg::load_env_levels();
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "Blending Discard", NULL, NULL);
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
    unsigned int texmarble = LoadTexture(TEXTURE_MARBLE);
    unsigned int texmetal = LoadTexture(TEXTURE_METAL);
    unsigned int texgrass = LoadTexture(TEXTURE_GRASS);

    unsigned int texwindow = LoadTexture(TEXTURE_WINDOW);
    glActiveTexture(GL_TEXTURE0);

    // shaders
    Shader sp(SRC_VSHADER, SRC_FSHADER);
    sp.use();

    // VAO  0: cube
    //      1: plane
    //      2: grass
    unsigned int VAO[3], VBO[3];
    glGenVertexArrays(3, VAO);
    glBindVertexArray(VAO[0]);
    glGenBuffers(3, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float lframe = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe - lframe);
        lframe = cframe;
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        sp.use();

        glm::mat4 model(1.f);
        glm::mat4 view = mcam.GetViewMatrix();
        glm::mat4 proj(1.f);
        proj = glm::perspective(glm::radians(mcam.Zoom), 4.f / 3.f, 0.1f, 100.f);

        glBindTexture(GL_TEXTURE_2D, texmetal);
        sp.setMat4("view", view);
        sp.setMat4("proj", proj);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, texmarble);
        glBindVertexArray(VAO[0]);
        model = glm::translate(model, cubepos1);
        sp.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.f);
        model = glm::translate(model, cubepos2);
        sp.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindTexture(GL_TEXTURE_2D, texgrass);
        glBindTexture(GL_TEXTURE_2D, texwindow);
        glBindVertexArray(VAO[2]);
        auto wsize = sizeof(vegetationPositions) / sizeof(vegetationPositions[0]);
        std::map<float, glm::vec3> sorted;
        for (int i = 0; i < wsize; ++i) {
            float distance = glm::length(mcam.Position-vegetationPositions[i]);
            sorted[distance] = vegetationPositions[i];

            // model = glm::mat4(1.f);
            // model = glm::translate(model, vegetationPositions[i]);
            // sp.setMat4("model", model);
            // glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
            model = glm::mat4(1.f);
            model = glm::translate(model, it->second);
            sp.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
