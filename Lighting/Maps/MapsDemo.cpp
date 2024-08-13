#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "camera.hpp"
#include "shader.hpp"
#include "config.h"

#define SRC_VSHADER_OBJ TOP_SRC_DIR"Lighting/Maps/obj_shader.vs"
#define SRC_FSHADER_OBJ TOP_SRC_DIR"Lighting/Maps/obj_shader.fs"

#define SRC_VSHADER_LIT TOP_SRC_DIR"Lighting/Maps/lit_shader.vs"
#define SRC_FSHADER_LIT TOP_SRC_DIR"Lighting/Maps/lit_shader.fs"

Camera mcam(glm::vec3(0.f, 0.f, 5.f));

uint32_t idx = 1;
glm::vec3 mambient = materials[idx].ambient; // (1.0f, 0.5f, 0.31f);
glm::vec3 mdiffuse = materials[idx].diffuse; // (1.0f, 0.5f, 0.31f);
glm::vec3 mspecular(0.5f, 0.5f, 0.5f);
float mshininess = 64.f;

glm::vec3 lambient(0.3f, 0.3f, 0.3f);
glm::vec3 ldiffuse(0.5f, 0.5f, 0.5f);
glm::vec3 lspecular(1.f, 1.f, 1.f);
glm::vec3 lposition(1.2f, 1.f, 2.f);

void GLFWErrorCB(int ec, const char* emsg);
void ProcessInputs(GLFWwindow* window, float delta);
void GLFWResizeCB(GLFWwindow* window, int w, int h);
void GLFWCursorPosCB(GLFWwindow* window, double xpos, double ypos);
void GLFWScrollCB(GLFWwindow* window, double xoffset, double yoffset);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting Maps", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("GLFW Create Window Faield");
        return -2;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("GLAD Load Proc Failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }
    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(window, GLFWResizeCB);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, GLFWCursorPosCB);
    glfwSetScrollCallback(window, GLFWScrollCB);

    // textures
    unsigned int textures[2];
    glGenTextures(2, textures);
    int w, h, c;
    unsigned char* data = stbi_load(TEXTURE_C0, &w, &h, &c, 0);
    SPDLOG_INFO("comp: {}", c);
    if (data) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        SPDLOG_ERROR("Load Texture({}) Failed", TEXTURE_C0);
    }

    data = stbi_load(TEXTURE_CS, &w, &h, &c, 0);
    SPDLOG_INFO("comp: {}", c);
    if (data) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        SPDLOG_ERROR("Load Texture({}) Failed", TEXTURE_CS);
    }

    unsigned int VAO[2], VBO;
    glGenVertexArrays(2, VAO);
    glBindVertexArray(VAO[0]);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(norvertices), norvertices, GL_STATIC_DRAW);
    // vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader objsp(SRC_VSHADER_OBJ, SRC_FSHADER_OBJ);
    objsp.use();
    objsp.setMat4("model", glm::mat4(1.f));
    objsp.setVec3("light.ambient", lambient);
    objsp.setVec3("light.diffuse", ldiffuse);
    objsp.setVec3("light.specular", lspecular);
    objsp.setVec3("light.position", lposition);

    // objsp.setVec3("material.ambient", mambient);
    objsp.setInt("material.diffuse", 0);
    // objsp.setVec3("material.specular", mspecular);
    objsp.setInt("material.specular", 1);
    objsp.setFloat("material.shininess", mshininess);

    Shader litsp(SRC_VSHADER_LIT, SRC_FSHADER_LIT);
    litsp.use();

    glm::mat4 lmod(1.f);
    lmod = glm::translate(lmod, lposition);
    lmod = glm::scale(lmod, glm::vec3(0.3f));
    litsp.setMat4("model", lmod);

    litsp.setVec3("light.ambient", lambient);
    litsp.setVec3("light.diffuse", ldiffuse);
    litsp.setVec3("light.specular", lspecular);
    // litsp.setVec3("light.position", lposition);

    float lframe = 0.f;
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float cframe = glfwGetTime();
        ProcessInputs(window, cframe-lframe);
        lframe = cframe;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        glm::mat4 mview = mcam.GetViewMatrix();
        glm::mat4 mproj = glm::perspective(glm::radians(mcam.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);

        objsp.use();
        objsp.setVec3("camPos", mcam.Position);
        objsp.setMat4("view", mview);
        objsp.setMat4("proj", mproj);
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
//////////////////////////////////////////////////////////////////////////////
void GLFWErrorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW Error Callback({}): {}", ec, emsg);
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

void GLFWResizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

bool bIsFirstCursor = true;
float lxpos, lypos;
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
