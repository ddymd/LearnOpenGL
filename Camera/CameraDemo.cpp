#include "spdlog/spdlog.h"

#include "glad/glad.h"
#include "glfw/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"
#include "camera.h"
#include "config.h"

#define TEXTURE_IMAGE1 RESOURCES_DIR"container.jpg"
#define TEXTURE_IMAGE2 RESOURCES_DIR"awesomeface.png"

#define SRC_VERTEX_SHADER TOP_SRC_DIR"Camera/texture.vs"
#define SRC_FRAGMENT_SHADER TOP_SRC_DIR"Camera/texture.fs"

void glfwErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR_CB({}): {}", ec, emsg);
}

void glfwResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

// float cameraSpeed = 0.05f;
// glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
// glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.f; // Time between current frame and last frame
float lastFrame = 0.f; // Time of last frame

Camera insCamera(glm::vec3(0.0f, 0.0f, 5.0f));

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // cameraPos += cameraSpeed * cameraFront;
        insCamera.ProcessKeyboard(ECameraMovement::FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // cameraPos -= cameraSpeed * cameraFront;
        insCamera.ProcessKeyboard(ECameraMovement::BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
        insCamera.ProcessKeyboard(ECameraMovement::LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
        insCamera.ProcessKeyboard(ECameraMovement::RIGHT, deltaTime);
    }
}
float lastX = 400;
float lastY = 300;
float yaw, pitch;
bool firstMouse = true;
void glfwCursorPosCB(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    // const float sensitivity = 0.1f;
    // xoffset *= sensitivity;
    // yoffset *= sensitivity;

    // yaw += xoffset;
    // if (pitch > 89.f) pitch = 89.f;
    // if (pitch < -89.f) pitch = -89.f;
    // pitch += yoffset;

    // glm::vec3 direction;
    // direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    // direction.y = sin(glm::radians(pitch));
    // direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    // cameraFront = glm::normalize(direction);

    insCamera.ProcessMouseMovement(xoffset, yoffset);
}

float zoom = 45.f;
void scrollCB(GLFWwindow* window, double xoffset, double yoffset) {
    // zoom -= (float)yoffset;
    // if (zoom < 1.f) zoom = 1.f;
    // if (zoom > 45.f) zoom = 45.f;
    insCamera.ProcessMouseScroll(yoffset);
}

int main(int argc, char** argv) {
    // init glfw
    if (!glfwInit()) {
        SPDLOG_CRITICAL("GLFW INIT failed");
        return -1;
    }

    // set error callback
    glfwSetErrorCallback(glfwErrorCB);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for apple
#endif // __APPLE__

    GLFWwindow* window = glfwCreateWindow(800, 600, "Camera", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("GLFW Create Window Failed");
        return -2;
    }

    // make context
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("GLAD Load Proc Failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }
    glfwSetFramebufferSizeCallback(window, glfwResizeCB);

    // texutres
    unsigned int textures[2];
    glGenTextures(2, textures);
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
        SPDLOG_ERROR("Load texture1({}) failed", TEXTURE_IMAGE1);
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
        SPDLOG_ERROR("Load texture2({}) failed", TEXTURE_IMAGE2);
    }

    // VAO, VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // attrib - vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // attrib - coord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // shader program
    Shader sp(SRC_VERTEX_SHADER, SRC_FRAGMENT_SHADER);
    sp.use();
    sp.setInt("texture1", 0);
    sp.setInt("texture2", 1);

    glm::mat4 mmodel(1.f);
    mmodel = glm::rotate(mmodel, glm::radians(30.f), {0.8f, 0.5f, 0.f});
    sp.setMat4("model", mmodel);

    glm::mat4 mview(1.f);
    mview = glm::translate(mview, {0.f, 0.f, -6.f});
    sp.setMat4("view", mview);

    glm::mat4 mproj = glm::perspective(glm::radians(45.f), 4.f/3.f, 0.1f, 100.f);
    sp.setMat4("proj", mproj);

    float radius = 10.f;
    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, glfwCursorPosCB);
    glfwSetScrollCallback(window, scrollCB);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // >>>>>>>>>>>>>>>>> 1 >>>>>>>>>>>>>>>>>>>>
        // float camX = sin(glfwGetTime()) * radius;
        // float camZ = cos(glfwGetTime()) * radius;
        // mview = glm::lookAt(glm::vec3{camX, 0.f, camZ}, glm::vec3{0.f, 0.f, 0.f}, glm::vec3{0.f, 1.f, 0.f});

        // >>>>>>>>>>>>>>>>> 2 >>>>>>>>>>>>>>>>>>>>
        // mview = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // >>>>>>>>>>>>>>>>> 3 >>>>>>>>>>>>>>>>>>>>
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // cameraSpeed = 2.5f * deltaTime;
        // mview = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // sp.setMat4("view", mview);
        sp.setMat4("view", insCamera.GetViewMatrix());

        // mproj = glm::perspective(glm::radians(zoom), 4.f/3.f, 0.1f, 100.f);
        mproj = glm::perspective(glm::radians(insCamera.Zoom), 4.f/3.f, 0.1f, 100.f);
        sp.setMat4("proj", mproj);

        glBindVertexArray(VAO);
        for (int i = 0; i < sizeof(cubePositions)/sizeof(cubePositions[0]); ++i) {
            mmodel = glm::mat4(1.f);
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
