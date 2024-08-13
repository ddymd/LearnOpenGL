// Can you simulate some of the real-world objects by defining their respective materials
// like we’ve seen at the start of this chapter? Note that the linked table’s ambient values
// are not the same as the diffuse values; they didn’t take light intensities into account. To
// correctly set their values you’d have to set all the light intensities to vec3(1.0) to get the
// same output. Solution: /src/2.lighting/3.2.materials_exercise1/ of cyan
// plastic container

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "config.h"

#define SRC_VSHADER_OBJ TOP_SRC_DIR"Lighting/Materials/Exercises/obj_shader2.vs"
#define SRC_FSHADER_OBJ TOP_SRC_DIR"Lighting/Materials/Exercises/obj_shader2.fs"

#define SRC_VSHADER_LIT TOP_SRC_DIR"Lighting/Materials/Exercises/lit_shader2.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Lighting/Materials/Exercises/lit_shader2.fs"

void GLFWErrorCB(int ec, const char* emsg);
void ProcessInputs(GLFWwindow* window, float delta);
void GLFWResizeCB(GLFWwindow* window, int w, int h);
void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos);
void GLFWScrollCB(GLFWwindow* window, double xoffset, double yoffset);

uint32_t idx = 0;
glm::vec3 mambient = materials[idx].ambient; // (1.0f, 0.5f, 0.31f);
glm::vec3 mdiffuse = materials[idx].diffuse; // (1.0f, 0.5f, 0.31f);
glm::vec3 mspecular = materials[idx].specular; // (0.5f, 0.5f, 0.5f);
float mshininess = materials[idx].shininess; // (32.0f);

glm::vec3 lambient(0.2f, 0.2f, 0.2f);
glm::vec3 ldiffuse(0.5, 0.5f, 0.5f);
glm::vec3 lspecular(1.f, 1.f, 1.f);
glm::vec3 lposition(1.2f, 1.f, 2.f);
Camera mcam(glm::vec3(0.f, 0.f, 3.f));

int main(int argc, char** argv) {
    if (!glfwInit()) {
        SPDLOG_CRITICAL("GLFW Init Failed");
        return -1;
    }
    glfwSetErrorCallback(GLFWErrorCB);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for apple
#endif // __APPLE__
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Materials Exercise2", NULL, NULL);
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
    glfwSetFramebufferSizeCallback(window, GLFWResizeCB);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, GLFWCursorPosCB);
    glfwSetScrollCallback(window, GLFWScrollCB);
    glEnable(GL_DEPTH_TEST);

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

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader osp(SRC_VSHADER_OBJ, SRC_FSHADER_OBJ);
    osp.use();
    osp.setMat4("model", glm::mat4(1.f));
    osp.setVec3("material.ambient", mambient);
    osp.setVec3("material.diffuse", mdiffuse);
    osp.setVec3("material.specular", mspecular);
    osp.setFloat("material.shininess", mshininess);
    // osp.setVec3("light.ambient", lambient);
    // osp.setVec3("light.diffuse", ldiffuse);
    // osp.setVec3("light.specular", lspecular);
    osp.setVec3("light.ambient", glm::vec3(1.f));
    osp.setVec3("light.diffuse", glm::vec3(1.f));
    osp.setVec3("light.specular", glm::vec3(1.f));
    osp.setVec3("light.position", lposition);

    Shader lsp(SRC_VSHADER_LIT, SRC_FSHADER_LIT);
    lsp.use();
    glm::mat4 lmodel(1.f);
    lmodel = glm::translate(lmodel, lposition);
    lmodel = glm::scale(lmodel, glm::vec3(0.3f));
    lsp.setMat4("model", lmodel);
    lsp.setVec3("light.ambient", lambient);
    lsp.setVec3("light.diffuse", ldiffuse);
    lsp.setVec3("light.specular", lspecular);

    float lframe = 0.f;
    float gap = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        float delta = cframe - lframe;
        ProcessInputs(window, delta);
        lframe = cframe;
        if (gap > 3) {
            gap = 0;
            if (++idx > 23) idx = 0;
            SPDLOG_INFO("material change to => {}: {}", idx, materials[idx].name);
        }
        gap+=delta;
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 mview = mcam.GetViewMatrix();
        glm::mat4 mproj = glm::perspective(glm::radians(mcam.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);

        osp.use();
        osp.setMat4("view", mview);
        osp.setMat4("proj", mproj);
        osp.setVec3("camPos", mcam.Position);

        osp.setVec3("material.ambient", materials[idx].ambient);
        osp.setVec3("material.diffuse", materials[idx].diffuse);
        osp.setVec3("material.specular", materials[idx].specular);
        osp.setFloat("material.shininess", materials[idx].shininess);

        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lsp.use();
        lsp.setMat4("view", mview);
        lsp.setMat4("proj", mproj);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

///////////////////////////////////////////////////////////////////////////
void GLFWErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR_CB({}): {}", ec, emsg);
}

float tdelta = 0;
void ProcessInputs(GLFWwindow* window, float delta) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_W) ==  GLFW_PRESS) {
        mcam.ProcessKeyboard(ECameraMovement::FORWARD, delta);
    }
    if (glfwGetKey(window, GLFW_KEY_S) ==  GLFW_PRESS) {
        mcam.ProcessKeyboard(ECameraMovement::BACKWARD, delta);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_A) ==  GLFW_PRESS) {
        mcam.ProcessKeyboard(ECameraMovement::LEFT, delta);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_D) ==  GLFW_PRESS) {
        mcam.ProcessKeyboard(ECameraMovement::RIGHT, delta);
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        tdelta += delta;
        if (tdelta > 0.1) {
            tdelta = 0;
            idx++;
            if (idx > 23) idx = 0;
            SPDLOG_INFO("material[{}] name: {}", idx, materials[idx].name);
        }
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        tdelta += delta;
        if (tdelta > 0.1) {
            tdelta = 0;
            if (idx == 0) idx = 24;
            idx--;
            SPDLOG_INFO("material[{}] name: {}", idx, materials[idx].name);
        }
        return;
    }
}

void GLFWResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

float lxpos, lypos;
bool bIsFirstCursor = true;
void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos) {
    if (bIsFirstCursor) {
        bIsFirstCursor = false;
    } else {
        mcam.ProcessMouseMovement(xpos-lxpos, ypos-lypos);
    }
    lxpos = xpos;
    lypos = ypos;
}

void GLFWScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
    mcam.ProcessMouseScroll(yoffset);
}
