#include "graphics/framebuffer.h"

#include "core/assert.h"
#include "core/log.h"

#include <glad/glad.h>

#include <utility>

namespace ngin {

namespace {

bool is_color_format(Framebuffer::TextureFormat format) {
    switch (format) {
		case Framebuffer::TextureFormat::rgba8:
		case Framebuffer::TextureFormat::rgba16f:
		case Framebuffer::TextureFormat::rgba32f:
		case Framebuffer::TextureFormat::r32i:
            return true;
        default:
            return false;
    }
}

bool is_depth_format(Framebuffer::TextureFormat format) {
    switch (format) {
		case Framebuffer::TextureFormat::depth24stencil8:
		case Framebuffer::TextureFormat::depth32f:
            return true;
        default:
            return false;
    }
}

GLenum to_gl_internal_format(Framebuffer::TextureFormat format) {
    switch (format) {
        case Framebuffer::TextureFormat::rgba8:
            return GL_RGBA8;
        case Framebuffer::TextureFormat::rgba16f:
            return GL_RGBA16F;
        case Framebuffer::TextureFormat::rgba32f:
            return GL_RGBA32F;
        case Framebuffer::TextureFormat::r32i:
            return GL_R32I;
        case Framebuffer::TextureFormat::depth24stencil8:
            return GL_DEPTH24_STENCIL8;
        case Framebuffer::TextureFormat::depth32f:
            return GL_DEPTH_COMPONENT32F;
        default:
            return 0;
    }
}

GLenum depth_attachment_point(Framebuffer::TextureFormat format) {
    switch (format) {
        case Framebuffer::TextureFormat::depth24stencil8:
            return GL_DEPTH_STENCIL_ATTACHMENT;
        case Framebuffer::TextureFormat::depth32f:
            return GL_DEPTH_ATTACHMENT;
        default:
            return 0;
    }
}

bool tex_image_pixel_format(Framebuffer::TextureFormat format, GLenum& out_format, GLenum& out_type) {
    switch (format) {
        case Framebuffer::TextureFormat::rgba8:
            out_format = GL_RGBA;
            out_type = GL_UNSIGNED_BYTE;
            return true;
		case Framebuffer::TextureFormat::rgba16f:
        case Framebuffer::TextureFormat::rgba32f:
            out_format = GL_RGBA;
            out_type = GL_FLOAT;
            return true;
        case Framebuffer::TextureFormat::r32i:
            out_format = GL_RED_INTEGER;
            out_type = GL_INT;
            return true;
        case Framebuffer::TextureFormat::depth24stencil8:
            out_format = GL_DEPTH_STENCIL;
            out_type = GL_UNSIGNED_INT_24_8;
            return true;
        case Framebuffer::TextureFormat::depth32f:
            out_format = GL_DEPTH_COMPONENT;
            out_type = GL_FLOAT;
            return true;
        default:
            return false;
    }
}

void set_bound_2d_texture_parameters(Framebuffer::TextureFormat format) {
    const bool integer_color = format == Framebuffer::TextureFormat::r32i;
    const GLenum filter = integer_color ? GL_NEAREST : GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

renderer_id create_attachment_texture(
    u32 width,
    u32 height,
    Framebuffer::TextureFormat format,
    bool multisampled,
    u32 samples
) {
    const GLenum internal = to_gl_internal_format(format);
    NGIN_ASSERT_MSG(internal != 0, "Invalid framebuffer attachment format.");

    renderer_id texture = 0;
    glGenTextures(1, &texture);

    if (multisampled) {
        GLint prev = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &prev);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE,
            static_cast<GLsizei>(samples),
            internal,
            static_cast<GLsizei>(width),
            static_cast<GLsizei>(height),
            GL_TRUE
        );
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, static_cast<GLuint>(prev));
    } else {
        GLenum pix_format = 0;
        GLenum pix_type = 0;
        NGIN_ASSERT_MSG(tex_image_pixel_format(format, pix_format, pix_type), "Unsupported format for 2D texture upload.");

        GLint prev = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            static_cast<GLint>(internal),
            static_cast<GLsizei>(width),
            static_cast<GLsizei>(height),
            0,
            pix_format,
            pix_type,
            nullptr
        );
        set_bound_2d_texture_parameters(format);
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(prev));
    }

    return texture;
}

GLenum framebuffer_texture_target(bool multisampled) {
    return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

} // namespace

Framebuffer::Framebuffer(const Specification& spec) : m_specification(spec) {
    invalidate();
}

