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

    bool is_valid() const { return m_renderer_id != 0; }
	renderer_id get_id() const { return m_renderer_id; }

    void bind(u32 slot = 0) const;
    void unbind(u32 slot = 0) const;

	bool load_from_file(
		const std::filesystem::path& path,
		texture_wrap wrap = texture_wrap::clamp_to_edge,
		texture_filter filter = texture_filter::linear,
		bool flip_vertically_on_load = true
	);

    u32 get_width() const { return m_width; }
    u32 get_height() const { return m_height; }

private:
    void release();

private:
    renderer_id m_renderer_id = 0;
    u32 m_width = 0;
    u32 m_height = 0;
};

} // namespace ngin
