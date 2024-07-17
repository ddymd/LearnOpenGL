#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "spdlog/spdlog.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TEXTURE_IMAGE "brickwall.jpg"

float vertices[] = {
    // positions      // colors         // texture coords
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
};

void glfwErrorCallback(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR::CALLBACK({}): {}", ec, emsg);
}

void glfwFramebufferSizeCallback(GLFWwindow* window, int w, int h) {
    // SPDLOG_INFO("resize to: {}x{}", w, h);
    glViewport(0, 0, w, h);
    glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(int argc, char** argv) {
    // >>> init glfw
    if (!glfwInit()) {
        SPDLOG_CRITICAL("initialize glfw failed");
        return -1;
    }
    // >>> set glfw error callback
    glfwSetErrorCallback(glfwErrorCallback);
    // create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for apple
#endif  // __APPLE__
    GLFWwindow* window = glfwCreateWindow(800, 600, "Textures", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("create glfw window failed");
        glfwTerminate();
        return -2;
    }
    // >>> make context
    glfwMakeContextCurrent(window);
    // init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("load glfw proc failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }
    // >>> set framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);
    // >>> textures
    // load image
    int w, h, c;
    unsigned char* data = stbi_load(TEXTURE_IMAGE, &w, &h, &c, 0);
    // create texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (data) {
        // GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // GLenum target
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        SPDLOG_ERROR("failed to load texture");
    }
    stbi_image_free(data);
    // >>> shader program
    // >>> VAO
    // >>> VBO
    // >>> vertex attributes
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
