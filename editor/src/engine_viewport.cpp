#include <glad/glad.h>

#include "engine_viewport.h"

#include <QFocusEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>
#include <filesystem>

#include <glm/glm.hpp>

#include "core/log.h"
#include "graphics/framebuffer.h"
#include "graphics/renderer.h"
#include "qt_input_adapter.h"
#include "transform.h"

EngineViewport::EngineViewport(QWidget* parent) : QOpenGLWidget(parent) {
    m_tick_timer = new QTimer(this);
    QObject::connect(m_tick_timer, &QTimer::timeout, this, [this]() {
        if (!m_gl_initialized) {
            return;
        }
        const ngin::f32 dt = static_cast<ngin::f32>(m_frame_timer.restart()) / 1000.0f;
        on_update(dt);
        update();
    });
}

EngineViewport::~EngineViewport() {
    if (m_tick_timer) {
        m_tick_timer->stop();
    }
    on_destroy();
}

void EngineViewport::on_create() {
    m_viewport_width = static_cast<ngin::u32>(std::max(1, width()));
    m_viewport_height = static_cast<ngin::u32>(std::max(1, height()));

    ngin::Renderer::initialize();

    m_camera = ngin::create_ref<ngin::Camera>(
        m_viewport_width,
        m_viewport_height,
        glm::vec2(
            static_cast<float>(m_viewport_width) * 0.5f,
            static_cast<float>(m_viewport_height) * 0.5f
        )
    );
    ngin::Renderer::set_camera(m_camera);
    ngin::Renderer::set_viewport(
        0.0f,
        0.0f,
        static_cast<ngin::f32>(m_viewport_width),
        static_cast<ngin::f32>(m_viewport_height)
    );

    auto texture_filepath = std::filesystem::path("assets/textures/wall.jpg");
    m_texture = ngin::create_ref<ngin::Texture>();
    if (!m_texture->load_from_file(texture_filepath)) {
        NGIN_ERROR("Failed to load texture {}", texture_filepath.string());
    }

    ngin::Framebuffer::Specification spec;
    spec.width = 400;
    spec.height = 300;
    spec.multisampled = false;
    spec.attachments.push_back(ngin::Framebuffer::TextureFormat::rgba32f);
    m_offscreen_fbo = ngin::create_ref<ngin::Framebuffer>(spec);

    m_gl_initialized = true;
    m_frame_timer.start();
    m_tick_timer->start(16);
}

void EngineViewport::on_destroy() {
    if (!m_gl_initialized) {
        return;
    }

    makeCurrent();
    m_offscreen_fbo.reset();
    m_texture.reset();
    m_camera.reset();
    ngin::Renderer::release();
    m_gl_ctx.reset();
    m_gl_initialized = false;
    m_event_queue.clear();
    m_keys_held.clear();
    doneCurrent();
}

void EngineViewport::on_ngin_event(const ngin::Event& event) {
    if (const auto pressed = event.get_if<ngin::KeyPressed>()) {
        m_keys_held.insert(static_cast<int>(pressed->key_code));
        return;
    }
    if (const auto released = event.get_if<ngin::KeyReleased>()) {
        m_keys_held.erase(static_cast<int>(released->key_code));
        return;
    }
    if (event.get_if<ngin::KeyRepeated>()) {
        return;
    }
    if (const auto scroll = event.get_if<ngin::MouseScroll>()) {
        if (m_camera) {
            constexpr ngin::f32 zoom_base = 1.1f;
            m_camera->zoom(std::pow(zoom_base, scroll->y_offset));
        }
    }
}

void EngineViewport::on_update(ngin::time_step delta_time) {
    if (!m_gl_initialized || !m_camera) {
        return;
    }

    while (const std::optional<ngin::Event> e = m_event_queue.pop_next()) {
        on_ngin_event(*e);
    }

    constexpr ngin::f32 camera_speed = 700.0f;
    const ngin::f32 move_delta = camera_speed * delta_time;
    if (m_keys_held.contains(static_cast<int>(ngin::KeyCode::w))) {
        m_camera->move({0.0f, move_delta});
    }
    if (m_keys_held.contains(static_cast<int>(ngin::KeyCode::s))) {
        m_camera->move({0.0f, -move_delta});
    }
    if (m_keys_held.contains(static_cast<int>(ngin::KeyCode::a))) {
        m_camera->move({-move_delta, 0.0f});
    }
    if (m_keys_held.contains(static_cast<int>(ngin::KeyCode::d))) {
        m_camera->move({move_delta, 0.0f});
    }
}

