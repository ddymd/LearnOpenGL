// Try drawing a second container with another call to glDrawElements but place it at a
// different position using transformations only. Make sure this second container is placed at the
// top-left of the window and instead of rotating, scale it over time (using the sin function is
// useful here; note that using sin will cause the object to invert as soon as a negative scale is applied).
// Solution: /src/1.getting_started/5.2.transformations_exercis
// e2/.

#include "glad/glad.h"
#include "glfw/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "config.h"
#include "shader.h"

#include "spdlog/spdlog.h"

#define TEXTURE_IMAGE1 RESOURCES_DIR"container.jpg"
#define TEXTURE_IMAGE2 RESOURCES_DIR"awesomeface.png"

#define SRC_VSHADER TOP_SRC_DIR"Transformations/Exercises/texture2.vs"
#define SRC_FSHADER TOP_SRC_DIR"Transformations/Exercises/texture2.fs"

float vertices[] = {
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

void glfwErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR_CB({}): {}", ec, emsg);
}

void glfwResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char** argv) {
    // init glfw
    if (!glfwInit()) {
        SPDLOG_CRITICAL("glfw init failed");
        return -1;
    }

    // set error cb
    glfwSetErrorCallback(glfwErrorCB);

    // create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for apple

    GLFWwindow* window = glfwCreateWindow(800, 600, "Transformations Exercises2", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("glfw create window failed");
        return -2;
    }

    // make context
    glfwMakeContextCurrent(window);

    // init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("glad load proc failed");
        return -3;
    }

    // set resize callback
    glfwSetFramebufferSizeCallback(window, glfwResizeCB);


    // textures
    unsigned int textures[2];
    glGenTextures(2, textures);
    stbi_set_flip_vertically_on_load(true);
    int w, h, c;
    unsigned char* data = stbi_load(TEXTURE_IMAGE1, &w, &h, &c, 0);
    if (data) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        SPDLOG_ERROR("load texture1({}) failed", TEXTURE_IMAGE1);
    }

    data = stbi_load(TEXTURE_IMAGE2, &w, &h, &c, 0);
    if (data) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        SPDLOG_ERROR("load texture2({}) failed", TEXTURE_IMAGE2);
    }

    // VAO, VBO, EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    // shader program
    Shader sp(SRC_VSHADER, SRC_FSHADER);
    sp.use();
    sp.setInt("texture1", 0);
    sp.setInt("texture2", 1);
    auto translid = glGetUniformLocation(sp.id, "trans");


    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);

        glm::mat4 trans0(1.0);
        trans0 = glm::translate(trans0, {0.5f,-0.5f,0.f});
        trans0 = glm::rotate(trans0, (float)glfwGetTime(), {0.f,0.f,1.f});
        glUniformMatrix4fv(translid, 1, GL_FALSE, glm::value_ptr(trans0));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glm::mat4 trans1(1.0);
        trans1 = glm::translate(trans1, {-0.5f,0.5f,0.f});
        float scaleAmount = static_cast<float>(sin(glfwGetTime()));
        trans1 = glm::scale(trans1, {scaleAmount, scaleAmount, scaleAmount});
        glUniformMatrix4fv(translid, 1, GL_FALSE, glm::value_ptr(trans1));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
