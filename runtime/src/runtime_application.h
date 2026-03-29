#pragma once

#include "engine.h"

#include <optional>

class RuntimeApplication : public ngin::Application {
public:
    void on_create() override;
    void on_destroy() override;
    void on_update(ngin::time_stamp delta_time) override;
    void on_render() override;

private:
    ngin::scope<ngin::Window> m_window;
    ngin::scope<ngin::OpenGLContext> m_context;

    ngin::u32 m_viewport_width = 1280;
    ngin::u32 m_viewport_height = 720;

    ngin::ref<ngin::Framebuffer> m_offscreen_fbo;

    ngin::ref<ngin::Texture> m_texture;
    ngin::ref<ngin::Sprite> m_demo_sprite;
    ngin::ref<ngin::Camera> m_camera;
};
