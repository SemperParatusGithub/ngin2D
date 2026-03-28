#pragma once

#include "core/types.h"
#include "graphics/graphics_pipeline.h"
#include "graphics/shader.h"

#include <glm/glm.hpp>

namespace ngin {

class Sprite;
class Transform;
class Texture;
class Camera;

class Renderer {
public:
    static void init();
    static void release();

    static void set_clear_color(const glm::vec4& clear_color);
    static void clear();
    static void clear_magenta();

    static void set_viewport(f32 x, f32 y, f32 width, f32 height);
    /// When set, submit_* uses the camera projection×view matrix for `u_view_projection`.
    /// If no camera is set (or after remove_camera), `u_view_projection` is the identity matrix.
    static void set_camera(ref<Camera> camera);
    static void remove_camera();

    static void submit_triangle(const Transform& transform, const glm::vec4& color);
    static void submit_triangle_with_texture(
        const Transform& transform,
        const ref<Texture>& texture,
        const glm::vec4& tint = glm::vec4(1.0f)
    );

    static void submit_quad(const Transform& transform, const glm::vec4& color);
    static void submit_circle(const Transform& transform, const glm::vec4& color);
    static void submit_circle_with_texture(
        const Transform& transform,
        const ref<Texture>& texture,
        const glm::vec4& tint = glm::vec4(1.0f)
    );

    static void submit_pipeline_with_shader(const ref<GraphicsPipeline>& pipeline, const ref<Shader>& shader);
    static void submit_quad_with_texture(
        const Transform& transform,
        const ref<Texture>& texture,
        const glm::vec4& tint = glm::vec4(1.0f)
    );
    static void submit_sprite(const ref<Sprite>& sprite);
};

} // namespace ngin
