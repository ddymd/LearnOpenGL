// 1. Try to draw 2 triangles next to each other using glDrawArrays by adding more
// vertices to your data. Solution: /src/1.getting_started/2.3.hello_triangle_
// exercise1/


#include <cstddef>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "spdlog/spdlog.h"

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

bool DrawTriangles(unsigned int* VAO, unsigned int* shaderProgram) {
    float vertices1[] = {
        // first triangle
        -1.f, -0.5f, 0.f,
        0.f, -0.5f, 0.f,
        -0.5f, 0.5f, 0.f,
        // second triangle
        0.f, -0.5f, 0.f,
        1.f, -0.5f, 0.f,
        0.5f, 0.5f, 0.f
    };

    float vertices2[] = {
        // first triangle
        -0.9f, -0.5f, 0.0f,  // left 
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f,  // top 
        // second triangle
         0.0f, -0.5f, 0.0f,  // left
         0.9f, -0.5f, 0.0f,  // right
         0.45f, 0.5f, 0.0f   // top 
    };

    // unsigned int VAO[2];
    glGenVertexArrays(1, VAO);
    // bind the Vertex array object first, then bind and set vertex buffer(s)
    // and then configure vertex arrtibutes
    glBindVertexArray(*VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO
    // as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

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
        return false;
    }

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // shader program
    *shaderProgram = glCreateProgram();
    glAttachShader(*shaderProgram, vertexShader);
    glAttachShader(*shaderProgram, fragmentShader);
    glLinkProgram(*shaderProgram);
    glGetShaderiv(*shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::PROGRAM::LINK_FAILED\n{}", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

// error callback
void glfw_error_callback(int error, const char* description) {
    SPDLOG_ERROR("GLFW errorno({:d}): {}", error, description);
}

int main() {
    // init glfw
    if (!glfwInit()) {
        SPDLOG_ERROR("glfw init failed");
        return -1;
    }

    // Setting an error callback
    glfwSetErrorCallback(glfw_error_callback);

    // create window and context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef MacOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // MacOS
    GLFWwindow* window = glfwCreateWindow(800, 600, "Exercises-1", NULL, NULL);
    if (window == NULL) {
        SPDLOG_ERROR("create glfw window failed");
        glfwTerminate();
        return -2;
    }

    // making the opengl context current
    glfwMakeContextCurrent(window);

    // GLAD 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("Failed to initialize GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }

    unsigned int VAO;
    unsigned int shaderProgram;
    DrawTriangles(&VAO, &shaderProgram);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // glDrawArrays(GL_TRIANGLES, 3, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}