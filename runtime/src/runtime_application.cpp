#include "runtime_application.h"

#include <array>
#include <filesystem>
#include <optional>


void RuntimeApplication::on_create() {
    m_window = ngin::create_scope<ngin::Window>(1280, 720, "ngin2D Runtime - renderer demo");
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

    ngin::Renderer::init();

    m_texture = ngin::create_ref<ngin::Texture>();
    const bool texture_loaded = m_texture->load_from_file(
        std::filesystem::path("assets/textures/wall.jpg")
    );
    if (!texture_loaded || !m_texture->is_valid()) {
        NGIN_ERROR("Failed to load runtime texture");
        m_running = false;
        return;
    }

    m_demo_sprite = ngin::create_ref<ngin::Sprite>(m_texture);
    m_demo_sprite->get_transform().set_position(glm::vec3(980.0f, 560.0f, 0.0f));
    m_demo_sprite->get_transform().set_scale(glm::vec3(100.0f, 75.0f, 1.0f));

    m_camera = ngin::create_ref<ngin::Camera>(
        1280,
        720,
        glm::vec2(640.0f, 360.0f)
    );
    ngin::Renderer::set_camera(m_camera);
    ngin::Renderer::set_viewport(0.0f, 0.0f, 1280.0f, 720.0f);
}

void RuntimeApplication::on_destroy() {
    ngin::Renderer::remove_camera();
    ngin::Renderer::release();

    m_demo_sprite.reset();
    m_texture.reset();
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
                ngin::Renderer::set_viewport(
                    0.0f,
                    0.0f,
                    static_cast<ngin::f32>(e->width),
                    static_cast<ngin::f32>(e->height)
                );
                m_camera->set_viewport(
                    static_cast<ngin::u32>(e->width),
                    static_cast<ngin::u32>(e->height)
                );
            }
        }

        if (const auto e = event->get_if<ngin::MouseScroll>()) {
            constexpr ngin::f32 zoom_base = 1.1f;
            const ngin::f32 zoom_factor = std::pow(zoom_base, e->y_offset);
            m_camera->zoom(zoom_factor);
        }
    }

    constexpr ngin::f32 camera_speed = 700.0f;
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

    ngin::Renderer::set_clear_color(glm::vec4(0.08f, 0.09f, 0.12f, 1.0f));
    ngin::Renderer::clear();

    // Backdrop: 10×10 low-opacity quads with diagonal color fade
    {
        constexpr int grid_n = 10;
        constexpr ngin::f32 gx0 = 70.0f;
        constexpr ngin::f32 gy0 = 50.0f;
        constexpr ngin::f32 gx1 = 1210.0f;
        constexpr ngin::f32 gy1 = 670.0f;
        constexpr ngin::f32 gutter = 0.92f;
        const ngin::f32 cell_w = (gx1 - gx0) / static_cast<ngin::f32>(grid_n);
        const ngin::f32 cell_h = (gy1 - gy0) / static_cast<ngin::f32>(grid_n);
        const glm::vec3 fade_a(0.1f, 0.14f, 0.28f);
        const glm::vec3 fade_b(0.38f, 0.2f, 0.42f);
        constexpr ngin::f32 base_alpha = 0.4f;

        for (int j = 0; j < grid_n; ++j) {
            for (int i = 0; i < grid_n; ++i) {
                const ngin::f32 cx = gx0 + (static_cast<ngin::f32>(i) + 0.5f) * cell_w;
                const ngin::f32 cy = gy0 + (static_cast<ngin::f32>(j) + 0.5f) * cell_h;
                const ngin::f32 t =
                    static_cast<ngin::f32>(i + j) / static_cast<ngin::f32>(2 * (grid_n - 1));
                const glm::vec3 rgb = glm::mix(fade_a, fade_b, t);

                ngin::Transform cell;
                cell.set_position(glm::vec3(cx, cy, 0.0f));
                cell.set_scale(glm::vec3(cell_w * gutter, cell_h * gutter, 1.0f));

                ngin::Renderer::submit_quad(
                    cell,
                    glm::vec4(rgb, base_alpha)
                );
            }
        }
    }

    {
        ngin::Transform t;
        t.set_position(glm::vec3(180.0f, 560.0f, 0.0f));
        t.set_scale(glm::vec3(90.0f, 55.0f, 1.0f));
        ngin::Renderer::submit_quad(t, glm::vec4(0.85f, 0.25f, 0.2f, 1.0f));
    }
    {
        ngin::Transform t;
        t.set_position(glm::vec3(380.0f, 560.0f, 0.0f));
        t.set_scale(glm::vec3(110.0f, 70.0f, 1.0f));
        ngin::Renderer::submit_quad_with_texture(t, m_texture, glm::vec4(1.0f));
    }
    {
        ngin::Transform t;
        t.set_position(glm::vec3(600.0f, 560.0f, 0.0f));
        t.set_scale(glm::vec3(110.0f, 70.0f, 1.0f));
        ngin::Renderer::submit_quad_with_texture(
            t,
            m_texture,
            glm::vec4(0.55f, 0.95f, 1.0f, 1.0f)
        );
    }
    ngin::Renderer::submit_sprite(m_demo_sprite);

    // Row 2 — triangle, circle, raw pipeline + custom shader
    {
        ngin::Transform t;
        t.set_position(glm::vec3(220.0f, 340.0f, 0.0f));
        t.set_scale(glm::vec3(130.0f, 130.0f, 1.0f));
        ngin::Renderer::submit_triangle(t, glm::vec4(0.2f, 0.75f, 0.35f, 1.0f));
    }
    {
        ngin::Transform t;
        t.set_position(glm::vec3(460.0f, 340.0f, 0.0f));
        t.set_scale(glm::vec3(95.0f, 95.0f, 1.0f));
        ngin::Renderer::submit_circle(t, glm::vec4(0.65f, 0.35f, 0.95f, 1.0f));
    }
    {
        ngin::Transform t;
        t.set_position(glm::vec3(220.0f, 140.0f, 0.0f));
        t.set_scale(glm::vec3(100.0f, 100.0f, 1.0f));
        ngin::Renderer::submit_triangle_with_texture(t, m_texture, glm::vec4(1.0f, 0.92f, 0.85f, 1.0f));
    }
    {
        ngin::Transform t;
        t.set_position(glm::vec3(460.0f, 140.0f, 0.0f));
        t.set_scale(glm::vec3(90.0f, 90.0f, 1.0f));
        ngin::Renderer::submit_circle_with_texture(t, m_texture, glm::vec4(1.0f));
    }
    m_window->swap_buffers();
}
