#pragma once

#include "core/types.h"

#include <filesystem>

namespace ngin {

enum class texture_wrap {
    clamp_to_edge = 0,
    clamp_to_border,
    repeat,
    mirrored_repeat
};

enum class texture_filter {
    linear = 0,
    nearest
};

class Texture {
public:
    Texture() = default;
    Texture(
        const std::filesystem::path& path, 
        texture_wrap wrap = texture_wrap::clamp_to_edge, 
        texture_filter filter = texture_filter::linear,
        bool flip_vertically_on_load = true
    );
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    bool valid() const { return m_renderer_id != 0; }

    bool load_from_file(
		const std::filesystem::path& path,
		texture_wrap wrap = texture_wrap::clamp_to_edge,
		texture_filter filter = texture_filter::linear,
		bool flip_vertically_on_load = true
    );

    void bind(u32 slot = 0) const;

    renderer_id id() const { return m_renderer_id; }

    u32 width() const { return m_width; }
    u32 height() const { return m_height; }

private:
    void release();

private:
    renderer_id m_renderer_id = 0;
    u32 m_width = 0;
    u32 m_height = 0;
};

} // namespace ngin
