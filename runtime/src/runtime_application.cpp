#include "runtime_application.h"

#include "engine.h"
#include <glad/glad.h>
#include <array>
#include <filesystem>
#include <optional>
#include <span>


void RuntimeApplication::on_create() {
    m_window = ngin::create_scope<ngin::Window>(1280, 720, "ngin2D Runtime");
    if (!m_window->valid()) {
        m_running = false;
        return;
    }

    set_native_window_handle(m_window->native_handle());

    m_context = ngin::create_scope<ngin::OpenGLContext>(m_window->native_handle());
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
    m_pipeline = ngin::create_scope<ngin::GraphicsPipeline>(
        std::span<const ngin::f32>(triangle_vertices),
        std::span<const ngin::u32>(triangle_indices),
        layout
    );

    m_shader = ngin::create_scope<ngin::Shader>();
    bool shader_loaded = m_shader->load_from_files(
        std::filesystem::path("assets/shaders/triangle.vert"),
        std::filesystem::path("assets/shaders/triangle.frag")
    );
    if (!shader_loaded || m_shader->id() == 0) {
        NGIN_ERROR("Failed to load runtime shader files");
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

void RuntimeApplication::on_update(ngin::time_stamp) {
    if (!m_window || !m_window->valid()) {
        m_running = false;
        return;
    }

    while (const std::optional event = m_window->poll_event()) {
        if (event->is_type<ngin::WindowClose>()) {
            NGIN_TRACE("window close event received");
            m_running = false;
            break;
        }

        if (const auto e = event->get_if<ngin::WindowResize>()) {
            NGIN_TRACE("window resized: {}x{}", e->width, e->height);
        }
    }

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader->bind();
    m_pipeline->bind();
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(m_pipeline->get_index_count()),
        GL_UNSIGNED_INT,
        nullptr
    );

    m_window->swap_buffers();
}
