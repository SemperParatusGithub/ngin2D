#pragma once

#include "core/types.h"

#include <cstdint>
#include <vector>

namespace ngin {

class Framebuffer {
public:
	enum class TextureFormat {
		rgba8,
		rgba16f,
		rgba32f,
		r32i,
		depth24stencil8,
		depth32f
	};
    struct Specification {
		u32 width = 0;
		u32 height = 0;
		std::vector<TextureFormat> attachments;
		bool multisampled = false;
		u32 samples = 4;
    };
public:
    explicit Framebuffer(const Specification& spec);
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    bool is_valid() const { return m_renderer_id != 0; }
    renderer_id get_id() const { return m_renderer_id; }

    void bind() const;
    void unbind() const;

	void resize(u32 width, u32 height);

    renderer_id get_color_attachment_id(u32 index = 0) const;
    renderer_id get_depth_attachment_id() const;

    const Specification& get_specification() const { return m_specification; }

private:
    void invalidate();
    void release_gpu_resources();

private:
    renderer_id m_renderer_id = 0;
    Specification m_specification;
    std::vector<renderer_id> m_color_attachments;
    renderer_id m_depth_attachment = 0;
};

} // namespace ngin
