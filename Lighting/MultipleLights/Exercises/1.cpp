#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "common.hpp"
#include "config.h"

#define SRC_VSHADER TOP_SRC_DIR"Lighting/MultipleLights/Exercises/shader.vs"
#define SRC_FSHADER TOP_SRC_DIR"Lighting/MultipleLights/Exercises/shader.fs"

#define SRC_VSHADER_LIT TOP_SRC_DIR"Lighting/MultipleLights/Exercises/litshader.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Lighting/MultipleLights/Exercises/litshader.fs"

constexpr int NR_POINT_LIGHTS = 4;

LightStyle styles[] = { styleDesert , styleFactory, styleHorror, styleBiochemicalLab };
int selectedIndex = 1;

glm::vec3 dirlitdirection(-0.2f, -1.0f, -0.3f);
Camera mcam(glm::vec3(0.f, 0.f, 3.f));

int main() {
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MultipleLights Exercises1", NULL, NULL);
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
    // VAO, VBO
    unsigned int VAO[2], VBO;
    glGenVertexArrays(2, VAO);
    glBindVertexArray(VAO[0]);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norvertices), norvertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // shaders
    Shader sobj(SRC_VSHADER, SRC_FSHADER);
    sobj.use();
    // material
    sobj.setInt("material.diffuse", 0);
    sobj.setInt("material.specular", 1);
    sobj.setFloat("material.shininess", 32.f);
    // directional light
    sobj.setVec3("litdirectional.direction", dirlitdirection);
    glm::vec3* pdirlitcolors = styles[selectedIndex].DLColors;
    sobj.setVec3("litdirectional.ambient", pdirlitcolors[0]);
    sobj.setVec3("litdirectional.diffuse", pdirlitcolors[1]);
    sobj.setVec3("litdirectional.specular", pdirlitcolors[2]);
    // point light
    glm::vec3* ppointlitcolors = styles[selectedIndex].PLColors;
    for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
        sobj.setVec3(fmt::format("litpoint[{}].position", i).c_str(), pointLightPositions[i]);
        sobj.setVec3(fmt::format("litpoint[{}].ambient", i).c_str(), ppointlitcolors[i] * 0.1f);
        sobj.setVec3(fmt::format("litpoint[{}].diffuse", i).c_str(), ppointlitcolors[i]);
        sobj.setVec3(fmt::format("litpoint[{}].specular", i).c_str(), ppointlitcolors[i]);
        float* pattenuations = styles[selectedIndex].PLAttenuations[i];
        sobj.setFloat(fmt::format("litpoint[{}].constant", i).c_str(), pattenuations[0]);
        sobj.setFloat(fmt::format("litpoint[{}].linear", i).c_str(), pattenuations[1]);
        sobj.setFloat(fmt::format("litpoint[{}].quadratic", i).c_str(), pattenuations[2]);
    }
    // spot light
    glm::vec3* pspotlitcolors = styles[selectedIndex].SLColors;
    sobj.setVec3("litspot.ambient", pspotlitcolors[0]);
    sobj.setVec3("litspot.diffuse", pspotlitcolors[1]);
    sobj.setVec3("litspot.specular", pspotlitcolors[2]);
    float* pattenuations = styles[selectedIndex].SLAttenuations;
    sobj.setFloat("litspot.constant", pattenuations[0]);
    sobj.setFloat("litspot.linear", pattenuations[1]);
    sobj.setFloat("litspot.quadratic", pattenuations[2]);
    float* pintensities = styles[selectedIndex].SLIntensities;
    sobj.setFloat("litspot.innercutoff", glm::cos(glm::radians(pintensities[0])));
    sobj.setFloat("litspot.outercutoff", glm::cos(glm::radians(pintensities[1])));

    Shader slit(SRC_VSHADER_LIT, SRC_FSHADER_LIT);
    slit.use();

    float lframe = 0.f;
    float* pclearcolor = styles[selectedIndex].clearColor;
    glm::vec3* ppointlitghtcolor = styles[selectedIndex].PLColors;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe - lframe);
        lframe = cframe;
        glClearColor(pclearcolor[0], pclearcolor[1], pclearcolor[2], pclearcolor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = mcam.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(mcam.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);

        sobj.use();
        sobj.setMat4("view", view);
        sobj.setMat4("proj", proj);
        sobj.setVec3("litspot.position", mcam.Position);
        sobj.setVec3("litspot.direction", mcam.Front);
        sobj.setVec3("camPos", mcam.Position);
        glBindVertexArray(VAO[0]);
        for (int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); ++i) {
            glm::mat4 model(1.f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(20.f * i), glm::vec3(1.f, 0.3f, 0.5f));
            sobj.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        slit.use();
        slit.setMat4("view", view);
        slit.setMat4("proj", proj);
        glBindVertexArray(VAO[1]);
        for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
            glm::mat4 model(1.f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            slit.setMat4("model", model);
            slit.setVec3("litcolor", ppointlitghtcolor[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
