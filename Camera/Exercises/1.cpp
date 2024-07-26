// See if you can transform the camera class in such a way that it becomes a true fps camera
// where you cannot fly; you can only look around while staying on the xz plane. Solution:
// /src/1.getting_started/7.5.camera_exercise1/.

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "config.h"
#include "shader.h"

#define TEXTURE0 RESOURCES_DIR"container.jpg"
#define TEXTURE1 RESOURCES_DIR"awesomeface.png"

#define SRC_VERTEX_SHADER TOP_SRC_DIR"Camera/Exercises/shader1.vs"
#define SRC_FRAGMENT_SHADER TOP_SRC_DIR"Camera/Exercises/shader1.fs"

float vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f, 0.0f, 0.0f),
    glm::vec3( 2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f, 2.0f, -2.5f),
    glm::vec3( 1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f)
};

void glfwErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR_CB({}): {}", ec, emsg);
}

void glfwResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

glm::vec3 camFront(0.f,0.f,-1.f);
glm::vec3 camPos(0.f, 0.f, 5.f);
glm::vec3 camUp(0.f, 1.f, 0.f);
// float camSpeed = 0.005f;
void processInput(GLFWwindow* window, float camSpeed) {
    // escape key
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // forward
        camPos += camSpeed * camFront;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // backward
        camPos -= camSpeed * camFront;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // left
        camPos -= camSpeed * glm::normalize(glm::cross(camFront, camUp));
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // right
        camPos += camSpeed * glm::normalize(glm::cross(camFront, camUp));
    }
    camPos.y = 0;
}
bool bIsFirstMouse = true;
float lastX, lastY;
float sensitivity = 0.1f;
float pitch = 0.f;
float yaw = -90.f;

void mouseMoveCB(GLFWwindow* window, double xpos, double ypos) {
    if (bIsFirstMouse) {
        lastX = xpos;
        lastY = ypos;
        bIsFirstMouse = false;
        return;
    }
    float xoffset = (xpos - lastX) * sensitivity;
    float yoffset = (ypos - lastY) * sensitivity;
    lastX = xpos;
    lastY = ypos;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.f) pitch = 89.f;
    if (pitch < -89.f) pitch = -89.f;

    float fyaw = glm::radians(yaw);
    float fpitch = glm::radians(pitch);
    camFront = glm::normalize(glm::vec3{cos(fyaw)*cos(fpitch), sin(fpitch), sin(fyaw)*cos(fpitch)});
}

float zoom= 45.f;
void mouseScollCB(GLFWwindow* window, double xoffset, double yoffset) {
    zoom += yoffset;
}

int main(int argc, char** argv) {
    // init glfw
    if (!glfwInit()) {
        SPDLOG_CRITICAL("glfw init failed");
        return -1;
    }

    // set error callback
    glfwSetErrorCallback(glfwErrorCB);

    // create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for apple
#endif // __APPLE__

    GLFWwindow* window = glfwCreateWindow(800, 600, "Camera Exercises-1", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("glfw create window failed");
        return -2;
    }

    // make context
    glfwMakeContextCurrent(window);

    // init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("glad load proc failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }

    // set resize cb
    glfwSetFramebufferSizeCallback(window, glfwResizeCB);

    // set input mode
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // set mouse move callback
    glfwSetCursorPosCallback(window, mouseMoveCB);

    // set scroll cb
    glfwSetScrollCallback(window, mouseScollCB);

    // enable depth test
    glEnable(GL_DEPTH_TEST);

    // textures
    unsigned int textures[2];
    glGenTextures(2, textures);

    stbi_set_flip_vertically_on_load(true);
    int w, h, c;
    unsigned char* data = stbi_load(TEXTURE0, &w, &h, &c, 0);
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
        SPDLOG_ERROR("Load Texture({}) Failed", TEXTURE0);
    }

    data = stbi_load(TEXTURE1, &w, &h, &c, 0);
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
        SPDLOG_ERROR("Load Texture({}) Failed", TEXTURE1);
    }

    // VAO, VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // shader program
    Shader sp(SRC_VERTEX_SHADER, SRC_FRAGMENT_SHADER);
    sp.use();
    sp.setInt("texture1", 0);
    sp.setInt("texture2", 1);

    // glm::mat4 mview(1.f);
    // mview = glm::translate(mview, {0.f, 0.f, -5.f});
    glm::mat4 mview = glm::lookAt(camPos, {0.f,0.f,0.f}, {0.f,1.f,0.f});
    sp.setMat4("view", mview);

    glm::mat4 mproj(1.f);
    mproj = glm::perspective(glm::radians(45.f), 4.f/3.f, 0.1f, 100.f);
    sp.setMat4("proj", mproj);

    static float lastFrame = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float curFrame = glfwGetTime();
        processInput(window, 5.5f * (curFrame-lastFrame));
        lastFrame = curFrame;

        // textures
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, textures[0]);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, textures[1]);

        glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec3 lookat = camPos+camFront;
        lookat.y = 0;
        mview = glm::lookAt(camPos, lookat, {0.f,1.f,0.f});
        sp.setMat4("view", mview);

        mproj = glm::perspective(glm::radians(zoom), 4.f/3.f, 0.1f, 100.f);
        sp.setMat4("proj", mproj);

        glBindVertexArray(VAO);
        for (int i = 0; i < sizeof(cubePositions)/sizeof(cubePositions[0]); ++i) {
            glm::mat4 mmodel(1.f);
            mmodel = glm::translate(mmodel, cubePositions[i]);
            mmodel = glm::rotate(mmodel, glm::radians(20.f*i), {1.0f, 0.3f, 0.5f});
            sp.setMat4("model", mmodel);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
