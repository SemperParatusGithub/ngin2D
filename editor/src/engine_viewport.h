#pragma once

#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QSet>

#include "core/types.h"
#include "graphics/camera.h"
#include "graphics/framebuffer.h"
#include "graphics/opengl_context.h"
#include "graphics/texture.h"

class QKeyEvent;
class QTimer;
class QWheelEvent;

class EngineViewport final : public QOpenGLWidget {
public:
    explicit EngineViewport(QWidget* parent = nullptr);
    ~EngineViewport() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void on_create();
    void on_destroy();
    void on_update(ngin::time_stamp delta_time);
    void on_render();

private:
    bool m_gl_initialized = false;
    ngin::u32 m_viewport_width = 1;
    ngin::u32 m_viewport_height = 1;

    ngin::ref<ngin::Texture> m_texture;
    ngin::ref<ngin::Framebuffer> m_offscreen_fbo;
    ngin::ref<ngin::Camera> m_camera;

    QTimer* m_tick_timer = nullptr;
    QElapsedTimer m_frame_timer;
    QSet<int> m_keys_held;

    ngin::ref<ngin::OpenGLContext> m_gl_ctx;
};
