#pragma once

#include "core/types.h"

#include <filesystem>
#include <glm/glm.hpp>
#include <string_view>

namespace ngin {

class Shader {
public:
    Shader();
    Shader(std::string_view vertex_source, std::string_view fragment_source);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    bool load_from_source(std::string_view vertex_source, std::string_view fragment_source);
    bool load_from_files(
        const std::filesystem::path& vertex_path,
        const std::filesystem::path& fragment_path
    );

    void set_uniform_i32(std::string_view name, i32 value) const;
    void set_uniform_f32(std::string_view name, f32 value) const;
    void set_uniform_vec2(std::string_view name, const glm::vec2& value) const;
    void set_uniform_vec3(std::string_view name, const glm::vec3& value) const;
    void set_uniform_vec4(std::string_view name, const glm::vec4& value) const;
    void set_uniform_mat4(std::string_view name, const glm::mat4& value) const;

    void bind() const;
    void unbind() const;

    RendererID id() const { return m_renderer_id; }

private:
    i32 get_uniform_location(std::string_view name) const;

    RendererID m_renderer_id;
};

} // namespace ngin
