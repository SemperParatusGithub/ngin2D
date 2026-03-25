#pragma once

#include "application.h"
#include "window.h"
#include "opengl_context.h"
#include "graphics/shader.h"
#include "graphics/graphics_pipeline.h"

class RuntimeApplication : public ngin::Application {
public:
    void on_create() override;
    void on_destroy() override;
    void on_update(ngin::time_stamp delta_time) override;

private:
    ngin::scope<ngin::Window> m_window;
    ngin::scope<ngin::OpenGLContext> m_context;
    ngin::scope<ngin::Shader> m_shader;
    ngin::scope<ngin::GraphicsPipeline> m_pipeline;
};
