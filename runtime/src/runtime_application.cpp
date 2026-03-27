#include "runtime_application.h"

#include "engine.h"
#include <glad/glad.h>
#include <array>
#include <cmath>
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
        640.0f, 420.0f, 0.0f,
        560.0f, 300.0f, 0.0f,
        720.0f, 300.0f, 0.0f
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
    if (!shader_loaded || !m_shader->is_valid()) {
        NGIN_ERROR("Failed to load runtime shader files");
        m_running = false;
        return;
    }

    m_camera = ngin::create_scope<ngin::Camera>(
        1280,
        720,
        glm::vec2(640.0f, 360.0f)
    );
    glViewport(0, 0, 1280, 720);
}

void RuntimeApplication::on_destroy() {
    m_pipeline.reset();
    m_shader.reset();
    m_camera.reset();

    set_native_window_handle(nullptr);
    m_context.reset();
    m_window.reset();
}

void RuntimeApplication::on_update(ngin::time_stamp delta_time) {
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
            if (e->width > 0 && e->height > 0) {
                glViewport(0, 0, e->width, e->height);
                m_camera->set_viewport(
                    static_cast<ngin::u32>(e->width),
                    static_cast<ngin::u32>(e->height)
                );
            }
        }

        if (const auto e = event->get_if<ngin::MouseScroll>()) {
            constexpr ngin::f32 zoom_base = 1.1f;
            const ngin::f32 zoom_factor =
                std::pow(zoom_base, e->y_offset);
            m_camera->zoom(zoom_factor);
        }
    }

    constexpr ngin::f32 camera_speed = 400.0f;
    const ngin::f32 move_delta = camera_speed * delta_time;
    if (ngin::Input::is_key_pressed(ngin::KeyCode::w)) {
        m_camera->move({0.0f, move_delta});
    }
    if (ngin::Input::is_key_pressed(ngin::KeyCode::s)) {
        m_camera->move({0.0f, -move_delta});
    }
    if (ngin::Input::is_key_pressed(ngin::KeyCode::a)) {
        m_camera->move({-move_delta, 0.0f});
    }
    if (ngin::Input::is_key_pressed(ngin::KeyCode::d)) {
        m_camera->move({move_delta, 0.0f});
    }

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader->bind();
    m_shader->set_uniform_mat4("u_view_projection", m_camera->get_projection_view_matrix());
    m_pipeline->bind();
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(m_pipeline->get_index_count()),
        GL_UNSIGNED_INT,
        nullptr
    );

    m_window->swap_buffers();
}
