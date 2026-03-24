#pragma once

#include "application.h"

class RuntimeApplication : public ngin::Application {
public:
    void on_create() override;
    void on_destroy() override;

    void on_update(float delta_time) override;
    void on_event(ngin::Event& e) override;

private:
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_shader_program = 0;
};
