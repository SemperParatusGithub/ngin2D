#pragma once

#include <QElapsedTimer>
#include <QOpenGLWidget>

#include <unordered_set>

#include "core/types.h"
#include "event_queue.h"
#include "graphics/camera.h"
#include "graphics/opengl_context.h"

class Editor;
class QKeyEvent;
class QFocusEvent;
class QTimer;
class QWheelEvent;

class EngineViewport final : public QOpenGLWidget {
public:
    explicit EngineViewport(Editor* editor, QWidget* parent = nullptr);
    ~EngineViewport() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void on_create();
    void on_destroy();
    void on_update(ngin::time_step delta_time);
    void on_ngin_event(const ngin::Event& event);
    void on_render();

private:
    Editor* m_editor = nullptr;
    bool m_gl_initialized = false;
    ngin::u32 m_viewport_width = 1;
    ngin::u32 m_viewport_height = 1;

    ngin::ref<ngin::Camera> m_camera;

    QTimer* m_tick_timer = nullptr;
    QElapsedTimer m_frame_timer;

    ngin::EventQueue m_event_queue;
    std::unordered_set<int> m_keys_held;

    ngin::ref<ngin::OpenGLContext> m_gl_ctx;
};
