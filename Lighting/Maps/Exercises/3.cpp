// Also add something they call an emission map which is a texture that stores emission values
// per fragment. Emission values are colors an object may emit as if it contains a light source
// itself; this way an object can glow regardless of the light conditions. Emission maps are
// often what you see when objects in a game glow (like the eyes of a robot, or light strips
// on a container). Add the following texture (by creativesam) as an emission map onto the
// container as if the letters emit light: learnopengl.com/img/textures/matrix.
// jpg. Solution: /src/2.lighting/4.4.lighting_maps_exercise4/; result:
// learnopengl.com/img/lighting/lighting_maps_exercise4.png.
#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "common.hpp"
#include "config.h"

#define SRC_VSHADER_OBJ TOP_SRC_DIR"Lighting/Maps/Exercises/obj_shader3.vs"
#define SRC_FSHADER_OBJ TOP_SRC_DIR"Lighting/Maps/Exercises/obj_shader3.fs"

#define SRC_VSHADER_LIT TOP_SRC_DIR"Lighting/Maps/Exercises/lit_shader3.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Lighting/Maps/Exercises/lit_shader3.fs"

Camera mcam(glm::vec3(0.f, 0.f, 3.f));

float mshininess = 64.f;
glm::vec3 lambient(0.2f, 0.2f, 0.2f);
glm::vec3 ldiffuse(0.5f, 0.5f, 0.5f);
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Maps Exercises3", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("GLFW Create Window Failed");
        return -2;
    }
    SetCamera(&mcam);
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

    unsigned int VAO[2], VBO;
    glGenVertexArrays(2, VAO);
    glBindVertexArray(VAO[0]);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norvertices), norvertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // textures
    unsigned int diffusetex = LoadTexture(TEXTURE_C0);
    unsigned int speculartex = LoadTexture(TEXTURE_CSC);
    unsigned int emissiontex = LoadTexture(TEXTURE_MATRIX);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffusetex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, speculartex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissiontex);

    Shader osp(SRC_VSHADER_OBJ, SRC_FSHADER_OBJ);
    osp.use();
    osp.setMat4("model", glm::mat4(1.f));
    osp.setInt("material.diffuse", 0);
    osp.setInt("material.specular", 1);
    osp.setInt("material.emission", 2);
    osp.setFloat("material.shininess", mshininess);

    osp.setVec3("light.ambient", lambient);
    osp.setVec3("light.diffuse", ldiffuse);
    osp.setVec3("light.specular", lspecular);
    osp.setVec3("light.position", lposition);
    Shader lsp(SRC_VSHADER_LIT, SRC_FSHADER_LIT);
    lsp.use();
    glm::mat4 lm(1.f);
    lm = glm::translate(lm, lposition);
    lm = glm::scale(lm, glm::vec3(0.2f));
    lsp.setMat4("model", lm);

    float lfm = 0.f;
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float cfm = glfwGetTime();
        ProcessInputs(window, cfm-lfm);
        lfm = cfm;

        glm::mat4 mview = mcam.GetViewMatrix();
        glm::mat4 mproj = glm::perspective(glm::radians(mcam.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);

        osp.use();
        osp.setVec3("camPos", mcam.Position);
        osp.setMat4("view", mview);
        osp.setMat4("proj", mproj);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lsp.use();
        lsp.setMat4("view", mview);
        lsp.setMat4("proj", mproj);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
