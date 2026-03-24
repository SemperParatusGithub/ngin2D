#include "runtime_application.h"

#include "engine.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include <span>

namespace {
void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}
} // namespace

void RuntimeApplication::on_create() {
    m_window = std::make_unique<ngin::Window>(1280, 720, "ngin2D Runtime");
    if (!m_window->valid()) {
        m_running = false;
        return;
    }

    set_native_window_handle(m_window->native_handle());
    GLFWwindow* window = m_window->native_handle();
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

    constexpr std::array<ngin::f32, 9> triangle_vertices = {
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };
    constexpr std::array<ngin::u32, 3> triangle_indices = {0, 1, 2};

    ngin::GraphicsPipeline::Layout layout = {
        {
            {"a_position", ngin::VertexFormat::float3, false}
        }
    };
    m_pipeline = std::make_unique<ngin::GraphicsPipeline>(
        std::span<const ngin::f32>(triangle_vertices),
        std::span<const ngin::u32>(triangle_indices),
        layout
    );

    constexpr std::string_view vertex_source = R"(
        #version 330 core
        layout (location = 0) in vec3 a_position;

        void main() {
            gl_Position = vec4(a_position, 1.0);
        }
    )";
    constexpr std::string_view fragment_source = R"(
        #version 330 core
        out vec4 FragColor;

        void main() {
            FragColor = vec4(1.0, 0.4, 0.2, 1.0);
        }
    )";
    m_shader = std::make_unique<ngin::Shader>(vertex_source, fragment_source);
    if (m_shader->id() == 0) {
        NGIN_ERROR("Failed to create runtime shader");
        m_running = false;
        return;
    }
}

void RuntimeApplication::on_destroy() {
    m_pipeline.reset();
    m_shader.reset();

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

    m_shader->bind();
    m_pipeline->bind();
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(m_pipeline->index_buffer->count()),
        GL_UNSIGNED_INT,
        nullptr
    );

    m_window->swap_buffers();
}

void RuntimeApplication::on_event(ngin::Event&) {
}