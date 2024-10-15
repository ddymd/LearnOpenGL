#include "spdlog/spdlog.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "common.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "advglslconf.hpp"

#define DEMO_FRONT_FACING 1

#if DEMO_POINT_SIZE
#define VSHADER_SRC ADVGLSL_SRC_DIR"points.vs"
#define FSHADER_SRC ADVGLSL_SRC_DIR"points.fs"
#elif DEMO_FRAG_COORD
#define VSHADER_SRC ADVGLSL_SRC_DIR"fragcoord.vs"
#define FSHADER_SRC ADVGLSL_SRC_DIR"fragcoord.fs"
#elif DEMO_FRONT_FACING
#define VSHADER_SRC ADVGLSL_SRC_DIR"frontfacing.vs"
#define FSHADER_SRC ADVGLSL_SRC_DIR"frontfacing.fs"
#endif

Camera mcam(glm::vec3(0.f, 0.f, 3.f));

int main() {
    if (!glfwInit()) {
        SPDLOG_CRITICAL("GLFW Init Faield");
        return -1;
    }
    glfwSetErrorCallback(GLFWErrorCB);
    SetCamera(&mcam);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GL_TRUE);
#endif // __APPLE__

    GLFWwindow* window = glfwCreateWindow(800, 600, "AdvGLSL", NULL, NULL);
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
    glfwSetCursorPosCallback(window, GLFWCursorPosCB);
    glfwSetScrollCallback(window, GLFWScrollCB);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader sp(VSHADER_SRC, FSHADER_SRC);
    sp.use();
    sp.setMat4("model", glm::mat4(1.f));

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
#if DEMO_POINT_SIZE
    glEnable(GL_PROGRAM_POINT_SIZE);
#endif // DEMO_POINT_SIZE

#if DEMO_FRONT_FACING
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glActiveTexture(GL_TEXTURE0);
    unsigned int texback = LoadTexture(textures[0]);
    glBindTexture(GL_TEXTURE_2D, texback);
    sp.setInt("texback", 0);

    glActiveTexture(GL_TEXTURE1);
    unsigned int texfront = LoadTexture(textures[1]);
    glBindTexture(GL_TEXTURE_2D, texfront);
    sp.setInt("texfront", 1);
#endif // DEMO_FRONT_FACING
    glEnable(GL_DEPTH_TEST);
    float lframe = 0;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.2f, 0.f);
        glm::mat4 view = mcam.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(mcam.Zoom), 4.f/3.f, 0.1f, 100.f);
        sp.setMat4("view", view);
        sp.setMat4("proj", proj);

        glBindVertexArray(VAO);
#if DEMO_POINT_SIZE
        glDrawArrays(GL_POINTS, 0, 3);
#elif DEMO_FRAG_COORD || DEMO_FRONT_FACING
        glDrawArrays(GL_TRIANGLES, 0, 36);
#endif

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}
