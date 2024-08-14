#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "shader.hpp"
#include "common.hpp"
#include "config.h"

#define SRC_VSHADER TOP_SRC_DIR"Lighting/Casters/directional_shader.vs"
#define SRC_FSHADER TOP_SRC_DIR"Lighting/Casters/directional_shader.fs"
#define SRC_VSHADER_L TOP_SRC_DIR"Lighting/Casters/litshader.vs"
#define SRC_FSHADER_L TOP_SRC_DIR"Lighting/Casters/litshader.fs"

Camera mcam(glm::vec3(0.f, 0.f, 3.f));
float mshininess = 32.f;
glm::vec3 lambient(0.2f, 0.2f, 0.2f);
glm::vec3 ldiffuse(0.5f, 0.5f, 0.5f);
glm::vec3 lspecular(1.f, 1.f, 1.f);
glm::vec3 ldirection(-0.2f, -1.f, -0.3f);
glm::vec3 lposition(1.2f, 1.f, 2.f);

int main(int argc, char** argv) {
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Casters Directional", NULL, NULL);
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

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norvertices), norvertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int LVAO;
    glGenVertexArrays(1, &LVAO);
    glBindVertexArray(LVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, LoadTexture(TEXTURE_C0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, LoadTexture(TEXTURE_CS));

    Shader sp(SRC_VSHADER, SRC_FSHADER);
    sp.use();
    sp.setInt("material.diffuse", 0);
    sp.setInt("material.specular", 1);
    sp.setFloat("material.shininess", mshininess);
    sp.setVec3("light.ambient", lambient);
    sp.setVec3("light.diffuse", ldiffuse);
    sp.setVec3("light.specular", lspecular);
    sp.setVec3("light.direction", ldirection);
    // point light - attenuation
    sp.setVec3("light.position", lposition);
    sp.setFloat("light.constant", ldfs[4].kc);
    sp.setFloat("light.linear", ldfs[4].kl);
    sp.setFloat("light.quadratic", ldfs[4].kq);

    Shader spl(SRC_VSHADER_L, SRC_FSHADER_L);
    spl.use();
    glm::mat4 lmodel(1.f);
    lmodel = glm::translate(lmodel, lposition);
    lmodel = glm::scale(lmodel, glm::vec3(0.3f));
    spl.setMat4("model", lmodel);

    float lfm = 0.f;
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float cfm = glfwGetTime();
        ProcessInputs(window, cfm-lfm);
        lfm = cfm;
        glm::mat4 mview = mcam.GetViewMatrix();
        glm::mat4 mproj = glm::perspective(glm::radians(mcam.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);
        sp.use();
        sp.setMat4("view", mview);
        sp.setMat4("proj", mproj);
        sp.setVec3("camPos", mcam.Position);

        glBindVertexArray(VAO);
        for (int i = 0; i < sizeof(cubePositions)/sizeof(cubePositions[0]); ++i) {
            glm::mat4 cmodel(1.f);
            cmodel = glm::translate(cmodel, cubePositions[i]);
            float angle = 20.0f * i;
            cmodel = glm::rotate(cmodel, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            sp.setMat4("model", cmodel);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        spl.use();
        spl.setMat4("view", mview);
        spl.setMat4("proj", mproj);
        glGenVertexArrays(1, &LVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
