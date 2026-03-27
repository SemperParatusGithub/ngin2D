#include "graphics/texture.h"

#include "core/assert.h"
#include "core/log.h"

#include <glad/glad.h>

#include <utility>

// stb_image is a single-header library; we compile its implementation in this TU.
#define STB_IMAGE_IMPLEMENTATION
#include "../../third_party/stb/stb_image.h"

namespace {
static GLint convert_texture_wrap(ngin::texture_wrap wrap) {
    switch (wrap) {
		case ngin::texture_wrap::clamp_to_edge:     return GL_CLAMP_TO_EDGE;
		case ngin::texture_wrap::clamp_to_border:   return GL_CLAMP_TO_BORDER;
		case ngin::texture_wrap::repeat:            return GL_REPEAT;
		case ngin::texture_wrap::mirrored_repeat:   return GL_MIRRORED_REPEAT;
		default: NGIN_ASSERT_MSG(false, "Unknown texture_wrap type"); return 0;
    }
}
static GLint convert_texture_filter(ngin::texture_filter filter) {
    switch (filter) {
		case ngin::texture_filter::linear:  return GL_LINEAR;
		case ngin::texture_filter::nearest: return GL_NEAREST;
        default: NGIN_ASSERT_MSG(false, "Unknown texture_filter type"); return 0;
    }
}
}

namespace ngin {

Texture::Texture(
	const std::filesystem::path& path,
	texture_wrap wrap,
	texture_filter filter,
	bool flip_vertically_on_load
) {
    load_from_file(path, wrap, filter, flip_vertically_on_load);
}

Texture::~Texture() {
    release();
}

Texture::Texture(Texture&& other) noexcept
    : m_renderer_id(other.m_renderer_id), m_width(other.m_width), m_height(other.m_height) {
    other.m_renderer_id = 0;
    other.m_width = 0;
    other.m_height = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        release();
        m_renderer_id = other.m_renderer_id;
        m_width = other.m_width;
        m_height = other.m_height;

        other.m_renderer_id = 0;
        other.m_width = 0;
        other.m_height = 0;
    }
    return *this;
}

void Texture::release() {
    if (m_renderer_id != 0) {
        glDeleteTextures(1, &m_renderer_id);
        m_renderer_id = 0;
        m_width = 0;
        m_height = 0;
    }
}

bool Texture::load_from_file(
	const std::filesystem::path& path,
	texture_wrap wrap,
	texture_filter filter,
	bool flip_vertically_on_load
) {
    release();

    stbi_set_flip_vertically_on_load(flip_vertically_on_load ? 1 : 0);

    int width = 0;
    int height = 0;
    int channels = 0;

    // Force RGBA output so we can consistently upload GL_RGBA.
    unsigned char* const data =
        stbi_load(path.string().c_str(), &width, &height, &channels, 4);

    if (!data) {
        NGIN_ERROR("Failed to load texture '{}'", path.string());
        return false;
    }

    m_width = static_cast<u32>(width);
    m_height = static_cast<u32>(height);

    glGenTextures(1, &m_renderer_id);
    glBindTexture(GL_TEXTURE_2D, m_renderer_id);

    // Sampling parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convert_texture_wrap(wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convert_texture_wrap(wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, convert_texture_filter(filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, convert_texture_filter(filter));

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    return true;
}

void Texture::bind(u32 slot) const {
    NGIN_ASSERT_MSG(valid(), "Trying to bind an invalid texture.");
    glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(slot));
    glBindTexture(GL_TEXTURE_2D, m_renderer_id);
}

} // namespace ngin
