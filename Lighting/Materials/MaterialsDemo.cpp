#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "config.h"
#include "camera.h"
#include "shader.h"

#define SRC_VSHADER_OBJ TOP_SRC_DIR"Lighting/Materials/obj_shader.vs"
#define SRC_FSHADER_OBJ TOP_SRC_DIR"Lighting/Materials/obj_shader.fs"
#define SRC_VSHADER_LIT TOP_SRC_DIR"Lighting/Materials/lit_shader.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Lighting/Materials/lit_shader.fs"

glm::vec3 litPos(1.2f, 1.f, 2.f);
glm::vec3 litColor(1.0f, 1.0f, 1.0f);
glm::vec3 objColor(1.0f, 0.5f, 0.31f);

Camera mcam(glm::vec3(0.f, 0.f, 3.f));

void GLFWErrorCB(int ec, const char* emsg);
void GLFWResizeCB(GLFWwindow* window, int w, int h);
void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos);
void GLFWCursorScrollCB(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInputs(GLFWwindow* window, float delta);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Materials", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("GLFW Create Window Failed");
        return -2;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("GLAD Load Proc Addr Failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }
    glfwSetFramebufferSizeCallback(window, GLFWResizeCB);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, GLFWCursorPosCB);
    glfwSetScrollCallback(window, GLFWCursorScrollCB);
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

    Shader objsp(SRC_VSHADER_OBJ, SRC_FSHADER_OBJ);
    objsp.use();
    objsp.setMat4("model", glm::mat4(1.f));
    objsp.setVec3("litColor", litColor);
    // objsp.setVec3("objColor", objColor);
    // objsp.setVec3("litPos", litPos);
    objsp.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    objsp.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    objsp.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    objsp.setFloat("material.shininess", 32.0f);

    objsp.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    objsp.setVec3("light.diffuse", 0.5, 0.5f, 0.5f);
    objsp.setVec3("light.specular", 1.f, 1.f, 1.f);
    objsp.setVec3("light.position", litPos);

    Shader litsp(SRC_VSHADER_LIT, SRC_FSHADER_LIT);
    litsp.use();
    glm::mat4 litmodel(1.f);
    litmodel = glm::translate(litmodel, litPos);
    litmodel = glm::scale(litmodel, glm::vec3(0.2f));
    litsp.setMat4("model", litmodel);
    litsp.setVec3("litColor", litColor);

    glm::vec3 objlitcolor;
    float lframe = 0.f;
    while (!glfwWindowShouldClose(window)) {
        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;

        objlitcolor.x = sin(cframe * 2.0f);
        objlitcolor.y = sin(cframe * 0.7f);
        objlitcolor.z = sin(cframe * 1.3f);

        glm::vec3 diffuseColor = objlitcolor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mview = mcam.GetViewMatrix();
        glm::mat mproj = glm::perspective(glm::radians(mcam.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);

        objsp.use();
        objsp.setMat4("view", mview);
        objsp.setMat4("proj", mproj);
        objsp.setVec3("camPos", mcam.Position);

        objsp.setVec3("light.ambient", ambientColor);
        objsp.setVec3("light.diffuse", diffuseColor);

        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        litsp.use();
        litsp.setMat4("view", mview);
        litsp.setMat4("proj", mproj);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

/**
 * @brief GLFWErrorCB
 * @param ec
 * @param emsg
 */
void GLFWErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW ERROR CB({}): {}", ec, emsg);
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

void GLFWCursorScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
    mcam.ProcessMouseScroll(yoffset);
}

void ProcessInputs(GLFWwindow* window, float delta) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        mcam.ProcessKeyboard(ECameraMovement::FORWARD, delta);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        mcam.ProcessKeyboard(ECameraMovement::BACKWARD, delta);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        mcam.ProcessKeyboard(ECameraMovement::LEFT, delta);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        mcam.ProcessKeyboard(ECameraMovement::RIGHT, delta);
    }
}
