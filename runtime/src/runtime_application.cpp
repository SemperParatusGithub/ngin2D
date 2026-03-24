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
    return app->get_native_window_handle();
}
} // namespace

void RuntimeApplication::on_create() {
    m_window = std::make_unique<ngin::Window>(1280, 720, "ngin2D Runtime");
    if (!m_window->valid()) {
        m_running = false;
        return;
    }

    set_native_window_handle(m_window->native_handle());
    GLFWwindow* window = runtime_window(this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    m_context = std::make_unique<ngin::OpenGLContext>(m_window->native_handle());
    if (!m_context->init()) {
        m_context.reset();
        m_window.reset();
        set_native_window_handle(nullptr);
        m_running = false;
        return;
    }
    m_context->set_vsync(true);

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
    if (m_shader_program != 0) {
        glDeleteProgram(m_shader_program);
        m_shader_program = 0;
    }
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    set_native_window_handle(nullptr);
    m_context.reset();
    m_window.reset();
}

void RuntimeApplication::on_update(float) {
    if (!m_window || !m_window->valid()) {
        m_running = false;
        return;
    }

    m_window->poll_events();
    if (ngin::Input::is_key_pressed(ngin::KeyCode::escape)) {
        m_window->set_should_close(true);
    }

    if (m_window->should_close()) {
        m_running = false;
        return;
    }

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_shader_program);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    m_window->swap_buffers();
}

void RuntimeApplication::on_event(ngin::Event&) {
}