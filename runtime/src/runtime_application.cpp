#include "runtime_application.h"

#include "engine.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace {
void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

GLuint compile_shader(GLenum type, const char* source) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512] = {};
        glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);
        NGIN_ERROR("Shader compilation failed: {}", info_log);
    }

    return shader;
}

GLuint create_triangle_program() {
    constexpr const char* vertex_source = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";

    constexpr const char* fragment_source = R"(
        #version 330 core
        out vec4 FragColor;

        void main() {
            FragColor = vec4(1.0, 0.4, 0.2, 1.0);
        }
    )";

    const GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    const GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512] = {};
        glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);
        NGIN_ERROR("Program linking failed: {}", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}
} // namespace

namespace {
GLFWwindow* runtime_window(ngin::Application* app) {
    return static_cast<GLFWwindow*>(app->get_native_window_handle());
}
} // namespace

void RuntimeApplication::on_create() {
    if (!glfwInit()) {
        NGIN_ERROR("Failed to initialize GLFW");
        m_running = false;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ngin2D Runtime", nullptr, nullptr);
    if (!window) {
        NGIN_ERROR("Failed to create GLFW window");
        glfwTerminate();
        m_running = false;
        return;
    }
    set_native_window_handle(window);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        NGIN_ERROR("Failed to initialize GLAD");
        glfwDestroyWindow(window);
        set_native_window_handle(nullptr);
        glfwTerminate();
        m_running = false;
        return;
    }

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
    glViewport(0, 0, framebuffer_width, framebuffer_height);

    constexpr float triangle_vertices[] = {
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    m_shader_program = create_triangle_program();
}

void RuntimeApplication::on_destroy() {
    glDeleteProgram(m_shader_program);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);

    GLFWwindow* window = runtime_window(this);
    if (window) {
        glfwDestroyWindow(window);
        set_native_window_handle(nullptr);
    }

    glfwTerminate();
}

void RuntimeApplication::on_update(float) {
    GLFWwindow* window = runtime_window(this);
    if (!window) {
        m_running = false;
        return;
    }

    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (glfwWindowShouldClose(window)) {
        m_running = false;
        return;
    }

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_shader_program);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);

    NGIN_INFO("Mouse Position: {}, {}", ngin::Input::mouse_x(), ngin::Input::mouse_y());
}

void RuntimeApplication::on_event(ngin::Event&) {
}