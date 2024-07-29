// Try to create your own LookAt function where you manually create a view matrix as discussed
// at the start of this chapter. Replace glm’s LookAt function with your own implementation
// and see if it still acts the same. Solution: /src/1.getting_started/7.6.camera_
// exercise2/.

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "config.h"
#include "shader.h"

#define SRC_VERTEX_SHADER TOP_SRC_DIR"Camera/Exercises/shader2.vs"
#define SRC_FRAGMENT_SHADER TOP_SRC_DIR"Camera/Exercises/shader2.fs"

void glfwErrorCB(int ec, const char* emsg);
void glfwResizeCB(GLFWwindow* window, int w, int h);
glm::vec3 camFront(0.f, 0.f, -1.f);
void processInput(GLFWwindow* window, float camSpeed, glm::vec3& camPos, const glm::vec3& camFront);
void cursorPosCB(GLFWwindow* window, double xpos, double ypos/*, glm::vec3& camFront*/);
float fov = 45.f;
void cursorScollCB(GLFWwindow* window, double xoffset, double yoffset);
glm::mat4 mlookAt(const glm::vec3& camPos, const glm::vec3& camTgt, const glm::vec3& worldUp = glm::vec3(0.f, 1.f, 0.f));

int main(int argc, char** argv) {
    if (!glfwInit()) {
        SPDLOG_CRITICAL("GLFW Init Failed");
        return -1;
    }
    // set error callback
    glfwSetErrorCallback(glfwErrorCB);

    // create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Camera Exercieses 2", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("GLFW Create Window Failed");
        return -2;
    }
    // make context current
    glfwMakeContextCurrent(window);

    // init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("GLAD Load Proc Failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }
    // set resize callback
    glfwSetFramebufferSizeCallback(window, glfwResizeCB);
    // enable depth test
    glEnable(GL_DEPTH_TEST);

    // set cursor postion callback
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursorPosCB);

    // set scroll callback
    glfwSetScrollCallback(window, cursorScollCB);

    SPDLOG_INFO("glfw and glad");

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
        SPDLOG_ERROR("Load Texture0({}) Failed", TEXTURE0);
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
        SPDLOG_ERROR("Load Texture1({}) Failed", TEXTURE1);
    }

    // VAO, VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // vertex properties
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // shader program
    Shader sp(SRC_VERTEX_SHADER, SRC_FRAGMENT_SHADER);
    sp.use();
    sp.setInt("texture1", 0);
    sp.setInt("texture2", 1);
    // model
    glm::mat4 mmodel(1.f);
    // view
    glm::mat4 mview{1.f};
    // proj
    glm::mat4 mproj{1.f};
    glm::vec3 camPos(0.f, 0.f, 5.f);
    float lastFrame = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        processInput(window, 5.f*(currentFrame-lastFrame), camPos, camFront);
        lastFrame = currentFrame;
        glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, textures[0]);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, textures[1]);

        glBindVertexArray(VAO);
        // target = camPos + camFront
        mview = mlookAt(camPos, camPos+camFront, {0.f, 1.f, 0.f});
        sp.setMat4("view", mview);

        mproj = glm::perspective(glm::radians(fov), 4.f/3.f, 0.1f, 100.f);
        sp.setMat4("proj", mproj);

        for (int i = 0; i < sizeof(cubePositions)/sizeof(cubePositions[0]); ++i) {
            mmodel = glm::mat4(1.f);
            mmodel = glm::translate(mmodel, cubePositions[i]);
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

void glfwErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR_CB({}): {}", ec, emsg);
}

void glfwResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

glm::vec3 worldUp(0.f, 1.f, 0.f);
void processInput(GLFWwindow* window, float camSpeed, glm::vec3& camPos, const glm::vec3& camFront) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    // forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camPos += camSpeed * camFront;
    }
    // backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camPos -= camSpeed * camFront;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camPos -= camSpeed * glm::normalize(glm::cross(camFront, worldUp));
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camPos += camSpeed * glm::normalize(glm::cross(camFront, worldUp));
    }
}

float sensitivity = 0.01f;
bool bISFirstCursor = true;
float xlastpos, ylastpos;
float yaw = -90.f, pitch = 0.f;
void cursorPosCB(GLFWwindow* window, double xpos, double ypos/*, glm::vec3& camFront*/) {
    if (bISFirstCursor) {
        xlastpos = xpos;
        ylastpos = ypos;
        bISFirstCursor = false;
        return;
    }
    float xoffset = sensitivity * (xpos - xlastpos);
    xlastpos = xpos;
    float yoffset = sensitivity * (ypos - ylastpos);
    ylastpos = ypos;

    yaw += xoffset;
    pitch += yoffset;
    if (pitch > 89.f) pitch = 89.f;
    if (pitch < -89.f) pitch = -89.f;

    float x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    float y = sin(glm::radians(pitch));
    float z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(glm::vec3(x,y,z));
}

void cursorScollCB(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= yoffset;
    if (fov < 1.f) fov = 1.f;
    if (fov > 90.f) fov = 90.f;
}

glm::mat4 mlookAt(const glm::vec3& camPos, const glm::vec3& camTgt, const glm::vec3& worldUp) {
    glm::vec3 camDir = glm::normalize(camPos - camTgt);     // zaxis
    glm::vec3 camRight = glm::normalize(glm::cross(glm::normalize(worldUp), camDir));       // xaxis
    glm::vec3 camUp = glm::cross(camDir, camRight);     // yaxis

    glm::mat4 translation = glm::mat4(1.0f);
    translation[3][0] = -camPos.x;
    translation[3][1] = -camPos.y;
    translation[3][2] = -camPos.z;

    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0][0] = camRight.x;
    rotation[1][0] = camRight.y;
    rotation[2][0] = camRight.z;

    rotation[0][1] = camUp.x;
    rotation[1][1] = camUp.y;
    rotation[2][1] = camUp.z;

    rotation[0][2] = camDir.x;
    rotation[1][2] = camDir.y;
    rotation[2][2] = camDir.z;

    return rotation*translation;
}

