// Use a uniform variable as the mix function’s third parameter to vary the amount the two
//  textures are visible. Use the up and down arrow keys to change how much the container or
//  the smiley face is visible. Solution: /src/1.getting_started/4.6.textures_
//  exercise4/.

#include "spdlog/spdlog.h"
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "shader.h"
#include "config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float mvertices[] = {
    // positions        // colors           // texture coords
    0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
    0.5f,-0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    -0.5f,-0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
};

unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

// shader program
Shader* sp {nullptr};

float vRatio = 0.2f;

void errorCB(int ec, const char* emsg) {
    SPDLOG_ERROR("GLFW::ERROR_CALLBACK({}): {}", ec, emsg);
}

void resizeCB(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (!sp) return;
        vRatio += 0.005;
        if (vRatio > 1) {
            vRatio = 1;
        }
        sp->setFloat("vRatio", vRatio);
        // SPDLOG_INFO("GLFW_KEY_UP vRatio: {}", vRatio);
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (!sp) return;
        vRatio -= 0.005;
        if (vRatio < 0) {
            vRatio = 0;
        }
        sp->setFloat("vRatio", vRatio);
        // SPDLOG_INFO("GLFW_KEY_DOWN vRatio: {}", vRatio);
        return;
    }
}

int main(int argc, char** argv) {
    // init glfw
    if (!glfwInit()) {
        SPDLOG_CRITICAL("glfw initialize failed");
        return -1;
    }
    // set error cb
    glfwSetErrorCallback(errorCB);
    // create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // for apple
#endif  // __APPLE__
    GLFWwindow* window = glfwCreateWindow(800, 600, "Textures-Exercises4", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("create glfw window failed");
        return -2;
    }
    // make context
    glfwMakeContextCurrent(window);
    // set resize callback
    glfwSetFramebufferSizeCallback(window, resizeCB);
    // glad initialize
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("glad load proc failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }
    // >>>>>>>>>>>>>>>>>>> program
    sp = new Shader(TOP_SRC_DIR"Textures/Exercises/texture4.vs", TOP_SRC_DIR"Textures/Exercises/texture4.fs");
    sp->use();
    sp->setInt("texture1", 0);
    sp->setInt("texture2", 1);
    sp->setFloat("vRatio", vRatio);

    // >>>>>>>>>>>>>>>>>>> VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // >>>>>>>>>>>>>>>>>>> VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mvertices), mvertices, GL_STATIC_DRAW);

    // >>>>>>>>>>>>>>>>>>> EBO
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // >>>>>>>>>>>>>>>>>>> attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    // >>>>>>>>>>>>>>>>>>> texture
    unsigned int textures[2];
    glGenTextures(2, textures);
    stbi_set_flip_vertically_on_load(true);
    int w, h, c;
    unsigned char* data = stbi_load(RESOURCES_DIR"container.jpg", &w, &h, &c, 0);
    if (data) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        SPDLOG_ERROR("load texture1({}) failed", RESOURCES_DIR"container.jpg");
    }

    data = stbi_load(RESOURCES_DIR"awesomeface.png", &w, &h, &c, 0);
    if (data) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        SPDLOG_ERROR("load texture2({}) failed", RESOURCES_DIR"awesomeface.png");
    }

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
