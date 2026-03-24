#pragma once

#include "application.h"
#include "window.h"
#include "opengl_context.h"

#include <memory>

class RuntimeApplication : public ngin::Application {
public:
    void on_create() override;
    void on_destroy() override;

    void on_update(float delta_time) override;
    void on_event(ngin::Event& e) override;

private:
    std::unique_ptr<ngin::Window> m_window;
    std::unique_ptr<ngin::OpenGLContext> m_context;
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_shader_program = 0;
};