Framebuffer::~Framebuffer() {
    release_gpu_resources();
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : m_renderer_id(other.m_renderer_id),
      m_specification(std::move(other.m_specification)),
      m_color_attachments(std::move(other.m_color_attachments)),
      m_depth_attachment(other.m_depth_attachment) {
    other.m_renderer_id = 0;
    other.m_depth_attachment = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    if (this != &other) {
        release_gpu_resources();
        m_renderer_id = other.m_renderer_id;
        m_specification = std::move(other.m_specification);
        m_color_attachments = std::move(other.m_color_attachments);
        m_depth_attachment = other.m_depth_attachment;
        other.m_renderer_id = 0;
        other.m_depth_attachment = 0;
    }
    return *this;
}

void Framebuffer::release_gpu_resources() {
    for (renderer_id id : m_color_attachments) {
        if (id != 0) {
            glDeleteTextures(1, &id);
        }
    }
    m_color_attachments.clear();

    if (m_depth_attachment != 0) {
        glDeleteTextures(1, &m_depth_attachment);
        m_depth_attachment = 0;
    }

    if (m_renderer_id != 0) {
        glDeleteFramebuffers(1, &m_renderer_id);
        m_renderer_id = 0;
    }
}

void Framebuffer::resize(u32 width, u32 height) {
    m_specification.width = width;
    m_specification.height = height;
    invalidate();
}

void Framebuffer::bind() const {
    NGIN_ASSERT_MSG(is_valid(), "Framebuffer is not valid.");
    glBindFramebuffer(GL_FRAMEBUFFER, m_renderer_id);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

renderer_id Framebuffer::get_color_attachment_id(u32 index) const {
    NGIN_ASSERT_MSG(index < m_color_attachments.size(), "Color attachment index out of range.");
    return m_color_attachments[index];
}

renderer_id Framebuffer::get_depth_attachment_id() const {
    return m_depth_attachment;
}

void Framebuffer::invalidate() {
    release_gpu_resources();

    if (m_specification.width == 0 || m_specification.height == 0) {
        return;
    }

    u32 depth_count = 0;
    for (const TextureFormat& att : m_specification.attachments) {
        if (is_depth_format(att)) {
            ++depth_count;
        }
    }
    NGIN_ASSERT_MSG(depth_count <= 1, "Framebuffer supports at most one depth or depth-stencil attachment.");

    u32 samples = m_specification.samples;
    if (m_specification.multisampled) {
        if (samples == 0) {
            samples = 4;
        }
        GLint max_samples = 0;
        glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
        if (static_cast<GLint>(samples) > max_samples) {
            samples = static_cast<u32>(max_samples);
        }
        if (samples == 0) {
            samples = 1;
        }
    } else {
        samples = 1;
    }

    GLint prev_draw_fbo = 0;
    GLint prev_read_fbo = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prev_draw_fbo);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prev_read_fbo);

    glGenFramebuffers(1, &m_renderer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_renderer_id);

    const GLenum tex_target = framebuffer_texture_target(m_specification.multisampled);

    std::vector<GLenum> draw_buffers;
    u32 color_slot = 0;

    for (const TextureFormat& att : m_specification.attachments) {
        if (is_color_format(att)) {
            const renderer_id tex = create_attachment_texture(
                m_specification.width,
                m_specification.height,
                att,
                m_specification.multisampled,
                samples
            );
            const GLenum attach_point = GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(color_slot);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attach_point, tex_target, tex, 0);
            m_color_attachments.push_back(tex);
            draw_buffers.push_back(attach_point);
            ++color_slot;
        } else if (is_depth_format(att)) {
            NGIN_ASSERT_MSG(m_depth_attachment == 0, "Duplicate depth attachment.");
            m_depth_attachment = create_attachment_texture(
                m_specification.width,
                m_specification.height,
                att,
                m_specification.multisampled,
                samples
            );
            const GLenum attach_point = depth_attachment_point(att);
            NGIN_ASSERT_MSG(attach_point != 0, "Invalid depth attachment format.");
            glFramebufferTexture2D(GL_FRAMEBUFFER, attach_point, tex_target, m_depth_attachment, 0);
        }
    }

    if (draw_buffers.empty()) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    } else {
        glDrawBuffers(static_cast<GLsizei>(draw_buffers.size()), draw_buffers.data());
    }

    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        NGIN_ERROR("Framebuffer is incomplete (status {:#x}).", static_cast<unsigned>(status));
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(prev_draw_fbo));
    glBindFramebuffer(GL_READ_FRAMEBUFFER, static_cast<GLuint>(prev_read_fbo));
}

} // namespace ngin
