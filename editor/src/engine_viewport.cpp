#include "engine_viewport.h"

#include "editor.h"

#include <QFocusEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>

#include "core/log.h"
#include "graphics/framebuffer.h"
#include "graphics/renderer.h"
#include "qt_input_adapter.h"

EngineViewport::EngineViewport(Editor* editor, QWidget* parent)
    : QOpenGLWidget(parent),
      m_editor(editor) {
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

    m_gl_initialized = true;
    m_frame_timer.start();
    m_tick_timer->start(16);
}

void EngineViewport::on_destroy() {
    if (!m_gl_initialized) {
        return;
    }

    makeCurrent();
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
    ngin::Renderer::set_viewport(
        0.0f,
        0.0f,
        static_cast<ngin::f32>(m_viewport_width),
        static_cast<ngin::f32>(m_viewport_height)
    );
    ngin::Renderer::set_camera(m_camera);
    ngin::Renderer::set_clear_color({0.12f, 0.13f, 0.18f, 1.0f});
    ngin::Renderer::clear();

    if (m_editor) {
        ngin::Renderer::submit_scene(m_editor->get_scene());
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
    if (!m_gl_initialized || !m_camera || !m_editor) {
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
