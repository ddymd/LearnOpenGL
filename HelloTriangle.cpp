#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
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
    glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Checking the window close flag
    while (!glfwWindowShouldClose(window)) {
        process_input(window);
        // glClearColor(0.2f, 0.3f, 0.3f, 0.02f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    // Terminating GLFW
    glfwTerminate();

    return 0;
}
