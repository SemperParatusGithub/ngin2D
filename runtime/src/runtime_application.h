#pragma once

#include "engine.h"

class RuntimeApplication : public ngin::Application {
public:
    void on_create() override;
    void on_destroy() override;
    void on_update(ngin::time_stamp delta_time) override;

private:
    ngin::scope<ngin::Window> m_window;
    ngin::scope<ngin::OpenGLContext> m_context;

    ngin::ref<ngin::Texture> m_texture;
    ngin::ref<ngin::Sprite> m_demo_sprite;
    ngin::ref<ngin::Camera> m_camera;
};
