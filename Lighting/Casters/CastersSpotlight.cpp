#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "common.hpp"
#include "config.h"

#define SRC_VSHADER TOP_SRC_DIR"Lighting/Casters/spotlight_shader.vs"
#define SRC_FSHADER TOP_SRC_DIR"Lighting/Casters/spotlight_shader.fs"

Camera mcam(glm::vec3(0.f, 0.f, 3.f));
float mshininess = 32.f;
glm::vec3 lambient(0.2f);
glm::vec3 ldiffuse(0.5f);
glm::vec3 lspecular(1.f);
glm::vec3 lposition(1.2f, 1.f, 2.f);

int main(int argc, char const *argv[])
{
    if (!glfwInit()) {
        SPDLOG_CRITICAL("GLFW Init Failed");
        return -1;
    }
    SetCamera(&mcam);
    glfwSetErrorCallback(GLFWErrorCB);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for apple
#endif // __APPLE__
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Casters Spotlight", NULL, NULL);
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

    unsigned int texdiffuse = LoadTexture(TEXTURE_C0);
    unsigned int texspecular = LoadTexture(TEXTURE_CS);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texdiffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texspecular);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norvertices), norvertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    Shader sp(SRC_VSHADER, SRC_FSHADER);
    sp.use();
    sp.setVec3("light.ambient", lambient);
    sp.setVec3("light.diffuse", ldiffuse);
    sp.setVec3("light.specular", lspecular);
    sp.setFloat("light.constant", ldfs[4].kc);
    sp.setFloat("light.linear", ldfs[4].kl);
    sp.setFloat("light.quadratic", ldfs[4].kq);
    sp.setInt("material.diffuse", 0);
    sp.setInt("material.specular", 1);
    sp.setFloat("material.shininess", mshininess);

    float lfm = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cfm = glfwGetTime();
        ProcessInputs(window, cfm - lfm);
        lfm = cfm;
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sp.use();
        sp.setMat4("view", mcam.GetViewMatrix());
        glm::mat4 proj = glm::perspective(glm::radians(mcam.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);
        sp.setMat4("proj", proj);
        sp.setVec3("camPos", mcam.Position);
        sp.setVec3("light.position", mcam.Position);
        sp.setVec3("light.direction", mcam.Front);
        sp.setFloat("light.cutoff", glm::cos(glm::radians(12.5f)));

        glBindVertexArray(VAO);
        for (int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); ++i) {
            glm::mat4 model(1.f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(i * 20.f), glm::vec3(1.0f, 0.3f, 0.5f));
            sp.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
