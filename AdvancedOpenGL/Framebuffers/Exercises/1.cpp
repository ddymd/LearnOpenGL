//  Can you use framebuffers to create a rear-view mirror? For this you’ll have to draw your
//  scene twice: one with the camera rotated 180 degrees and the other as normal. Try to
//  create a small quad at the top of your screen to apply the mirror texture on. Solution:
//  /src/4.advanced_opengl/5.2.framebuffers_exercise1/.

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "common.hpp"
#include "advconf.hpp"

#define SRC_VSHADER ADVOGL_SRC_DIR"Exercises/shader.vs"
#define SRC_FSHADER ADVOGL_SRC_DIR"Exercises/shader.fs"
#define SRC_VSHADER_SCREEN ADVOGL_SRC_DIR"Exercises/screenshader.vs"
#define SRC_FSHADER_SCREEN ADVOGL_SRC_DIR"Exercises/screenshader.fs"

Camera mcam(glm::vec3(0.f, 0.f, 3.f));
Shader sp(SRC_VSHADER, SRC_FSHADER);
Shader screensp(SRC_VSHADER_SCREEN, SRC_FSHADER_SCREEN);

float quadVertices[] = {
    // positions // texCoords
    -1.0f, 1.0f, 0.0f, 1.0f,
    -1.0f,-1.0f, 0.0f, 0.0f,
     1.0f,-1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f,
     1.0f,-1.0f, 1.0f, 0.0f,
     1.0f, 1.0f, 1.0f, 1.0f
};

float quadVertices1[] = {
    // positions // texCoords
    -1.f,  1.f, 0.f, 1.f,
    -1.0f, 0.f, 0.f, 0.f,
     0.f,  0.f, 1.f, 0.f,
    -1.f,  1.f, 0.f, 1.f,
     0.f,  0.f, 1.f, 0.f,
     0.f,  1.f, 1.f, 1.f
};

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
    GLFWwindow* window = glfwCreateWindow(800, 600, "Framebuffers Exercises1", NULL, NULL);
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
    glBindVertexArray(VAO[0]);
    glGenBuffers(2, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    // cubes
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);

    float lframe = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;

        glm::mat4 model(1.f);
        glm::mat4 view = mcam.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(mcam.Zoom), 4.f/3.f, 0.1f, 100.f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
