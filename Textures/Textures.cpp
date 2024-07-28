#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "spdlog/spdlog.h"
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "config.h"

#define TEXTURE_IMAGE1 RESOURCES_DIR"container.jpg"
#define TEXTURE_IMAGE2 RESOURCES_DIR"awesomeface.png"
#define VERTEX_SHADER_SRC TOP_SRC_DIR"Textures/texture.vs"
#define FRAGMENT_SHADER_SRC TOP_SRC_DIR"Textures/texture.fs"

float mvertices[] = {
    // positions      // colors         // texture coords
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
};

unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

unsigned int VAO, texture[2];

void draw(unsigned int VAO, unsigned int *texture) {
    glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBindVertexArray(VAO);
    // GLenum mode, GLsizei count, GLenum type, const void *indices
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void glfwErrorCallback(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR::CALLBACK({}): {}", ec, emsg);
}

void glfwFramebufferSizeCallback(GLFWwindow* window, int w, int h) {
    // SPDLOG_INFO("resize to: {}x{}", w, h);
    glViewport(0, 0, w, h);
    draw(VAO, texture);
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
    stbi_set_flip_vertically_on_load(true);
    int w, h, c;
    unsigned char* data = stbi_load(TEXTURE_IMAGE1, &w, &h, &c, 0);
    // create texture
    glGenTextures(2, texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (data) {
        // GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // GLenum target
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        SPDLOG_ERROR("failed to load texture1");
    }

    data = stbi_load(TEXTURE_IMAGE2, &w, &h, &c, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        // GLenum target
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        SPDLOG_ERROR("failed to load texture2");
    }

    // >>> shader program
    Shader shaderProgram(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC);
    shaderProgram.use();
    shaderProgram.setInt("texture1", 0);
    shaderProgram.setInt("texture2", 1);
    // >>> VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // >>> VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mvertices), mvertices, GL_STATIC_DRAW);
    // >>> EBO
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // >>> vertex attributes
    // GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        draw(VAO, texture);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteTextures(2, texture);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
