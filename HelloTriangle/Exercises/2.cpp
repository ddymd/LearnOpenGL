// Now create the same 2 triangles using two different VAOs and VBOs for their data

#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "spdlog/spdlog.h"

void glfw_error_callback(int error_code, const char* description) {
    SPDLOG_ERROR("glfw error({:d}): {}", error_code, description);
}

void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vec4(1.f, 0.5f, 0.2f, 1.f);\n"
    "}\0";

int main(int argc, char** argv) {
    // init glfw
    if (!glfwInit()) {
        SPDLOG_CRITICAL("glfw initilization failed.");
        return -1;
    }

    // set error callback
    glfwSetErrorCallback(glfw_error_callback);

    // create window and context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow *window = glfwCreateWindow(800, 600, "HelloTriangle Exercises2", NULL, NULL);
    if (!window) {
        SPDLOG_CRITICAL("glfw create window failed.");
        glfwTerminate();
        return -2;
    }

    // make context
    glfwMakeContextCurrent(window);

    // resize callback
    glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

    // glad initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("glad load gl loader failed.");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }

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

    unsigned int VAO[2];
    glGenVertexArrays(2, VAO);

    unsigned int VBO[2];
    glGenBuffers(2, VBO);
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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
        glDeleteVertexArrays(2, VAO);
        glDeleteBuffers(2, VBO);
        glDeleteShader(vertexShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -4;
    }

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // error checking
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}", infoLog);
        glDeleteVertexArrays(2, VAO);
        glDeleteBuffers(2, VBO);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -5;
    }

    // shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // error checking
    glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::PROGRAM::LINK_FAILED\n{}", infoLog);
        glDeleteVertexArrays(2, VAO);
        glDeleteBuffers(2, VBO);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -6;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
