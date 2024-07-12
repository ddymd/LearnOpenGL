#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

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

void test() {
    SPDLOG_ERROR("error");
    SPDLOG_CRITICAL("critical");
    SPDLOG_WARN("warn");
    SPDLOG_INFO("info");
    SPDLOG_DEBUG("debug");
    SPDLOG_TRACE("trace");
    SPDLOG_ERROR("测试中文: error");
    SPDLOG_CRITICAL("测试中文: critical");
    SPDLOG_WARN("测试中文: warn");
    SPDLOG_INFO("测试中文: info");
    SPDLOG_DEBUG("测试中文: debug");
    SPDLOG_TRACE("测试中文: trace");
}

void init_logger() {
    auto handlers = std::make_shared<spdlog::file_event_handlers>();
    // handlers->before_open = [](spdlog::filename_t filename) {
    //     SPDLOG_INFO("Before opening {}", filename);
    // };
    handlers->after_open = [](spdlog::filename_t filename, std::FILE *fstream) {
        fputs("===========================================================================================\n", fstream);
        fputs("======================================TempMeasure==========================================\n", fstream);
        fputs("===========================================================================================\n", fstream);
    };
    handlers->before_close = [](spdlog::filename_t filename, std::FILE *fstream) {
        fputs("\n\n", fstream);
    };
    // handlers->after_close = [](spdlog::filename_t filename) {
    //     SPDLOG_INFO("After closing {}", filename);
    // };
    auto filesink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/tempmeasure.txt", 0, 0, false, 0, *handlers);
    auto consolesink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::shared_ptr<spdlog::logger>(new spdlog::logger("default", { filesink, consolesink }));
    spdlog::set_default_logger(logger);
    spdlog::cfg::load_env_levels();
    spdlog::set_pattern("[%^%L%$][%t][%H:%M:%S.%e][%s:%# %!()] %v");
}

// error callback
void glfw_error_callback(int error, const char* description) {
    SPDLOG_ERROR("GLFW errorno({:d}): {}", error, description);
}

// resize callback
void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, w, h);
    // SPDLOG_INFO("resize callback ==> {}x{}", w, h);
}

// Input
void process_input(GLFWwindow* window) {
    // SPDLOG_DEBUG("process input: {}", glfwGetKey(window, GLFW_KEY_F));
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

bool init_triangle(unsigned int* VAO, unsigned int* shaderProgram) {
    glGenVertexArrays(1, VAO);

    // --- vertex input ---
    float vertices[] = {
        -0.5f, -0.5f, 0.f,
        0.5f, -0.5f, 0.f,
        0.f, 0.5f, 0.f
    };
    // vertex buffer objects
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // 1. bind Vertex Array Object
    glBindVertexArray(*VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. then set our vertex attributes poniters
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- vertex shader ---
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // error checking
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::VERTEX::COMPILATION_FAILED {}", infoLog);
        glDeleteShader(vertexShader);
        return false;
    }

    // --- fragment shader ---
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // error checking
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED {}", infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // --- shader program ---
    *shaderProgram = glCreateProgram();
    glAttachShader(*shaderProgram, vertexShader);
    glAttachShader(*shaderProgram, fragmentShader);
    glLinkProgram(*shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // error checking
    glGetShaderiv(*shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(*shaderProgram, 512, NULL, infoLog);
        SPDLOG_ERROR("ERROR::SHADER::PROGRAM::LINK_FAILED {}", infoLog);
        return false;
    }
    return true;
}

int main(int args, char** argv) {
    init_logger();

    // Initializing GLFW
    if (!glfwInit()) {
        SPDLOG_CRITICAL("GLFW init failed");
        return -1;
    }

    // Setting an error callback
    glfwSetErrorCallback(glfw_error_callback);

    // Creating a window and context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef MacOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // MacOS
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL-HelloTriangle", NULL, NULL);
    if (window == NULL) {
        SPDLOG_CRITICAL("Failed to create GLFW window");
        glfwTerminate();
        return -2;
    }

    // Making the OpenGL context current
    glfwMakeContextCurrent(window);

    // Extension loader library initializing
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_CRITICAL("Failed to initialize GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -3;
    }
    test();
    // set view port
    // glViewport(0, 0, 400, 300);

    // set resize callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
    // glClear(GL_COLOR_BUFFER_BIT);
    unsigned int VAO, shaderProgram;
    init_triangle(&VAO, &shaderProgram);

    // Checking the window close flag
    while (!glfwWindowShouldClose(window)) {
        process_input(window);
        glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
        // glDrawArrays(GL_LINES, 0, 3);
        // glDrawArrays(GL_LINE_LOOP, 0, 3);
        // glDrawArrays(GL_LINE_STRIP, 0, 3);
        // glDrawArrays(GL_POINTS, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    // Terminating GLFW
    glfwTerminate();

    return 0;
}
