#include "graphics/renderer.h"

#include "core/assert.h"
#include "core/log.h"
#include "graphics/framebuffer.h"
#include "graphics/sprite.h"
#include "graphics/texture.h"
#include "scene/scene.h"
#include "scene/components/sprite_component.h"
#include "scene/components/transform_component.h"

#include <entt/entt.hpp>

#include "graphics/camera.h"
#include "transform.h"

#include <array>
#include <filesystem>

#include <glm/glm.hpp>

#include <glad/glad.h>

namespace ngin {

namespace {

const std::filesystem::path k_quad_vert = "assets/shaders/quad.vert";
const std::filesystem::path k_quad_frag = "assets/shaders/quad.frag";
const std::filesystem::path k_circle_vert = "assets/shaders/circle.vert";
const std::filesystem::path k_circle_frag = "assets/shaders/circle.frag";

struct RenderData {
    ref<GraphicsPipeline> triangle_pipeline;
    ref<GraphicsPipeline> quad_pipeline;
    ref<Shader> quad_shader;
    ref<Shader> circle_shader;
    ref<Camera> camera;
};

scope<RenderData> s_render_data;

glm::mat4 projection_view_matrix() {
    if (s_render_data && s_render_data->camera) {
        return s_render_data->camera->get_projection_view_matrix();
    }
    return glm::mat4(1.0f);
}

void draw_indexed(const ref<GraphicsPipeline>& pipeline) {
    pipeline->bind();
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(pipeline->get_index_count()),
        GL_UNSIGNED_INT,
        nullptr
    );
}

} // namespace

