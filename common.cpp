#include "common.hpp"
#include "camera.hpp"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Camera* gcam = nullptr;

void GLFWErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW Error Callback({}): {}", ec, emsg);
}

void GLFWFrameResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

float lxpos, lypos;
bool bIsFirstCursor = true;
void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos) {
    if (!gcam) {
        SPDLOG_CRITICAL("Camera not set");
        return;
    }
    if (bIsFirstCursor) {
        bIsFirstCursor = false;
    } else {
        gcam->ProcessMouseMovement(xpos-lxpos, ypos-lypos);
    }
    lxpos = xpos;
    lypos = ypos;
}

void GLFWScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
    if (!gcam) {
        SPDLOG_CRITICAL("Camera not set");
        return;
    }
    gcam->ProcessMouseScroll(yoffset);
}

void ProcessInputs(GLFWwindow* window, float delta) {
    if (!gcam) {
        SPDLOG_CRITICAL("Camera not set");
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        gcam->ProcessKeyboard(ECameraMovement::FORWARD, delta);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        gcam->ProcessKeyboard(ECameraMovement::BACKWARD, delta);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        gcam->ProcessKeyboard(ECameraMovement::LEFT, delta);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        gcam->ProcessKeyboard(ECameraMovement::RIGHT, delta);
    }
}

void SetCamera(Camera* pcam) {
    gcam = pcam;
}

unsigned int LoadTexture(char const* path) {
    unsigned int texture;
    glGenTextures(1, &texture);
    stbi_set_flip_vertically_on_load(true);
    int w, h, c;
    unsigned char* data = stbi_load(path, &w, &h, &c, 0);
    if (data) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        GLenum fmt = GL_RGB;
        switch (c) {
        case 1:
            fmt = GL_RED;
            break;
        case 3:
            fmt = GL_RGB;
            break;
        case 4:
            fmt = GL_RGBA;
            break;
        default:
            SPDLOG_WARN("Unhandled image format(channel={})", c);
            break;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        SPDLOG_WARN("Load Texture({}) Failed", path);
    }
    return texture;
}
