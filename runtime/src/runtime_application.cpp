#include "runtime_application.h"

#include "engine.h"
#include <glad/glad.h>
#include <array>
#include <optional>
#include <span>

namespace {
const char* event_type_to_string(ngin::EventType type) {
    switch (type) {
        case ngin::EventType::none: return "none";
        case ngin::EventType::window_close: return "window_close";
        case ngin::EventType::window_resize: return "window_resize";
        case ngin::EventType::window_move: return "window_move";
        case ngin::EventType::window_refresh: return "window_refresh";
        case ngin::EventType::window_focus: return "window_focus";
        case ngin::EventType::window_iconify: return "window_iconify";
        case ngin::EventType::window_maximize: return "window_maximize";
        case ngin::EventType::window_framebuffer_resize: return "window_framebuffer_resize";
        case ngin::EventType::window_content_scale: return "window_content_scale";
        case ngin::EventType::key_pressed: return "key_pressed";
        case ngin::EventType::key_released: return "key_released";
        case ngin::EventType::key_repeated: return "key_repeated";
        case ngin::EventType::character: return "character";
        case ngin::EventType::character_mods: return "character_mods";
        case ngin::EventType::mouse_button_pressed: return "mouse_button_pressed";
        case ngin::EventType::mouse_button_released: return "mouse_button_released";
        case ngin::EventType::mouse_move: return "mouse_move";
        case ngin::EventType::mouse_enter: return "mouse_enter";
        case ngin::EventType::mouse_scroll: return "mouse_scroll";
        case ngin::EventType::file_drop: return "file_drop";
        default: return "unknown";
    }
}
} // namespace

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
    m_shader = ngin::create_scope<ngin::Shader>(vertex_source, fragment_source);
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
        static_cast<GLsizei>(m_pipeline->index_buffer->count()),
        GL_UNSIGNED_INT,
        nullptr
    );

    m_window->swap_buffers();
}
