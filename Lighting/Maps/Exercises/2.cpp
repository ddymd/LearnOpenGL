// Try creating a specular map from the diffuse texture that uses actual colors instead of black
// and white and see that the result doesn’t look too realistic. You can use the following image
// if you can’t generate one yourself: learnopengl.com/img/lighting/lighting_
// maps_specular_color.png. Result: learnopengl.com/img/lighting/lig
// hting_maps_exercise3.png.

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "common.hpp"
#include "config.h"

#define SRC_VSHADER_OBJ TOP_SRC_DIR"Lighting/Maps/Exercises/obj_shader2.vs"
#define SRC_FSHADER_OBJ TOP_SRC_DIR"Lighting/Maps/Exercises/obj_shader2.fs"

#define SRC_VSHADER_LIT TOP_SRC_DIR"Lighting/Maps/Exercises/lit_shader2.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Lighting/Maps/Exercises/lit_shader2.fs"

Camera mcam(glm::vec3(0.f, 0.f, 4.f));

float mshininess = 64.f;
glm::vec3 lambient(0.3f, 0.3f, 0.3f);
glm::vec3 ldiffuse(0.5f, 0.5f, 0.5f);
glm::vec3 lspecular(1.f, 1.f, 1.f);
glm::vec3 lposition(1.2f, 1.f, 2.f);

int main(int argc, char** argv) {
    if (!glfwInit()) {
        SPDLOG_CRITICAL("GLFW Init Faliled");
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Maps Exercises2", NULL, NULL);
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

    // =====> textures
    unsigned int diffusetex = LoadTexture(TEXTURE_C0);
    unsigned int speculartex = LoadTexture(TEXTURE_CSC);

    // =====> vertex
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

    // =====> shaders
    Shader objsp(SRC_VSHADER_OBJ, SRC_FSHADER_OBJ);
    objsp.use();
    objsp.setMat4("model", glm::mat4(1.f));
    objsp.setInt("material.diffuse", 0);
    objsp.setInt("material.specular", 1);
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

    float lframe = 0.f;
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffusetex);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, speculartex);

        glm::mat4 mview = mcam.GetViewMatrix();
        glm::mat mproj = glm::perspective(glm::radians(mcam.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);

        objsp.use();
        objsp.setMat4("view", mview);
        objsp.setMat4("proj", mproj);
        objsp.setVec3("camPos", mcam.Position);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        litsp.use();
        litsp.setMat4("view", mview);
        litsp.setMat4("proj", mproj);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
