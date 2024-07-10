#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

void test() {
    SPDLOG_ERROR("error");
    SPDLOG_CRITICAL("critical");
    SPDLOG_WARN("warn");
    SPDLOG_INFO("info");
    SPDLOG_DEBUG("debug");
    SPDLOG_TRACE("trace");
}

void init_logger() {
    auto logger = std::make_shared<spdlog::logger>("console", std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    spdlog::set_default_logger(logger);
    spdlog::cfg::load_env_levels();
    spdlog::set_pattern("[%^%L%$][%t][%H:%M:%S.%e][%s:%# %!()] %v");
}

void glfw_error_callback(int error, const char* description) {
    SPDLOG_ERROR("GLFW errorno({:d}): {}", error, description);
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

    // Checking the window close flag
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    // Terminating GLFW
    glfwTerminate();

    return 0;
}
