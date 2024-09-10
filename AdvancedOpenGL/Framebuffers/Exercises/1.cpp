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

    unsigned int VAO[3], VBO[3];
    glGenVertexArrays(3, VAO);
    glGenBuffers(3, VBO);
    // cubes
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // plane
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // screen
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices1), quadVertices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    Shader sp(SRC_VSHADER, SRC_FSHADER);
    Shader screensp(SRC_VSHADER_SCREEN, SRC_FSHADER_SCREEN);

    unsigned int texcontainer = LoadTexture(TEXTURE_CONTAINER);
    unsigned int texmetal = LoadTexture(TEXTURE_METAL);
    glActiveTexture(GL_TEXTURE0);

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // texture attachment
    unsigned int texframebuffer;
    glGenTextures(1, &texframebuffer);
    glBindTexture(GL_TEXTURE_2D, texframebuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texframebuffer, 0);
    // rendering buffer attachment
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        SPDLOG_ERROR("ERROR::FRAMEBUFFER::STATUS Not Complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float lframe = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;
        glActiveTexture(GL_TEXTURE0);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 model(1.f);
        mcam.Yaw += 180.f;
        mcam.ProcessMouseMovement(0, 0, false);
        glm::mat4 view = mcam.GetViewMatrix();
        mcam.Yaw -= 180.f;
        mcam.ProcessMouseMovement(0, 0, true);
        glm::mat4 proj = glm::perspective(glm::radians(mcam.Zoom), 4.f/3.f, 0.1f, 100.f);
        sp.use();
        sp.setMat4("view", view);
        sp.setMat4("proj", proj);
        sp.setFloat("texture0", 0);

        model = glm::translate(model, cubepos1);
        sp.setMat4("model", model);
        glBindTexture(GL_TEXTURE_2D, texcontainer);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.f);
        model = glm::translate(model, cubepos2);
        sp.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindTexture(GL_TEXTURE_2D, texmetal);
        glBindVertexArray(VAO[1]);
        sp.setMat4("model", glm::mat4(1.f));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        model = glm::mat4(1.f);
        view = mcam.GetViewMatrix();
        sp.setMat4("view", view);

        model = glm::translate(model, cubepos1);
        sp.setMat4("model", model);
        glBindTexture(GL_TEXTURE_2D, texcontainer);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.f);
        model = glm::translate(model, cubepos2);
        sp.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindTexture(GL_TEXTURE_2D, texmetal);
        glBindVertexArray(VAO[1]);
        sp.setMat4("model", glm::mat4(1.f));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glClearColor(0.8f, 0.8f, 0.8f, 1.f);
        // glClear(GL_COLOR_BUFFER_BIT);

        screensp.use();
        screensp.setFloat("texture0", 0);
        glBindTexture(GL_TEXTURE_2D, texframebuffer);
        glBindVertexArray(VAO[2]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
