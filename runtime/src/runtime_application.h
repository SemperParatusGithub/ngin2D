#pragma once

#include "application.h"
#include "window.h"
#include "opengl_context.h"
#include "graphics/shader.h"
#include "graphics/graphics_pipeline.h"

#include <memory>

class RuntimeApplication : public ngin::Application {
public:
    void on_create() override;
    void on_destroy() override;
    void on_update(float delta_time) override;

private:
    std::unique_ptr<ngin::Window> m_window;
    std::unique_ptr<ngin::OpenGLContext> m_context;
    std::unique_ptr<ngin::Shader> m_shader;
    std::unique_ptr<ngin::GraphicsPipeline> m_pipeline;
};
