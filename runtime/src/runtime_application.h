#pragma once

#include "engine.h"

#include <optional>

class RuntimeApplication {
public:
    RuntimeApplication() = default;
    ~RuntimeApplication() = default;

    void run();

private:
    void on_create();
    void on_destroy();

    void on_update(ngin::time_stamp delta_time);
    void on_event(const std::optional<ngin::Event>& event);
    void on_render();

private:
    bool m_running = false;

    ngin::EventQueue m_event_queue;
    ngin::scope<ngin::Window> m_window;
    ngin::scope<ngin::OpenGLContext> m_context;

    ngin::u32 m_viewport_width = 1280;
    ngin::u32 m_viewport_height = 720;

    ngin::ref<ngin::Framebuffer> m_offscreen_fbo;

    ngin::ref<ngin::Texture> m_texture;
    ngin::ref<ngin::Sprite> m_demo_sprite;
    ngin::ref<ngin::Camera> m_camera;
};
