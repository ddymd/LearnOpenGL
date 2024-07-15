// Create two shader programs where the second program uses a different fragment shader
//  that outputs the color yellow; draw both triangles again where one outputs the color yellow.
//  Solution: /src/1.getting_started/2.5.hello_triangle_exercise3/.

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "spdlog/spdlog.h"

const char *vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main() {\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char *fragmentShaderSource1 =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"   FragColor = vec4(1.f, 0.5f, 0.2f, 1.f);\n"
"}\0";
const char *fragmentShaderSource2 =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"   FragColor = vec4(1.f, 1.f, 0.f, 1.f);\n"
"}\0";

float vertices1[] = {
    // first triangle
    -1.f, -0.5f, 0.f,
    0.f, -0.5f, 0.f,
    -0.5f, 0.5f, 0.f,
};

float vertices2[] = {
    // second triangle
    0.f, -0.5f, 0.f,
    1.f, -0.5f, 0.f,
    0.5f, 0.5f, 0.f
};

void glfwErrorCallback(int error_code, const char* info) {
    SPDLOG_ERROR("glfw error({:d}): {}", error_code, info);
}

void glfwFramebuffersizeCallback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(int argc, char** argv) {
    // initialization
    if (!glfwInit()) {
        SPDLOG_CRITICAL("initialization glfw failed");
        return -1;
    }

    // set error callback
    glfwSetErrorCallback(glfwErrorCallback);

    // create window and context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__
    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Triangle - Exercises3", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("create glfw window failed");
        return -2;
    }
    glfwMakeContextCurrent(window);

    // set resize callback
    glfwSetFramebufferSizeCallback(window, glfwFramebuffersizeCallback);

    // load
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("load glfw proc failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    SPDLOG_INFO("Maximum nr of vertex attributes supported: {:d}", nrAttributes);

    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // error checking
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", infoLog);
        glDeleteShader(vertexShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -4;
    }

    // fragment shaders
    unsigned int fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
    glCompileShader(fragmentShader1);
    glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader1, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::FRAGMENT1::COMPILATION_FAILED\n{}", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader1);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -4;
    }

    unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
    glCompileShader(fragmentShader2);
    glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::FRAGMENT2::COMPILATION_FAILED\n{}", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader1);
        glDeleteShader(fragmentShader2);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -4;
    }

    // shader programs
    unsigned int shaderProgram1 = glCreateProgram();
    glAttachShader(shaderProgram1, vertexShader);
    glAttachShader(shaderProgram1, fragmentShader1);
    glLinkProgram(shaderProgram1);
    glGetShaderiv(shaderProgram1, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram1, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::PROGRAM1::LINK_FAILED\n{}", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader1);
        glDeleteShader(fragmentShader2);
        glDeleteShader(shaderProgram1);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -5;
    }

    unsigned int shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);

    glGetShaderiv(shaderProgram2, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram2, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::PROGRAM2::LINK_FAILED\n{}", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader1);
        glDeleteShader(fragmentShader2);
        glDeleteShader(shaderProgram1);
        glDeleteShader(shaderProgram2);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -5;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader1);
    glDeleteShader(fragmentShader2);

    unsigned int VAO[2];
    unsigned int VBO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram1);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUseProgram(shaderProgram2);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteShader(shaderProgram1);
    glDeleteShader(shaderProgram2);
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

