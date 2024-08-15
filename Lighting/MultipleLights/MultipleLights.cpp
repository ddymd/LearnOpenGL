#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "common.hpp"
#include "config.h"

#define SRC_VSHADER TOP_SRC_DIR"Lighting/MultipleLights/shader.vs"
#define SRC_FSHADER TOP_SRC_DIR"Lighting/MultipleLights/shader.fs"

#define SRC_VSHADER_LIT TOP_SRC_DIR"Lighting/MultipleLights/litshader.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Lighting/MultipleLights/litshader.fs"

Camera mcam(glm::vec3(0.f, 0.f, 3.f));

float mshininess = 32.f;

glm::vec3 lambient(0.1f);
glm::vec3 ldiffuse(0.5f);
glm::vec3 lspecular(1.f);
glm::vec3 ldirection(-0.2f, -1.f, -0.3f);

int main(int argc, char* argv[]) {
    if (!glfwInit()) {
        SPDLOG_CRITICAL("GLFW Init Failed");
        return -1;
    }
    SetCamera(&mcam);
    glfwSetErrorCallback(GLFWErrorCB);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for apple
#endif // __APPLE__
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Multiple Lights", NULL, NULL);
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
    glEnable(GL_DEPTH_TEST);
    // textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, LoadTexture(TEXTURE_C0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, LoadTexture(TEXTURE_CS));

    unsigned int VAO[2], VBO;
    glGenVertexArrays(2, VAO);
    glBindVertexArray(VAO[0]);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norvertices), norvertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader sp(SRC_VSHADER, SRC_FSHADER);
    sp.use();
    sp.setInt("material.diffuse", 0);
    sp.setInt("material.specular", 1);
    sp.setFloat("material.shininess", mshininess);
    sp.setVec3("litdire.direction", ldirection);
    sp.setVec3("litdire.ambient", glm::vec3(0.05f));
    sp.setVec3("litdire.diffuse", glm::vec3(0.4f));
    sp.setVec3("litdire.specular", glm::vec3(0.5f));

    for (int i = 0; i < sizeof(pointLightPositions)/sizeof(pointLightPositions[0]); ++i) {
        sp.setFloat(fmt::format("litpoint[{}].constant", i).c_str(), ldfs[4].kc);
        sp.setFloat(fmt::format("litpoint[{}].linear", i).c_str(), ldfs[4].kl);
        sp.setFloat(fmt::format("litpoint[{}].quadratic", i).c_str(), ldfs[4].kq);
        sp.setVec3(fmt::format("litpoint[{}].position", i).c_str(), pointLightPositions[i]);
        sp.setVec3(fmt::format("litpoint[{}].ambient", i).c_str(), glm::vec3(0.05f));
        sp.setVec3(fmt::format("litpoint[{}].diffuse", i).c_str(), glm::vec3(0.8f));
        sp.setVec3(fmt::format("litpoint[{}].specular", i).c_str(), glm::vec3(1.f));
    }

    sp.setFloat("litspot.constant", ldfs[4].kc);
    sp.setFloat("litspot.linear", ldfs[4].kl);
    sp.setFloat("litspot.quadratic", ldfs[4].kq);
    sp.setFloat("litspot.innercutoff", glm::cos(glm::radians(12.5f)));
    sp.setFloat("litspot.outercutoff", glm::cos(glm::radians(15.f)));
    sp.setVec3("litspot.ambient", glm::vec3(0.f));
    sp.setVec3("litspot.diffuse", glm::vec3(1.f));
    sp.setVec3("litspot.specular", glm::vec3(1.f));

    Shader lsp(SRC_VSHADER_LIT, SRC_FSHADER_LIT);
    lsp.use();

    float lframe = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = mcam.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(mcam.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);

        sp.use();
        sp.setMat4("view", view);
        sp.setMat4("proj", proj);
        sp.setVec3("camPos", mcam.Position);
        sp.setVec3("litspot.position", mcam.Position);
        sp.setVec3("litspot.direction", mcam.Front);

        glBindVertexArray(VAO[0]);
        for (int i = 0; i < sizeof(cubePositions)/sizeof(cubePositions[0]); ++i) {
            glm::mat4 model(1.f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(i*20.f), {1.0f, 0.3f, 0.5f});
            sp.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        lsp.use();
        lsp.setMat4("view", view);
        lsp.setMat4("proj", proj);

        glBindVertexArray(VAO[1]);
        for (int i = 0; i < sizeof(pointLightPositions)/sizeof(pointLightPositions[0]); ++i) {
            glm::mat4 model(1.f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lsp.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