void Renderer::initialize() {
    s_render_data = create_scope<RenderData>();

    constexpr std::array<f32, 20> quad_vertices = {
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f
    };
    constexpr std::array<u32, 6> quad_indices = {0, 1, 2, 2, 3, 0};

    const GraphicsPipeline::Layout quad_layout = {
        {
            {"a_position", VertexFormat::float3, false},
            {"a_tex_coord", VertexFormat::float2, false}
        }
    };

    s_render_data->quad_pipeline = create_ref<GraphicsPipeline>(
        std::span<const f32>(quad_vertices),
        std::span<const u32>(quad_indices),
        quad_layout
    );

    constexpr std::array<f32, 15> triangle_vertices = {
        -0.5f, -0.4330127f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.4330127f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.4330127f, 0.0f, 0.5f, 1.0f
    };
    constexpr std::array<u32, 3> triangle_indices = {0, 1, 2};

    s_render_data->triangle_pipeline = create_ref<GraphicsPipeline>(
        std::span<const f32>(triangle_vertices),
        std::span<const u32>(triangle_indices),
        quad_layout
    );

    s_render_data->quad_shader = create_ref<Shader>();
    if (!s_render_data->quad_shader->load_from_files(k_quad_vert, k_quad_frag)
        || !s_render_data->quad_shader->is_valid()) {
        NGIN_ERROR("Renderer: failed to load quad shader");
    }

    s_render_data->circle_shader = create_ref<Shader>();
    if (!s_render_data->circle_shader->load_from_files(k_circle_vert, k_circle_frag)
        || !s_render_data->circle_shader->is_valid()) {
        NGIN_ERROR("Renderer: failed to load circle shader");
    }

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::release() {
    s_render_data.reset();
}

void Renderer::set_clear_color(const glm::vec4& clear_color) {
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::clear_magenta() {
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::set_viewport(f32 x, f32 y, f32 width, f32 height) {
    glViewport(
        static_cast<GLint>(x),
        static_cast<GLint>(y),
        static_cast<GLsizei>(width),
        static_cast<GLsizei>(height)
    );
}

void Renderer::set_camera(ref<Camera> camera) {
    if (!s_render_data) {
        return;
    }
    s_render_data->camera = std::move(camera);
}

void Renderer::remove_camera() {
    if (!s_render_data) {
        return;
    }
    s_render_data->camera.reset();
}

void Renderer::submit_triangle(const Transform& transform, const glm::vec4& color) {
    NGIN_ASSERT_MSG(s_render_data, "Renderer not initialized.");
    NGIN_ASSERT_MSG(s_render_data->quad_shader && s_render_data->quad_shader->is_valid(), "Quad shader not loaded.");
    NGIN_ASSERT_MSG(s_render_data->triangle_pipeline, "Triangle pipeline missing.");

    s_render_data->quad_shader->bind();
    s_render_data->quad_shader->set_uniform_mat4("u_model", transform.get_transformation_matrix());
    s_render_data->quad_shader->set_uniform_mat4("u_view_projection", projection_view_matrix());
    s_render_data->quad_shader->set_uniform_i32("u_use_sprite_uvs", 0);
    s_render_data->quad_shader->set_uniform_i32("u_use_texture", 0);
    s_render_data->quad_shader->set_uniform_vec4("u_color", color);
    s_render_data->quad_shader->set_uniform_vec4("u_tint", glm::vec4(1.0f));

    draw_indexed(s_render_data->triangle_pipeline);
}

void Renderer::submit_triangle_with_texture(const Transform& transform, const ref<Texture>& texture, const glm::vec4& tint) {
    NGIN_ASSERT_MSG(s_render_data, "Renderer not initialized.");
    NGIN_ASSERT_MSG(texture && texture->is_valid(), "Invalid texture.");
    NGIN_ASSERT_MSG(s_render_data->quad_shader && s_render_data->quad_shader->is_valid(), "Quad shader not loaded.");
    NGIN_ASSERT_MSG(s_render_data->triangle_pipeline, "Triangle pipeline missing.");

    s_render_data->quad_shader->bind();
    s_render_data->quad_shader->set_uniform_mat4("u_model", transform.get_transformation_matrix());
    s_render_data->quad_shader->set_uniform_mat4("u_view_projection", projection_view_matrix());
    s_render_data->quad_shader->set_uniform_i32("u_use_sprite_uvs", 0);
    s_render_data->quad_shader->set_uniform_i32("u_use_texture", 1);
    s_render_data->quad_shader->set_uniform_vec4("u_color", glm::vec4(1.0f));
    s_render_data->quad_shader->set_uniform_vec4("u_tint", tint);

    texture->bind(0);
    s_render_data->quad_shader->set_uniform_i32("u_texture", 0);

    draw_indexed(s_render_data->triangle_pipeline);
}

void Renderer::submit_quad(const Transform& transform, const glm::vec4& color) {
    NGIN_ASSERT_MSG(s_render_data, "Renderer not initialized.");
    NGIN_ASSERT_MSG(s_render_data->quad_shader && s_render_data->quad_shader->is_valid(), "Quad shader not loaded.");
    NGIN_ASSERT_MSG(s_render_data->quad_pipeline, "Quad pipeline missing.");

    s_render_data->quad_shader->bind();
    s_render_data->quad_shader->set_uniform_mat4("u_model", transform.get_transformation_matrix());
    s_render_data->quad_shader->set_uniform_mat4("u_view_projection", projection_view_matrix());
    s_render_data->quad_shader->set_uniform_i32("u_use_sprite_uvs", 0);
    s_render_data->quad_shader->set_uniform_i32("u_use_texture", 0);
    s_render_data->quad_shader->set_uniform_vec4("u_color", color);
    s_render_data->quad_shader->set_uniform_vec4("u_tint", glm::vec4(1.0f));

    draw_indexed(s_render_data->quad_pipeline);
}

void Renderer::submit_circle(const Transform& transform, const glm::vec4& color) {
    NGIN_ASSERT_MSG(s_render_data, "Renderer not initialized.");
    NGIN_ASSERT_MSG(s_render_data->circle_shader && s_render_data->circle_shader->is_valid(), "Circle shader not loaded.");
    NGIN_ASSERT_MSG(s_render_data->quad_pipeline, "Quad pipeline missing.");

    s_render_data->circle_shader->bind();
    s_render_data->circle_shader->set_uniform_mat4("u_model", transform.get_transformation_matrix());
    s_render_data->circle_shader->set_uniform_mat4("u_view_projection", projection_view_matrix());
    s_render_data->circle_shader->set_uniform_i32("u_use_texture", 0);
    s_render_data->circle_shader->set_uniform_vec4("u_color", color);
    s_render_data->circle_shader->set_uniform_vec4("u_tint", glm::vec4(1.0f));

    draw_indexed(s_render_data->quad_pipeline);
}

void Renderer::submit_circle_with_texture(const Transform& transform, const ref<Texture>& texture, const glm::vec4& tint) {
    NGIN_ASSERT_MSG(s_render_data, "Renderer not initialized.");
    NGIN_ASSERT_MSG(texture && texture->is_valid(), "Invalid texture.");
    NGIN_ASSERT_MSG(s_render_data->circle_shader && s_render_data->circle_shader->is_valid(), "Circle shader not loaded.");
    NGIN_ASSERT_MSG(s_render_data->quad_pipeline, "Quad pipeline missing.");

    s_render_data->circle_shader->bind();
    s_render_data->circle_shader->set_uniform_mat4("u_model", transform.get_transformation_matrix());
    s_render_data->circle_shader->set_uniform_mat4("u_view_projection", projection_view_matrix());
    s_render_data->circle_shader->set_uniform_i32("u_use_texture", 1);
    s_render_data->circle_shader->set_uniform_vec4("u_color", glm::vec4(1.0f));
    s_render_data->circle_shader->set_uniform_vec4("u_tint", tint);

    texture->bind(0);
    s_render_data->circle_shader->set_uniform_i32("u_texture", 0);

    draw_indexed(s_render_data->quad_pipeline);
}

void Renderer::submit_pipeline_with_shader(const ref<GraphicsPipeline>& pipeline, const ref<Shader>& shader) {
    NGIN_ASSERT_MSG(s_render_data, "Renderer not initialized.");
    NGIN_ASSERT_MSG(pipeline && shader && shader->is_valid(), "Invalid pipeline or shader.");

    shader->bind();
    draw_indexed(pipeline);
}

void Renderer::submit_quad_with_texture(const Transform& transform, const ref<Texture>& texture, const glm::vec4& tint) {
    NGIN_ASSERT_MSG(s_render_data, "Renderer not initialized.");
    NGIN_ASSERT_MSG(texture && texture->is_valid(), "Invalid texture.");
    NGIN_ASSERT_MSG(s_render_data->quad_shader && s_render_data->quad_shader->is_valid(), "Quad shader not loaded.");
    NGIN_ASSERT_MSG(s_render_data->quad_pipeline, "Quad pipeline missing.");

    s_render_data->quad_shader->bind();
    s_render_data->quad_shader->set_uniform_mat4("u_model", transform.get_transformation_matrix());
    s_render_data->quad_shader->set_uniform_mat4("u_view_projection", projection_view_matrix());
    s_render_data->quad_shader->set_uniform_i32("u_use_sprite_uvs", 0);
    s_render_data->quad_shader->set_uniform_i32("u_use_texture", 1);
    s_render_data->quad_shader->set_uniform_vec4("u_color", glm::vec4(1.0f));
    s_render_data->quad_shader->set_uniform_vec4("u_tint", tint);

    texture->bind(0);
    s_render_data->quad_shader->set_uniform_i32("u_texture", 0);

    draw_indexed(s_render_data->quad_pipeline);
}

void Renderer::submit_quad_with_framebuffer(
    const Transform& transform,
    const Framebuffer& framebuffer,
    u32 color_attachment_index,
    const glm::vec4& tint
) {
    NGIN_ASSERT_MSG(s_render_data, "Renderer not initialized.");
    NGIN_ASSERT_MSG(framebuffer.is_valid(), "Invalid framebuffer.");
    NGIN_ASSERT_MSG(s_render_data->quad_shader && s_render_data->quad_shader->is_valid(), "Quad shader not loaded.");
    NGIN_ASSERT_MSG(s_render_data->quad_pipeline, "Quad pipeline missing.");

    const renderer_id tex_id = framebuffer.get_color_attachment_id(color_attachment_index);
    NGIN_ASSERT_MSG(tex_id != 0, "Framebuffer has no color attachment at the given index.");

    s_render_data->quad_shader->bind();
    s_render_data->quad_shader->set_uniform_mat4("u_model", transform.get_transformation_matrix());
    s_render_data->quad_shader->set_uniform_mat4("u_view_projection", projection_view_matrix());
    s_render_data->quad_shader->set_uniform_i32("u_use_sprite_uvs", 0);
    s_render_data->quad_shader->set_uniform_i32("u_use_texture", 1);
    s_render_data->quad_shader->set_uniform_vec4("u_color", glm::vec4(1.0f));
    s_render_data->quad_shader->set_uniform_vec4("u_tint", tint);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    s_render_data->quad_shader->set_uniform_i32("u_texture", 0);

    draw_indexed(s_render_data->quad_pipeline);
}

void Renderer::submit_framebuffer_fullscreen(
    const Framebuffer& framebuffer,
    u32 color_attachment_index,
    const glm::vec4& tint
) {
    NGIN_ASSERT_MSG(s_render_data, "Renderer not initialized.");
    NGIN_ASSERT_MSG(framebuffer.is_valid(), "Invalid framebuffer.");
    NGIN_ASSERT_MSG(s_render_data->quad_shader && s_render_data->quad_shader->is_valid(), "Quad shader not loaded.");
    NGIN_ASSERT_MSG(s_render_data->quad_pipeline, "Quad pipeline missing.");

    const renderer_id tex_id = framebuffer.get_color_attachment_id(color_attachment_index);
    NGIN_ASSERT_MSG(tex_id != 0, "Framebuffer has no color attachment at the given index.");

    Transform ndc_quad;
    ndc_quad.set_scale(glm::vec3(2.0f, 2.0f, 1.0f));

    s_render_data->quad_shader->bind();
    s_render_data->quad_shader->set_uniform_mat4("u_model", ndc_quad.get_transformation_matrix());
    s_render_data->quad_shader->set_uniform_mat4("u_view_projection", glm::mat4(1.0f));
    s_render_data->quad_shader->set_uniform_i32("u_use_sprite_uvs", 0);
    s_render_data->quad_shader->set_uniform_i32("u_use_texture", 1);
    s_render_data->quad_shader->set_uniform_vec4("u_color", glm::vec4(1.0f));
    s_render_data->quad_shader->set_uniform_vec4("u_tint", tint);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    s_render_data->quad_shader->set_uniform_i32("u_texture", 0);

    draw_indexed(s_render_data->quad_pipeline);
}

void Renderer::submit_sprite(
    const TransformComponent& transform_component,
    const SpriteComponent& sprite_component
) {
    NGIN_ASSERT_MSG(s_render_data, "Renderer not initialized.");

    if (sprite_component.texture && sprite_component.texture->is_valid()) {
        submit_quad_with_texture(transform_component.transform, sprite_component.texture, sprite_component.color);
        return;
    }

    submit_quad(transform_component.transform, sprite_component.color);
}

void Renderer::submit_scene(const Scene& scene) {
    NGIN_ASSERT_MSG(s_render_data, "Renderer not initialized.");

    const auto& registry = scene.get_registry();
    const auto view = registry.view<const TransformComponent, const SpriteComponent>();
    view.each([](const entt::entity, const TransformComponent& tc, const SpriteComponent& sc) {
        submit_sprite(tc, sc);
    });
}

void Renderer::submit_sprite(const ref<Sprite>& sprite) {
    NGIN_ASSERT_MSG(s_render_data, "Renderer not initialized.");
    NGIN_ASSERT_MSG(sprite, "Invalid sprite.");
    NGIN_ASSERT_MSG(s_render_data->quad_shader && s_render_data->quad_shader->is_valid(), "Quad shader not loaded.");
    NGIN_ASSERT_MSG(s_render_data->quad_pipeline, "Quad pipeline missing.");

    const ref<Texture> texture = sprite->get_texture();
    if (!texture || !texture->is_valid()) {
        NGIN_WARN("submit_sprite: sprite has no valid texture.");
        return;
    }

    const std::array<glm::vec2, 4>& uv = sprite->get_texture_coords();

    s_render_data->quad_shader->bind();
    s_render_data->quad_shader->set_uniform_mat4("u_model", sprite->get_transform().get_transformation_matrix());
    s_render_data->quad_shader->set_uniform_mat4("u_view_projection", projection_view_matrix());
    s_render_data->quad_shader->set_uniform_i32("u_use_sprite_uvs", 1);
    s_render_data->quad_shader->set_uniform_vec2("u_sprite_uv0", uv[0]);
    s_render_data->quad_shader->set_uniform_vec2("u_sprite_uv1", uv[1]);
    s_render_data->quad_shader->set_uniform_vec2("u_sprite_uv2", uv[2]);
    s_render_data->quad_shader->set_uniform_vec2("u_sprite_uv3", uv[3]);

    s_render_data->quad_shader->set_uniform_i32("u_use_texture", 1);
    s_render_data->quad_shader->set_uniform_vec4("u_color", glm::vec4(1.0f));
    s_render_data->quad_shader->set_uniform_vec4("u_tint", glm::vec4(1.0f));

    texture->bind(0);
    s_render_data->quad_shader->set_uniform_i32("u_texture", 0);

    draw_indexed(s_render_data->quad_pipeline);
}

} // namespace ngin
