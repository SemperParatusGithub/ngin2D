#include "runtime_application.h"

#include <filesystem>
#include <optional>

#include <glm/glm.hpp>


#include <GLFW/glfw3.h>


void RuntimeApplication::run() {
	m_running = true;
	on_create();

	auto previous_time = static_cast<ngin::time_stamp>(glfwGetTime());
	while (m_running) {
        ngin::time_stamp current_time = static_cast<ngin::time_stamp>(glfwGetTime());
        ngin::time_step delta_time = current_time - previous_time;
		previous_time = current_time;

		on_update(delta_time);
		on_render();
	}

	on_destroy();
}

void RuntimeApplication::on_create() {
    m_window = ngin::create_scope<ngin::Window>(1280, 720, "ngin2D Runtime - renderer demo", m_event_queue);
    if (!m_window->valid()) {
        m_running = false;
        return;
    }

    m_context = ngin::create_scope<ngin::OpenGLContext>();
    if (!m_context->create_from_glfw(m_window->native_handle())) {
        m_context.reset();
        m_window.reset();
        m_running = false;
        return;
    }
    m_context->set_vsync(true);

    ngin::Input::initialize(m_window->native_handle());

    ngin::Renderer::initialize();

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
    m_demo_sprite->set_position(glm::vec3(980.0f, 560.0f, 0.0f));
    m_demo_sprite->set_scale(glm::vec3(100.0f, 75.0f, 1.0f));

    m_camera = ngin::create_ref<ngin::Camera>(
        m_viewport_width,
        m_viewport_height,
        glm::vec2(static_cast<float>(m_viewport_width) * 0.5f, static_cast<float>(m_viewport_height) * 0.5f)
    );
    ngin::Renderer::set_camera(m_camera);
    ngin::Renderer::set_viewport(0.0f, 0.0f, static_cast<ngin::f32>(m_viewport_width), static_cast<ngin::f32>(m_viewport_height));

    ngin::Framebuffer::Specification spec;
    spec.width = 400;
    spec.height = 300;
    spec.multisampled = false;
    spec.attachments.push_back(ngin::Framebuffer::TextureFormat::rgba32f);

    m_offscreen_fbo = ngin::create_ref<ngin::Framebuffer>(spec);
}

void RuntimeApplication::on_destroy() {
    ngin::Input::release();

    ngin::Renderer::remove_camera();
    ngin::Renderer::release();

    m_offscreen_fbo.reset();

    m_demo_sprite.reset();
    m_texture.reset();
    m_camera.reset();

    m_context.reset();
    m_window.reset();
}

void RuntimeApplication::on_update(ngin::time_stamp delta_time) {
    if (!m_window || !m_window->valid()) {
        m_running = false;
        return;
    }

    while (const std::optional event = m_window->poll_event()) {
        on_event(*event);
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
}
void RuntimeApplication::on_event(const ngin::Event& event) {
	if (event.is_type<ngin::WindowClose>()) {
		NGIN_TRACE("window close event received");
		m_running = false;
		return;
	}

	if (const auto e = event.get_if<ngin::WindowResize>()) {
		NGIN_TRACE("window resized: {}x{}", e->width, e->height);
		if (e->width > 0 && e->height > 0) {
			m_viewport_width = static_cast<ngin::u32>(e->width);
			m_viewport_height = static_cast<ngin::u32>(e->height);
			m_camera->set_viewport(m_viewport_width, m_viewport_height);
		}
	}

	if (const auto e = event.get_if<ngin::MouseScroll>()) {
		constexpr ngin::f32 zoom_base = 1.1f;
		const ngin::f32 zoom_factor = std::pow(zoom_base, e->y_offset);
		m_camera->zoom(zoom_factor);
	}
}

void RuntimeApplication::on_render() {
    // Pass 1
    m_offscreen_fbo->bind();
    auto& spec = m_offscreen_fbo->get_specification();
    ngin::Renderer::set_viewport(0.0f, 0.0f, static_cast<ngin::f32>(spec.width), static_cast<ngin::f32>(spec.height));
    ngin::Renderer::remove_camera();
    ngin::Renderer::clear_magenta();
    {
        ngin::Transform t;
        t.set_position({ 0.0f, 0.0f, 0.0f });
        t.set_scale({ 0.5f, 0.5f, 0.0f });
        ngin::Renderer::submit_quad_with_texture(t, m_texture);
    }
    m_offscreen_fbo->unbind();

    // Pass 2: Main Pass
	ngin::Renderer::set_viewport(
		0.0f,
		0.0f,
		static_cast<ngin::f32>(m_viewport_width),
		static_cast<ngin::f32>(m_viewport_height)
	);
    ngin::Renderer::set_camera(m_camera);
    ngin::Renderer::set_clear_color({ 0.0f, 0.0f, 0.0f, 0.0f });
    ngin::Renderer::clear();

    {
        ngin::Transform t;
        t.set_position({ 0.0f, 0.0f, 0.0f });
        t.set_scale({ 400.0f, 300.0f,1.0f });
		ngin::Renderer::submit_quad_with_framebuffer(t, *m_offscreen_fbo, 0);
    }
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