void EngineViewport::on_render() {
    // Pass 1 — matches RuntimeApplication::on_render
    m_offscreen_fbo->bind();
    auto& spec = m_offscreen_fbo->get_specification();
    ngin::Renderer::set_viewport(0.0f, 0.0f, static_cast<ngin::f32>(spec.width), static_cast<ngin::f32>(spec.height));
    ngin::Renderer::remove_camera();
    ngin::Renderer::clear_magenta();
    {
        ngin::Transform t;
        t.set_position({0.0f, 0.0f, 0.0f});
        t.set_scale({0.5f, 0.5f, 0.0f});
        ngin::Renderer::submit_quad_with_texture(t, m_texture);
    }
    m_offscreen_fbo->unbind();

    // Pass 2
    ngin::Renderer::set_viewport(
        0.0f,
        0.0f,
        static_cast<ngin::f32>(m_viewport_width),
        static_cast<ngin::f32>(m_viewport_height)
    );
    ngin::Renderer::set_camera(m_camera);
    ngin::Renderer::set_clear_color({0.8f, 0.8f, 0.8f, 0.8f});
    ngin::Renderer::clear();

    {
        ngin::Transform t;
        t.set_position({0.0f, 0.0f, 0.0f});
        t.set_scale({400.0f, 300.0f, 1.0f});
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
}

void EngineViewport::initializeGL() {
    m_gl_ctx = ngin::create_ref<ngin::OpenGLContext>();
    if (!m_gl_ctx->create_from_qt(this)) {
        m_gl_ctx.reset();
        return;
    }

    NGIN_INFO("Succesfully initialized OpenGL context from qt");

    on_create();
}

void EngineViewport::resizeGL(int w, int h) {
    if (!m_gl_initialized || w <= 0 || h <= 0) {
        return;
    }

    m_viewport_width = static_cast<ngin::u32>(w);
    m_viewport_height = static_cast<ngin::u32>(h);
    if (m_camera) {
        m_camera->set_viewport(m_viewport_width, m_viewport_height);
    }
}

void EngineViewport::focusInEvent(QFocusEvent* event) {
    NGIN_INFO("Editor viewport focused");
    QOpenGLWidget::focusInEvent(event);
}

void EngineViewport::focusOutEvent(QFocusEvent* event) {
    NGIN_INFO("Editor viewport unfocused");
    m_event_queue.clear();
    m_keys_held.clear();
    QOpenGLWidget::focusOutEvent(event);
}

void EngineViewport::paintGL() {
    if (!m_gl_initialized || !m_offscreen_fbo || !m_texture || !m_camera) {
        return;
    }

    ngin::Framebuffer::set_default_framebuffer_binding(
        static_cast<ngin::renderer_id>(defaultFramebufferObject()));

    on_render();
}

void EngineViewport::keyPressEvent(QKeyEvent* event) {
    if (!hasFocus()) {
        QOpenGLWidget::keyPressEvent(event);
        return;
    }

    if (auto ev = ngin::editor::event_from_qt_key_press(event)) {
        m_event_queue.push(std::move(*ev));
        event->accept();
        return;
    }
    QOpenGLWidget::keyPressEvent(event);
}

void EngineViewport::keyReleaseEvent(QKeyEvent* event) {
    if (!hasFocus()) {
        QOpenGLWidget::keyReleaseEvent(event);
        return;
    }

    if (auto ev = ngin::editor::event_from_qt_key_release(event)) {
        m_event_queue.push(std::move(*ev));
        event->accept();
        return;
    }
    QOpenGLWidget::keyReleaseEvent(event);
}

void EngineViewport::wheelEvent(QWheelEvent* event) {
    if (!hasFocus()) {
        QOpenGLWidget::wheelEvent(event);
        return;
    }

    if (auto ev = ngin::editor::event_from_qt_wheel(event)) {
        m_event_queue.push(std::move(*ev));
        event->accept();
        return;
    }
    QOpenGLWidget::wheelEvent(event);
}
